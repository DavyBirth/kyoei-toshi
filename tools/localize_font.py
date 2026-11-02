"""Add missing Latin glyphs to the native intermission font from the owned dump.

Keeps existing glyph rows and atlas pixels; adds rows and uses empty space on
the existing third R8 page. No font/game pixels are stored in the repository.
"""
import math
import struct
import zlib
from check_game_font_tables import glyph_rows
from text_tool import decode
from tex_preview import unpack_tex

PORTUGUESE_CHARACTERS='ÀÁÂÃÉÊÍÓÔÕÚÜÇàáâãéêíóôõúüç'


def extend_intermission(read, required):
    source_data=read('font/utf8all_table.csh')
    target_data=read('font/utf8all_IM_table.csh')
    source=glyph_rows(source_data)[0]; target=glyph_rows(target_data)[0]
    missing=sorted(set(required)-target.keys())
    if not missing:return {}
    if set(missing)-source.keys():raise ValueError('Required glyph is absent from donor font')
    page=2; name=f'font/utf8all_IM_{page}.tex'
    atlas=read(name); info,pixels=unpack_tex(atlas)
    if (info['storage_family'],info['channel_layout'],info['levels'])!=(1,0x30,1):
        raise ValueError('Expected the audited single-level R8 atlas')
    width,height=info['width'],info['height']; output=bytearray(pixels)
    if len(output)!=width*height:raise ValueError('R8 size differs')
    x=4; y=math.ceil(max(g['y']+g['height'] for g in target.values() if g['page']==page))+4
    row_height=0; additions=[]; donors={}
    donor_resource,donor_table=decode(source_data)
    for char in missing:
        glyph=source[char]; w,h=math.ceil(glyph['width']),math.ceil(glyph['height'])
        if x+w+4>width:x=4;y+=row_height+4;row_height=0
        if y+h+4>height:raise ValueError('No empty atlas space for required glyphs')
        if any(output[(y+j)*width+x:(y+j)*width+x+w] != bytes(w) for j in range(h)):
            raise ValueError('Destination pixels are not empty')
        donor_page=glyph['page']
        if donor_page not in donors:
            donor_info,donor_pixels=unpack_tex(read(f'font/utf8all_{donor_page}.tex'))
            if (donor_info['storage_family'],donor_info['channel_layout'],donor_info['levels'])!=(1,0x30,1):
                raise ValueError('Expected single-level R8 donor atlas')
            donors[donor_page]=(donor_info,donor_pixels)
        donor_info,donor_pixels=donors[donor_page]; dw=donor_info['width']
        sx,sy=glyph['x'],glyph['y']
        if int(sx)!=sx or int(sy)!=sy or sx<0 or sy<0 or sx+w>dw or sy+h>donor_info['height']:
            raise ValueError('Donor glyph coordinates differ')
        for j in range(h):
            begin=(int(sy)+j)*dw+int(sx)
            output[(y+j)*width+x:(y+j)*width+x+w]=donor_pixels[begin:begin+w]
        values=[]
        for column in range(9):
            ptr,tag=struct.unpack_from('>II',donor_resource.payload,12+(glyph['row']*9+column)*8)
            raw=donor_resource.payload[ptr:ptr+4]
            if column in (1,2,3):raw=struct.pack('>I',{1:page,2:x,3:y}[column])
            values.append((tag,raw))
        additions.append((char,values));x+=w+4;row_height=max(row_height,h)
    resource,table=decode(target_data)
    shift=len(additions)*9*8; old_pool=12+table.rows*9*8
    cells=bytearray(resource.payload[12:old_pool])
    for offset in range(0,len(cells),8):
        ptr=struct.unpack_from('>I',cells,offset)[0]
        if ptr<old_pool or ptr+4>len(resource.payload):raise ValueError('Unexpected native font pointer')
        struct.pack_into('>I',cells,offset,ptr+shift)
    extra_cells=bytearray(); extra_pool=bytearray()
    pool_start=len(resource.payload)+shift
    for _,values in additions:
        for tag,raw in values:
            extra_cells+=struct.pack('>II',pool_start+len(extra_pool),tag);extra_pool+=raw
    payload=(struct.pack('>III',table.flags,9,table.rows+len(additions))+cells+extra_cells+
             resource.payload[old_pool:]+extra_pool)
    new_table=resource.rebuild(bytes(payload))
    checked=glyph_rows(new_table)[0]
    if any(checked[c]!=g for c,g in target.items()):raise ValueError('Existing glyph changed')
    if set(required)-checked.keys():raise ValueError('Missing glyph after rebuild')
    packed=zlib.compress(bytes.fromhex(info['descriptor'])+bytes(output),9)
    _,_,_,flags=struct.unpack_from('>4sIII',atlas,128)
    new_atlas=atlas[:128]+struct.pack('>4sIII',b'BILZ',12+len(output),len(packed),flags)+packed
    if unpack_tex(new_atlas)[1]!=output:raise ValueError('Atlas roundtrip differs')
    return {'font/utf8all_IM_table.csh':new_table,name:new_atlas}
