"""Explicit word wrap for boxes the runtime word-wrap hook does not reach.

Mission/episode details and the item tooltip/info boxes break lines per glyph in
a separate native routine (Japanese style), so Western text split mid-word
("noi|te", "Indisponív|el", "o|bra"). Their text is pre-broken at spaces here,
measured with the game's own glyph advances (utf8all_table units).
Measured breaks: missions 986-998, item name tooltip 521-542, item description 500-521.
"""
from prepare_translation_catalog import TOKENS

# resource -> list of (string-id suffix or None for all, width, break marker)
PREWRAP = {
    'common/lua/mission_text.luc': [(None, 940, '\n')],
    # mail body/replies: native break measured 1133-1155
    'common/lua/Text_Mail.luc': [(None, 1090, '\n')],
    # names: '<br>' is shown literally there, a real newline breaks the line
    'common/csv/item_data.csh': [('/c25', 520, '\n'), ('/c26', 480, '<br>'), ('/c28', 480, '<br>')],
}


def rule(resource, string_id):
    for suffix, width, mark in PREWRAP.get(resource, ()):
        if suffix is None or string_id.endswith(suffix): return width, mark
    return None


def prewrap(text, glyphs, limit, mark='\n'):
    def width(s):
        return sum(glyphs[c]['advance'] for c in TOKENS.sub('', s) if c in glyphs)
    out = []
    for para in text.split(mark):
        line = ''
        for word in para.split(' '):
            cand = word if not line else line + ' ' + word
            if line and width(cand) > limit:
                out.append(line); line = word
            else:
                line = cand
        out.append(line)
    return mark.join(out)
