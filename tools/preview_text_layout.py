"""Offline text-layout preview: estimate where translations need more lines than the game's boxes hold.

Box width per group (one Lua file, or one CSV column) is taken as the widest
Japanese line in that group, since the original always fits. Translations are
wrapped at that width with the port's word-wrap rule (break at the last space,
mid-word only when a line has no space) using the game's own glyph advances.
An entry is flagged when it needs more lines than any Japanese entry of its
group. Flagged entries are drawn with the game's atlas glyphs. Estimate only:
real box sizes and scaling are not read from the running game.
"""
import argparse
import base64
import glob
import html
import io
import json
import re
from collections import defaultdict
from pathlib import Path
from PIL import Image
from check_game_font_tables import glyph_rows
from tex_preview import unpack_tex
from text_tool import REPO, export

TAG = re.compile(r'\[#[0-9a-fA-F]{8}\]|\[/#\]')
ICON = re.compile(r'\[i\d+\]')
NAME = re.compile(r'<P_[FL]NAME>')
NEWLINE = re.compile(r'\\n|\r\n|\n')
NAME_WIDTH = 9 * 24  # longest allowed name: nine wide Latin letters


def load_translations(language, merged):
    files = [merged] if merged else sorted(glob.glob(str(REPO / 'translations/localization-en*.json')))
    key = 'pt-BR' if language == 'pt-BR' else 'en'
    out = {}
    for name in files:
        for resource in json.loads(Path(name).read_text(encoding='utf8'))['resources']:
            for entry in resource['entries']:
                if entry.get(key):
                    out[(resource['path'], entry['id'])] = entry[key]
    return out


class Font:
    def __init__(self, root):
        self.glyphs = glyph_rows((root / 'common/font/utf8all_table.csh').read_bytes())[0]
        info, pixels = unpack_tex((root / 'common/font/utf8all_0.tex').read_bytes())
        self.atlas = Image.frombytes('L', (info['width'], info['height']), bytes(pixels))
        self.fallback = self.glyphs['あ']['advance']

    def tokens(self, line):
        """Yield (kind, value, advance) pieces of one explicit line."""
        pos = 0
        for match in re.finditer(r'\[#[0-9a-fA-F]{8}\]|\[/#\]|\[i\d+\]|<P_[FL]NAME>', line):
            for ch in line[pos:match.start()]:
                yield 'char', ch, self.glyphs.get(ch, {}).get('advance', self.fallback)
            token = match.group()
            if ICON.fullmatch(token): yield 'icon', token, self.fallback
            elif NAME.fullmatch(token): yield 'name', token, NAME_WIDTH
            pos = match.end()
        for ch in line[pos:]:
            yield 'char', ch, self.glyphs.get(ch, {}).get('advance', self.fallback)

    def width(self, line):
        return sum(a for _, _, a in self.tokens(line))

    def wrap(self, text, limit):
        lines = []
        for explicit in NEWLINE.split(text):
            pieces = list(self.tokens(explicit)); start = 0
            while True:
                used = 0; end = start; last_space = None
                while end < len(pieces) and used + pieces[end][2] <= limit:
                    if pieces[end][1] == ' ' and end > start: last_space = end
                    used += pieces[end][2]; end += 1
                if end >= len(pieces):
                    lines.append(pieces[start:]); break
                if last_space is not None:
                    lines.append(pieces[start:last_space]); start = last_space + 1
                else:
                    end = max(end, start + 1); lines.append(pieces[start:end]); start = end
        return lines

    def draw(self, lines, limit, max_lines):
        line_height = 44; scale = .5
        width = int(limit) + 16; height = line_height * max(len(lines), max_lines) + 16
        image = Image.new('RGB', (width, height), (24, 28, 36))
        box = Image.new('RGB', (int(limit), line_height * max_lines), (40, 52, 72))
        image.paste(box, (8, 8))
        for row, pieces in enumerate(lines):
            x = 8; y = 8 + row * line_height
            colour = (255, 120, 120) if row >= max_lines else (240, 240, 240)
            for kind, value, advance in pieces:
                glyph = self.glyphs.get(value) if kind == 'char' else None
                if glyph and glyph['page'] == 0 and glyph['width'] > 0:
                    left, top = int(glyph['x']), int(glyph['y'])
                    crop = self.atlas.crop((left, top, left + int(glyph['width']), top + int(glyph['height'])))
                    ink = Image.new('RGB', crop.size, colour)
                    # offset_y is the glyph's height above the baseline.
                    image.paste(ink, (int(x + glyph['offset_x']), int(y + 34 - glyph['offset_y'])), crop)
                elif kind != 'char':
                    tint = (90, 140, 255) if kind == 'icon' else (90, 200, 120)
                    image.paste(Image.new('RGB', (int(advance) - 4, 30), tint), (int(x) + 2, y + 6))
                x += advance
        image = image.resize((int(width * scale), int(height * scale)), Image.LANCZOS)
        buffer = io.BytesIO(); image.save(buffer, 'PNG')
        return base64.b64encode(buffer.getvalue()).decode()


