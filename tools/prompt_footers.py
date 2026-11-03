"""Button cells baked into journal/Collection footer artwork, in native pixels.

Only these cells change; translated labels and the surrounding artwork stay intact.
"""
PREFIX = 'texture/intermission/'
FOOTERS = {
    PREFIX+'IM_RE_operation_02.tex': [('batu', (0,0,33,32)), ('maru',(133,0,167,32)), ('L1',(346,0,397,32)), ('R1',(593,0,645,32))],
    PREFIX+'IM_RE_operation.tex': [('batu',(0,0,33,40)), ('maru',(132,0,168,40))],
    PREFIX+'IM_RE_operation_03.tex': [('batu',(0,0,33,32)), ('L1',(131,0,183,32)), ('R1',(365,0,417,32))],
    PREFIX+'backnumber_operation.tex': [('s_L',(325,0,361,36)), ('maru',(473,0,509,36)), ('batu',(578,0,614,36))],
    PREFIX+'IM_detail_operation_3.tex': [('s_L',(370,0,407,36)), ('batu',(578,0,614,36))],
    PREFIX+'IM_detail_operation_2.tex': [('s_L',(236,0,273,36)), ('juzhi',(443,0,477,36)), ('batu',(578,0,614,36))],
    PREFIX+'IM_detail_operation_4.tex': [('batu',(578,0,614,36))],
    PREFIX+'IM_detail_operation.tex': [('juzhi',(443,0,477,36)), ('batu',(578,0,614,36))],
}
KEYS = {'batu':'Space', 'maru':'E', 'L1':'Q', 'R1':'Shift', 's_L':'WASD', 'juzhi':'@arrows'}
IDS = {'batu':2, 'maru':1, 'L1':10, 'R1':12, 's_L':14, 'juzhi':5}


def draw(name, image, family='keyboard', references=None):
    from make_keyboard_icons import keycap
    from make_pad_icons import font_icon
    out = image.copy()
    for role, box in FOOTERS[name]:
        crop = image.crop(box)
        w, h = crop.size
        if family == 'keyboard':
            icon = keycap(w, h, KEYS[role])
        elif family == 'american':
            if role not in ('maru', 'batu'): continue
            # Native PS glyphs, swapped for the PC's physical confirm/back mapping.
            other = 'batu' if role == 'maru' else 'maru'
            icon = references[other].resize((w,h))
        else:
            icon = font_icon(f'IC_{role}_{IDS[role]}.tex', crop, family)
        if icon is None: raise ValueError('Footer icon missing: '+role)
        out.paste((0,0,0,0),box)
        out.alpha_composite(icon,box[:2])
    return out
