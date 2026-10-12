"""Deferred module metadata derived from the verified original ELF."""
import struct

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
