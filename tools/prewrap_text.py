"""Explicit word wrap for boxes the runtime word-wrap hook does not reach.

The mission/episode detail boxes break lines per glyph in a separate native
routine (Japanese style), so Western text split mid-word ("noi|te"). Their
text is pre-broken at spaces here, measured with the game's own glyph advances.
Measured box width: lines broke at 986-998 font units; 940 leaves margin.
"""
from prepare_translation_catalog import TOKENS

# resource -> width in font units (utf8all_table advances)
PREWRAP = {'common/lua/mission_text.luc': 940}


def prewrap(text, glyphs, limit):
    def width(s):
        return sum(glyphs[c]['advance'] for c in TOKENS.sub('', s) if c in glyphs)
    out = []
    for para in text.split('\n'):
        line = ''
        for word in para.split(' '):
            cand = word if not line else line + ' ' + word
            if line and width(cand) > limit:
                out.append(line); line = word
            else:
                line = cand
        out.append(line)
    return '\n'.join(out)
