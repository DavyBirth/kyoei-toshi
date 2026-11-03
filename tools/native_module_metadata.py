"""Deferred module metadata derived from the verified original ELF."""
import struct
import json

def write_tls_sites(out):
    linked = json.loads((out/'link.json').read_text())
    sites = sorted(int(value,16) for values in linked['fs_loads_not_patched'].values() for value in values)
    if len(sites) != len(set(sites)) or len(sites) > 4096:
        raise ValueError('Invalid TLS instruction metadata')
    (out/'tls-sites.tsv').write_text(''.join(f'{site:x}\n' for site in sites))
    return len(sites)

def records(base, module):
    rows = []
    fini = module['tags'].get(13, 0)
    if fini:
        rows.append(f'F\t{base+fini:x}')
    for p in module['ph']:
        if p['type'] in (1, 0x61000010) and p['memsz']:
            rows.append(f'S\t{base+p["vaddr"]:x}\t{p["memsz"]:x}\t{p["flags"]:x}')
        elif p['type'] == 0x6474e550:
            data = module['elf'][p['offset']:p['offset']+p['filesz']]
            if len(data) < 8 or data[0] != 1 or data[1] != 0x1b:
                raise ValueError('Uncertified EH frame header encoding')
            frame = p['vaddr'] + 4 + struct.unpack_from('<i',data,4)[0]
            if not 0 <= frame < p['vaddr']:
                raise ValueError('Uncertified EH frame range')
            rows.append(f'H\t{base+p["vaddr"]:x}\t{p["memsz"]:x}\t{base+frame:x}\t{p["vaddr"]-frame:x}')
    return rows


def normalize_imports(out):
    path = out/'imports.tsv'
    rows = []
    unnamed = []
    for index, line in enumerate(path.read_text(encoding='utf8').splitlines()):
        fields = line.split('\t')
        if len(fields) != 4 or fields[0] != str(index) or not fields[1] or not fields[2]:
            raise ValueError('Tabela de importações inválida')
        if not fields[3]:
            fields[3] = '?'
            unnamed.append(index)
        if fields[1].split('#')[0] == 'fJgP+wqifno' and fields[2] == 'libSceDiscMap':
            fields[3] = 'sceDiscMap_7C980FFB0AA27E7A'
        rows.append('\t'.join(fields))
    path.write_text('\n'.join(rows)+'\n', encoding='utf8')
    return unnamed

