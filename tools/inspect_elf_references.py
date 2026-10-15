"""Locate candidate RIP-relative LEA and direct CALL references without executing an ELF.

Reports contain addresses, hashes and explicitly requested technical labels only.
Byte matches are candidates: validate instruction boundaries with a disassembler.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct


def load_segments(data):
    if len(data)<64 or data[:7]!=b'\x7fELF\x02\x01\x01':
        raise ValueError('Expected little-endian ELF64')
    header=struct.unpack_from('<16sHHIQQQIHHHHHH',data)
    if header[2]!=62: raise ValueError('Expected x86-64')
    offset,size,count=header[5],header[9],header[10]
    if size<56 or offset+size*count>len(data): raise ValueError('Invalid program headers')
    segments=[]
    for index in range(count):
        kind,flags,start,address,_,stored,memory,alignment=struct.unpack_from('<IIQQQQQQ',data,offset+index*size)
        if kind!=1: continue
        if stored>memory or start+stored>len(data): raise ValueError('Invalid load segment')
        segments.append(dict(offset=start,address=address,size=stored,flags=flags))
    if not segments: raise ValueError('No load segments')
    return segments


def references(data,literals=(),call_targets=(),address_targets=()):
    segments=load_segments(data); targets={address:[] for address in address_targets}; rows=[]
    for literal in literals:
        if not literal or not literal.isascii() or '\0' in literal:
            raise ValueError('Use nonempty ASCII technical labels')
        for match in re.finditer(re.escape(literal.encode()+b'\0'),data):
            for segment in segments:
                if segment['offset']<=match.start()<segment['offset']+segment['size']:
                    address=segment['address']+match.start()-segment['offset']
                    targets.setdefault(address,[]).append(literal)
                    rows.append(dict(label=literal,file_offset=hex(match.start()),address=hex(address)))
    candidates=[]; requested_calls=set(call_targets)
    for segment in segments:
        if not segment['flags']&1: continue
        code=data[segment['offset']:segment['offset']+segment['size']]
        for match in re.finditer(rb'[\x48-\x4f]\x8d[\x05\x0d\x15\x1d\x25\x2d\x35\x3d]',code):
            start=match.start()
            if start+7>len(code): continue
            address=segment['address']+start
            target=address+7+struct.unpack_from('<i',code,start+3)[0]
            if target in targets:
                candidates.append(dict(kind='candidate_rip_lea',address=hex(address),target=hex(target),labels=targets[target]))
        for match in re.finditer(b'\xe8',code):
            start=match.start()
            if start+5>len(code): continue
            address=segment['address']+start
            target=address+5+struct.unpack_from('<i',code,start+1)[0]
            if target in requested_calls:
                candidates.append(dict(kind='candidate_direct_call',address=hex(address),target=hex(target)))
    return dict(source_sha256=hashlib.sha256(data).hexdigest(),segments=segments,
        labels=rows,references=sorted(candidates,key=lambda row:int(row['address'],16)),
        instruction_boundaries_verified=False,runtime_verified=False)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('elf',type=Path)
    parser.add_argument('--literal',action='append',default=[])
    parser.add_argument('--call-target',action='append',type=lambda value:int(value,0),default=[])
    parser.add_argument('--address',action='append',type=lambda value:int(value,0),default=[])
    parser.add_argument('--report',type=Path)
    args=parser.parse_args()
    if not args.literal and not args.call_target and not args.address: parser.error('Select a technical label, address or call target')
    report=references(args.elf.read_bytes(),args.literal,args.call_target,args.address)
    report['source']=str(args.elf.resolve())
    serialized=json.dumps(report,indent=2)+'\n'
    if args.report: args.report.write_text(serialized,encoding='utf8')
    print(serialized)


if __name__=='__main__': main()