def group_of(path, entry_id):
    match = re.match(r'r\d+/(c\d+)$', entry_id)
    return f'{path}#{match.group(1)}' if match else path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', type=Path)
    parser.add_argument('--pt-merged', type=Path, help='merge_language_fallback output for pt-BR')
    parser.add_argument('--limit', type=int, default=400, help='max flagged entries drawn per language')
    args = parser.parse_args()
    audit = json.loads((REPO / 'docs/m2/audit.json').read_text(encoding='utf8'))
    root = Path(audit['source']); font = Font(root)
    languages = {'en': load_translations('en', None)}
    if args.pt_merged: languages['pt-BR'] = load_translations('pt-BR', args.pt_merged)
    originals = {}
    for row in audit['records']:
        if row['path'].endswith(('.luc', '.csh')) and '/font/' not in row['path']:
            try: document = export((root / row['path']).read_bytes())
            except Exception: continue
            for string in document['strings']:
                if string['japanese_candidate']:
                    originals[(row['path'], string['id'])] = string['original']
    boxes = defaultdict(lambda: [0.0, 0])
    for (path, entry_id), text in originals.items():
        box = boxes[group_of(path, entry_id)]
        explicit = NEWLINE.split(text)
        box[0] = max(box[0], max(font.width(line) for line in explicit)); box[1] = max(box[1], len(explicit))
    report = dict(scope=__doc__.strip(), runtime_verified=False, languages={})
    sections = []
    for language, texts in languages.items():
        flagged = []
        for key, text in texts.items():
            if key not in originals: continue
            limit, max_lines = boxes[group_of(*key)]
            # Short one-line labels (names, captions) and developer test files are not boxes we can infer.
            if limit <= 0 or (max_lines == 1 and limit < 400) or '_test' in key[0]: continue
            lines = font.wrap(text, limit)
            if len(lines) > max_lines:
                flagged.append((len(lines) - max_lines, key, text, lines, limit, max_lines))
        flagged.sort(key=lambda item: (-item[0], item[1]))
        report['languages'][language] = dict(checked=sum(k in originals for k in texts), flagged=len(flagged),
            entries=[dict(resource=k[0], id=k[1], extra_lines=e, box_lines=m) for e, k, _, _, _, m in flagged])
        cards = []
        for extra, key, text, lines, limit, max_lines in flagged[:args.limit]:
            picture = font.draw(lines, limit, max_lines)
            cards.append(f'<div class=card><div class=meta>{html.escape(key[0])} · {html.escape(key[1])} · '
                         f'+{extra} linha(s) além de {max_lines}</div><img src="data:image/png;base64,{picture}">'
                         f'<div class=text>{html.escape(text)}</div></div>')
        sections.append(f'<h2>{language}: {len(flagged)} textos passam da caixa estimada</h2>' + ''.join(cards))
    page = ('<!doctype html><meta charset=utf-8><title>Prévia de diagramação</title><style>'
            'body{background:#111;color:#ddd;font:14px sans-serif;margin:16px}.card{margin:12px 0;padding:8px;'
            'background:#1b1f27;border-radius:6px}.meta{color:#8ab;font-size:12px}.text{white-space:pre-wrap;'
            'color:#999;font-size:12px}img{display:block;margin:6px 0;max-width:100%}</style>'
            '<h1>Prévia de diagramação (estimativa fora do jogo)</h1><p>Caixa azul = largura e número de linhas '
            'que o japonês usa naquele tipo de texto. Linhas em vermelho passam da caixa.</p>' + ''.join(sections))
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(page, encoding='utf8')
    args.output.with_suffix('.json').write_text(json.dumps(report, ensure_ascii=False, indent=1), encoding='utf8')
    for language, row in report['languages'].items():
        print(language, 'checked', row['checked'], 'flagged', row['flagged'])


if __name__ == '__main__':
    main()
