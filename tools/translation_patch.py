"""Streaming patch for PSARCs rebuilt by psarc_patch (shared original body).

The patch contains a new TOC and translated appended blocks, never the shared
disc body. No third-party binaries or whole-archive in-memory buffers needed.
"""
import hashlib
import json
import struct
from pathlib import Path

MAGIC = b'KYTRPAT1'
CHUNK = 8 << 20
MAX_LITERAL = 64 << 20


def digest(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def toc_size(stream):
    stream.seek(0)
    header = stream.read(32)
    if len(header) != 32 or header[:12] != b'PSAR\x00\x01\x00\x04zlib':
        raise ValueError('Unsupported PSARC')
    size = struct.unpack_from('>I', header, 12)[0]
    if not 32 <= size <= MAX_LITERAL:
        raise ValueError('Invalid PSARC TOC')
    return size


def build(base, target, output):
    base, target, output = map(Path, (base, target, output))
    if output.exists() or output.resolve() in (base.resolve(), target.resolve()):
        raise ValueError('Choose a fresh patch path')
    before = [(p.stat().st_size, p.stat().st_mtime_ns) for p in (base, target)]
    with base.open('rb') as old, target.open('rb') as new:
        old_toc, new_toc = toc_size(old), toc_size(new)
        old.seek(old_toc)
        new.seek(new_toc)
        shared = 0
        while True:
            a, b = old.read(CHUNK), new.read(CHUNK)
            if a == b:
                shared += len(a)
                if not a:
                    break
            else:
                shared += next((i for i, (x, y) in enumerate(zip(a, b)) if x != y), min(len(a), len(b)))
                break
        literal_size = target.stat().st_size - shared
        if literal_size > MAX_LITERAL:
            raise ValueError('Archives do not share enough original data; refusing a full archive payload')
        info = dict(base_sha256=digest(base), base_bytes=base.stat().st_size,
                    target_sha256=digest(target), target_bytes=target.stat().st_size,
                    base_offset=old_toc, copy_bytes=shared, toc_bytes=new_toc)
        encoded = json.dumps(info, sort_keys=True).encode()
        with output.open('xb') as patch:
            patch.write(MAGIC + struct.pack('<I', len(encoded)) + encoded)
            new.seek(0)
            patch.write(new.read(new_toc))
            new.seek(new_toc + shared)
            while block := new.read(CHUNK):
                patch.write(block)
    if before != [(p.stat().st_size, p.stat().st_mtime_ns) for p in (base, target)]:
        output.unlink()
        raise ValueError('Source changed while generating the patch')
    return info


def header(stream):
    if stream.read(8) != MAGIC:
        raise ValueError('Invalid translation patch')
    raw = stream.read(4)
    if len(raw) != 4:
        raise ValueError('Truncated patch')
    size = struct.unpack('<I', raw)[0]
    if not 1 <= size <= 4096:
        raise ValueError('Invalid patch header size')
    info = json.loads(stream.read(size))
    for name in ('base_bytes', 'target_bytes', 'base_offset', 'copy_bytes', 'toc_bytes'):
        if type(info.get(name)) is not int or info[name] < 0:
            raise ValueError('Invalid patch ranges')
    if (info['base_offset'] + info['copy_bytes'] > info['base_bytes'] or
            info['toc_bytes'] + info['copy_bytes'] > info['target_bytes'] or
            not 32 <= info['toc_bytes'] <= MAX_LITERAL or
            info['target_bytes'] - info['copy_bytes'] > MAX_LITERAL):
        raise ValueError('Patch copy range outside archive')
    for name in ('base_sha256', 'target_sha256'):
        value = info.get(name, '')
        if len(value) != 64 or any(c not in '0123456789abcdef' for c in value):
            raise ValueError('Invalid patch hash')
    return info


def copy_exact(source, target, length):
    while length:
        block = source.read(min(CHUNK, length))
        if not block:
            raise ValueError('Truncated patch or base')
        target.write(block)
        length -= len(block)


def apply(base, patch, output):
    base, patch, output = map(Path, (base, patch, output))
    if output.exists() or output.resolve() in (base.resolve(), patch.resolve()):
        raise ValueError('Output must be a fresh file')
    try:
        with patch.open('rb') as changes, base.open('rb') as original:
            info = header(changes)
            if base.stat().st_size != info['base_bytes'] or digest(base) != info['base_sha256']:
                raise ValueError('Installed English archive is not the expected en-16')
            with output.open('xb') as target:
                copy_exact(changes, target, info['toc_bytes'])
                original.seek(info['base_offset'])
                copy_exact(original, target, info['copy_bytes'])
                copy_exact(changes, target, info['target_bytes'] - info['toc_bytes'] - info['copy_bytes'])
                if changes.read(1):
                    raise ValueError('Unexpected trailing patch data')
            if output.stat().st_size != info['target_bytes'] or digest(output) != info['target_sha256']:
                raise ValueError('Reconstructed archive hash mismatch')
    except BaseException:
        output.unlink(missing_ok=True)
        raise
    return info
