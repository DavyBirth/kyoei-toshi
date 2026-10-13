"""Read-only preview of measured TEX/BILZ BC3 images. No reinsertion yet."""
import argparse
import io
from pathlib import Path
import struct
import zlib

def unpack_tex(data):
    if len(data)<144 or data[:4]!=b'\0xet' or struct.unpack_from('>I',data,4)[0]!=2:
        raise ValueError('Unsupported TEX container')
    magic,raw_size,stored_size,flags=struct.unpack_from('>4sIII',data,128)
    if magic!=b'BILZ' or stored_size!=len(data)-144 or raw_size>128*1024*1024:
        raise ValueError('Invalid TEX BILZ header')
    stream=zlib.decompressobj(); raw=stream.decompress(data[144:],raw_size+1)
    if not stream.eof or stream.unused_data or stream.unconsumed_tail or len(raw)!=raw_size or len(raw)<12:
        raise ValueError('Invalid TEX payload')
    width,height=struct.unpack_from('<HH',raw)
    if not 0<width<=8192 or not 0<height<=8192: raise ValueError('Invalid TEX dimensions')
    return dict(width=width,height=height,pixel_code=raw[4],levels=raw[5]&15,
                descriptor=raw[:12].hex(),payload_bytes=len(raw)-12),raw[12:]

def dds_bc(width,height,body,fourcc):
    header=bytearray(128); header[:4]=b'DDS '
    struct.pack_into('<7I',header,4,124,0x81007,height,width,len(body),0,1)
    struct.pack_into('<2I4s',header,76,32,4,fourcc)
    struct.pack_into('<I',header,108,0x1000)
    return bytes(header)+body

def preview(data):
    from PIL import Image
    info,body=unpack_tex(data)
    if info['pixel_code']!=0x12: raise ValueError(f'Pixel code not certified: {info["pixel_code"]:#x}')
    count=((info['width']+3)//4)*((info['height']+3)//4)*16
    if len(body)<count: raise ValueError('BC3 base level truncated')
    image=Image.open(io.BytesIO(dds_bc(info['width'],info['height'],body[:count],b'DXT5')))
    image.load(); return image,info

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source',type=Path); parser.add_argument('output',type=Path)
    args=parser.parse_args(); image,info=preview(args.source.read_bytes())
    args.output.parent.mkdir(parents=True,exist_ok=True)
    image.save(args.output); print(info)
if __name__=='__main__': main()
