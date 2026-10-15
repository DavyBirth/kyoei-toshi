"""Reinsere recursos num PSARC novo, conservando o corpo antigo.

Mudanças ficam em blocos anexados; a TOC aponta para eles. Sem mudanças,
a saída preserva todos os bytes do arquivo original.
"""
import argparse
import hashlib
import json
import struct
import zlib
from pathlib import Path
from psarc import Psarc, safe_name


def pack_blocks(data):
    lengths, packed = [], []
    for pos in range(0, len(data), 65536):
        raw = data[pos:pos+65536]
        compressed = zlib.compress(raw, 9)
        block = compressed if len(compressed) < len(raw) else raw
        lengths.append(len(block) % 65536)
        packed.append(block)
    return lengths, b''.join(packed)


def rebuild_toc(archive, original_toc, changed):
    """New TOC plus appended block bodies; the old body is reused unchanged."""
    if not changed:
        return original_toc, []
    old_toc = archive.header['toc_bytes']
    blocks = list(archive.blocks)
    appended = []
    append_bytes = 0
    entries = []
    for entry in archive.entries:
        if entry.name in changed:
            lengths, packed = pack_blocks(changed[entry.name])
            entries.append((entry.digest, len(blocks), len(changed[entry.name]),
                            archive.archive_size-old_toc+append_bytes))
            blocks.extend(lengths)
            appended.append(packed)
            append_bytes += len(packed)
        else:
            entries.append((entry.digest, entry.block, entry.size, entry.offset-old_toc))
    new_toc = 32 + len(entries) * 30 + len(blocks) * 2
    header = bytearray(original_toc[:32])
    struct.pack_into('>I', header, 12, new_toc)
    table = bytearray(header)
    for digest, index, size, offset in entries:
        table.extend(digest + index.to_bytes(4, 'big') + size.to_bytes(5, 'big')
                     + (new_toc+offset).to_bytes(5, 'big'))
    table.extend(b''.join(n.to_bytes(2, 'big') for n in blocks))
    return bytes(table), appended


def patch(source, output, replacements, expected_sha256=None):
    source, output = Path(source), Path(output)
    if source.resolve() == output.resolve() or output.exists():
        raise ValueError('Saída deve ser um arquivo novo diferente da fonte')
    before = source.stat()
    source_hash = hashlib.sha256()
    output_hash = hashlib.sha256()
    with Psarc(source) as archive:
        names = {e.name: e for e in archive.entries[1:]}
        if replacements.keys() - names.keys():
            raise ValueError('Nome de recurso ausente no PSARC')
        changed = {}
        for name, data in replacements.items():
            safe_name(name)
            if not isinstance(data, bytes) or len(data) > 64 * 1024 * 1024:
                raise ValueError('Recurso substituto inválido ou excessivamente grande')
            entry = names[name]
            if entry.size > 64 * 1024 * 1024:
                raise ValueError('Recurso original excessivamente grande')
            original = b''.join(archive.chunks(entry))
            if data != original:
                changed[name] = data
        old_toc = archive.header['toc_bytes']
        archive.file.seek(0)
        original_toc = archive.read_exact(old_toc)
        source_hash.update(original_toc)
        table, appended = rebuild_toc(archive, original_toc, changed)
        output.parent.mkdir(parents=True, exist_ok=True)
        with output.open('xb') as stream:
            stream.write(table)
            output_hash.update(table)
            while block := archive.file.read(1024 * 1024):
                source_hash.update(block)
                output_hash.update(block)
                stream.write(block)
            for block in appended:
                output_hash.update(block)
                stream.write(block)
    after = source.stat()
    if (before.st_size, before.st_mtime_ns) != (after.st_size, after.st_mtime_ns):
        raise ValueError('Arquivo fonte mudou durante a leitura')
    source_digest = source_hash.hexdigest()
    if expected_sha256 and source_digest != expected_sha256:
        raise ValueError('PSARC fonte diferente do inventário M0; saída não validada')
    with output.open('rb') as stream:
        written_digest = hashlib.file_digest(stream, 'sha256').hexdigest()
    if written_digest != output_hash.hexdigest():
        raise ValueError('Hash da gravação não confere')
    with Psarc(output) as archive:
        entries = {e.name: e for e in archive.entries[1:]}
        for name, data in replacements.items():
            if b''.join(archive.chunks(entries[name])) != data:
                raise ValueError(f'Reinserção não confere: {name}')
    if not changed and written_digest != source_digest:
        raise ValueError('PSARC sem mudanças deveria ser idêntico')
    return dict(source_sha256=source_digest, output_sha256=written_digest,
                supplied_resources=len(replacements), changed_resources=len(changed),
                identical_archive=written_digest == source_digest, verified=True)


def main():
    from text_tool import REPO, output_path
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--replace', action='append', default=[], metavar='PATH=FILE')
    args = parser.parse_args()
    output = output_path(args.output)
    inventory = json.loads((REPO / 'docs/m0/inventory.json').read_text(encoding='utf8'))
    source_root = Path(inventory['source']).resolve()
    relative = args.source.resolve().relative_to(source_root).as_posix()
    expected = next(r['sha256'] for r in inventory['files'] if r['path'] == relative)
    replacements = {}
    for pair in args.replace:
        name, file = pair.split('=', 1)
        if name in replacements:
            raise ValueError('Recurso repetido')
        replacements[safe_name(name)] = Path(file).read_bytes()
    result = patch(args.source, output, replacements, expected)
    (output.parent / (output.name+'.proof.json')).write_text(json.dumps(result, indent=2)+'\n', encoding='utf8')
    print(json.dumps(result), flush=True)


if __name__ == '__main__':
    main()
