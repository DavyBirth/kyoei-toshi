"""Plano local de limpeza: somente derivados reproduziveis, sem apagar nada."""
import json
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
WORK = Path('G:/kyoei-toshi-work').resolve(strict=True)
MEDIA = {'.tex', '.mdl', '.mpk', '.awb', '.acb', '.usm', '.pak', '.acb_tmp', '.mp4'}


def main():
    catalog = json.loads((REPO/'work/m1/catalog.json').read_text(encoding='utf8'))
    extraction = Path(catalog['extraction']).resolve(strict=True)
    if extraction != WORK/'m1':
        raise ValueError('Extracao inesperada')
    files = []
    retained = 0
    for archive in catalog['archives']:
        for row in archive['files']:
            path = extraction/Path(archive['archive']).stem/row['path']
            actual = path.resolve(strict=True)
            if not actual.is_relative_to(extraction) or actual != path.absolute():
                raise ValueError(f'Arquivo redirecionado: {path}')
            if path.stat().st_size != row['bytes']:
                raise ValueError(f'Arquivo alterado: {path}')
            if path.suffix.lower() in MEDIA:
                files.append(dict(path=str(actual), bytes=row['bytes'],
                                  archive=archive['archive'], entry=row['path'], sha256=row['sha256']))
            else:
                retained += 1
    directories = []
    for relative in ('m2/prova-01/CUSA03562', 'native-01/out', 'native-02/out', 'native-03/out'):
        path = WORK/relative
        actual = path.resolve(strict=True)
        if not actual.is_relative_to(WORK) or actual != path.absolute():
            raise ValueError(f'Pasta redirecionada: {path}')
        items = list(path.rglob('*'))
        if any(p.is_symlink() or p.is_junction() for p in items):
            raise ValueError('Link dentro da pasta de limpeza')
        directories.append(dict(path=str(actual), bytes=sum(p.stat().st_size for p in items if p.is_file())))
    report = dict(work=str(WORK), extraction=str(extraction), delete_files=files,
                  delete_directories=directories, retained_extracted_files=retained,
                  planned_bytes=sum(r['bytes'] for r in files+directories),
                  preserved=['original dump', 'native-04', 'build', 'fixed core',
                             'catalogs, logs, evidence and saves', 'all non-media M1 resources'])
    output = WORK/'cleanup-20261009'
    output.mkdir(exist_ok=True)
    with (output/'plan.json').open('x', encoding='utf8') as stream:
        json.dump(report, stream, indent=2)
    print(json.dumps({k:v for k,v in report.items() if k not in ('delete_files',)}, indent=2))
    print(f'Media files to delete: {len(files)}', flush=True)


if __name__ == '__main__':
    main()
