"""Regression for opaque save/talk markers, using only an owner's private archive."""
import sys
from pathlib import Path
from psarc import Psarc
from tex_preview import preview, unpack_tex
from make_american_icons import build, PAIRS


def main(base):
    names={name for pair in PAIRS for name in pair}
    names.add('texture/hud/maru_button.tex')
    names.update(f'texture/text marker/talk_icon_0{i}.tex' for i in range(1,7))
    with Psarc(base) as archive:
        entries={e.name:e for e in archive.entries[1:]}
        originals={n:b''.join(archive.chunks(entries[n])) for n in names}
    changed=build(originals)
    for a,b in PAIRS:
        assert unpack_tex(changed[a])[1]==unpack_tex(originals[b])[1]
        assert unpack_tex(changed[b])[1]==unpack_tex(originals[a])[1]
    for name in sorted(names):
        if name!='texture/hud/maru_button.tex' and not name.startswith('texture/text marker/'):
            continue
        original,_=preview(originals[name]);result,_=preview(changed[name])
        assert original.size==result.size
        result=result.convert('RGBA');original=original.convert('RGBA')
        cx,cy,radius=(15,15,6) if name.endswith('maru_button.tex') else (24,28,8)
        # Erasing the old glyph to transparency made the scene show through.
        for y in range(cy-radius,cy+radius+1):
            for x in range(cx-radius,cx+radius+1):
                if (x-cx)**2+(y-cy)**2<=radius**2:
                    assert result.getpixel((x,y))[3]>=240,(name,x,y,'transparent interior')
        if name.startswith('texture/text marker/'):
            # Keep the original stem all the way through tall prompt variants.
            box=(0,52,result.width,result.height)
            assert result.crop(box).tobytes()==original.crop(box).tobytes(),(name,'stem pixels changed')
    print('AMERICAN ICONS PASS: native paired pixels, seven opaque interiors, six complete stems')


if __name__=='__main__':main(Path(sys.argv[1]))
