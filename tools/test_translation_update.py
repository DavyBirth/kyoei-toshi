"""Regression checks for archive reconstruction and recoverable installation."""
import json
from pathlib import Path
import struct
import tempfile
import unittest
from unittest.mock import patch
import os
import shutil
import subprocess
import sys

import apply_translation_update as updater
from translation_patch import apply, build, digest


def archive_bytes(toc, body, tail):
    return struct.pack('>4sI4s5I', b'PSAR', 0x10004, b'zlib', toc, 30, 1, 65536, 13) + b'T' * (toc - 32) + body + tail


class TranslationUpdate(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.work = Path(self.temp.name)
        self.base = self.work / 'old.psarc'
        self.target = self.work / 'new.psarc'
        body = bytes(range(256)) * 4096
        self.base.write_bytes(archive_bytes(40, body, b'OLD translated text'))
        self.target.write_bytes(archive_bytes(54, body, b'NEW natural English text'))
        self.delta = self.work / 'common.patch'
        self.info = build(self.base, self.target, self.delta)

    def package(self):
        root = self.work / 'game'
        package = self.work / 'package'
        root.mkdir()
        package.mkdir()
        (root / 'runtime').mkdir()
        (root / 'runtime/kyoei.exe').write_bytes(b'GoW3 executable fixture')
        current = root / updater.ARCHIVE
        current.parent.mkdir(parents=True)
        current.write_bytes(self.base.read_bytes())
        (package / 'common.patch').write_bytes(self.delta.read_bytes())
        installed, rows = [], []
        for name in sorted(updater.FILES):
            target = root / name
            payload = package / 'payload' / name
            payload.parent.mkdir(parents=True, exist_ok=True)
            payload.write_bytes(('new ' + name).encode())
            # Cover both replacement and creation of previously absent assets.
            if name.endswith('.exe') or name.endswith('.kypack'):
                target.parent.mkdir(parents=True, exist_ok=True)
                target.write_bytes(('old ' + name).encode())
                installed.append(dict(path=name, sha256=digest(target)))
            rows.append(dict(path=name, bytes=payload.stat().st_size, sha256=digest(payload)))
        (root / 'manifest.json').write_text(json.dumps(dict(title='CUSA03562', version='01.03', files=installed)))
        (package / 'translation-update.json').write_text(json.dumps(dict(
            schema=1, release=updater.RELEASE, title='CUSA03562', version='01.03',
            required_runtime_sha256=digest(root / 'runtime/kyoei.exe'),
            archive=self.info, patch_sha256=digest(package / 'common.patch'), files=rows)))
        for name in ('data/user/save.bin', 'data/settings.txt', 'data/cache/test', 'data/wineprefix/test', 'game/unchanged.psarc'):
            p = root / name
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_bytes(b'unique player data')
        return root, package

    def snapshot(self, root):
        return {p.relative_to(root).as_posix(): p.read_bytes() for p in root.rglob('*')
                if p.is_file() and p.name != 'session.lock'}

    def test_reconstruct_exact_archive(self):
        result = self.work / 'output.psarc'
        apply(self.base, self.delta, result)
        self.assertEqual(result.read_bytes(), self.target.read_bytes())
        self.assertLess(self.delta.stat().st_size, 4096)
        self.assertGreaterEqual(self.info['copy_bytes'], 1 << 20)

    def test_wrong_base_never_creates_output(self):
        self.base.write_bytes(b'unexpected file')
        result = self.work / 'output.psarc'
        with self.assertRaises(ValueError):
            apply(self.base, self.delta, result)
        self.assertFalse(result.exists())

    def test_corrupt_or_truncated_patch(self):
        for data in (self.delta.read_bytes()[:-2], self.delta.read_bytes() + b'junk'):
            self.delta.write_bytes(data)
            result = self.work / 'output.psarc'
            with self.assertRaises(ValueError):
                apply(self.base, self.delta, result)
            self.assertFalse(result.exists())

    def test_existing_output_is_preserved(self):
        result = self.work / 'output.psarc'
        result.write_bytes(b'unique work')
        with self.assertRaises(ValueError):
            apply(self.base, self.delta, result)
        self.assertEqual(result.read_bytes(), b'unique work')

    def test_install_reapply_discard_old_archive_and_preserve_player_files(self):
        root, package = self.package()
        before = self.snapshot(root)
        updater.install(package, root, check_only=True)
        self.assertEqual(self.snapshot(root), before)
        backup = updater.install(package, root)
        self.assertEqual((root / updater.ARCHIVE).read_bytes(), self.target.read_bytes())
        self.assertFalse((backup / 'original' / updater.ARCHIVE).exists())
        self.assertTrue(json.loads((backup / 'transaction.json').read_text())['archive_discarded'])
        self.assertIsNone(updater.install(package, root))
        for name, content in before.items():
            if name not in updater.FILES | {updater.ARCHIVE}:
                self.assertEqual((root / name).read_bytes(), content)
        with self.assertRaisesRegex(ValueError, 'excluída'):
            updater.rollback(root, backup)

    def test_bad_payload_refused_without_writes(self):
        root, package = self.package()
        before = self.snapshot(root)
        (package / 'payload/assets/ui-art.kyart').write_bytes(b'damaged')
        with self.assertRaises(ValueError):
            updater.install(package, root)
        self.assertEqual(self.snapshot(root), before)

    def test_command_line_preflight_creates_no_python_cache(self):
        root, package = self.package()
        for name in ('apply_translation_update.py', 'translation_patch.py', 'apply_runtime_update_linux.py'):
            shutil.copyfile(Path(__file__).parent / name, package / name)
        before = self.snapshot(root), self.snapshot(package)
        result = subprocess.run([sys.executable, str(package / 'apply_translation_update.py'),
                                 '--game', str(root), '--check-only'], capture_output=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual((self.snapshot(root), self.snapshot(package)), before)

    def test_wrong_runtime_refused_without_writes(self):
        root, package = self.package()
        (root / 'runtime/kyoei.exe').write_bytes(b'old engine')
        before = self.snapshot(root)
        with self.assertRaises(ValueError):
            updater.install(package, root)
        self.assertEqual(self.snapshot(root), before)

    def test_failed_commit_restores_every_original(self):
        root, package = self.package()
        before = self.snapshot(root)
        replace = os.replace
        failed = False

        def fail_once(source, target):
            nonlocal failed
            if not failed and 'stage' in Path(source).parts and Path(target) == root / updater.ARCHIVE:
                failed = True
                raise OSError('simulated write failure after archive was backed up')
            return replace(source, target)

        with patch.object(updater.os, 'replace', side_effect=fail_once):
            with self.assertRaises(OSError):
                updater.install(package, root)
        self.assertTrue(failed)
        for name, content in before.items():
            self.assertEqual((root / name).read_bytes(), content)
        for name in updater.FILES - before.keys():
            self.assertFalse((root / name).exists())

    def test_rollback_refuses_overwriting_later_edits(self):
        root, package = self.package()
        # Simulate interruption after installation but before discarding the old archive.
        with patch.object(updater, 'discard_old_archive'):
            backup = updater.install(package, root)
        (root / 'assets/ui-art.kyart').write_bytes(b'new unique user work')
        before = self.snapshot(root)
        with self.assertRaises(ValueError):
            updater.rollback(root, backup)
        self.assertEqual(self.snapshot(root), before)

    def test_reapply_finishes_interrupted_archive_cleanup(self):
        root, package = self.package()
        with patch.object(updater, 'discard_old_archive'):
            backup = updater.install(package, root)
        self.assertTrue((backup / 'original' / updater.ARCHIVE).exists())
        updater.install(package, root)
        self.assertFalse((backup / 'original' / updater.ARCHIVE).exists())
        self.assertEqual((root / updater.ARCHIVE).read_bytes(), self.target.read_bytes())

    def test_paths_cannot_escape_installation(self):
        for name in ('../outside', '/absolute', 'C:/outside', 'data/../../outside', 'data\\outside'):
            with self.assertRaises(ValueError):
                updater.inside(self.work, name)

    def test_insufficient_space_refused_without_writes(self):
        root, package = self.package()
        before = self.snapshot(root)
        with patch.object(updater.shutil, 'disk_usage', return_value=type('Usage', (), {'free': 0})()):
            with self.assertRaises(ValueError):
                updater.install(package, root)
        self.assertEqual(self.snapshot(root), before)

    def test_active_launcher_lock_prevents_installation(self):
        root, package = self.package()
        with updater.session_guard(root):
            before = self.snapshot(root)
            with self.assertRaises((OSError, RuntimeError)):
                updater.install(package, root)
            self.assertEqual(self.snapshot(root), before)

    def test_interrupted_staging_requires_recovery(self):
        root, package = self.package()
        backup = root / 'data/update-backups' / (updater.RELEASE + '-interrupted')
        backup.mkdir(parents=True)
        stage = backup / 'stage'
        stage.mkdir()
        (stage / 'unfinished-output').write_bytes(b'recreatable partial output')
        updater.save_journal(backup / 'transaction.json', dict(
            release=updater.RELEASE, root=str(root.resolve()), status='staging', files=[]))
        before = self.snapshot(root)
        with self.assertRaises(ValueError):
            updater.install(package, root)
        self.assertEqual(self.snapshot(root), before)
        updater.rollback(root, backup)
        self.assertFalse(stage.exists())
        self.assertEqual(json.loads((backup / 'transaction.json').read_text())['status'], 'rolled-back')


if __name__ == '__main__':
    unittest.main()
