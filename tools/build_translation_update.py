"""Build the owner's private en-16 -> en-17 difference update, without a PSARC.

Generated payloads stay in ignored build/. Only tools and technical metadata
belong in Git. The original disc body is copied locally by the applicator.
"""
import argparse
import json
from pathlib import Path
import tempfile
import zipfile

from apply_translation_update import FILES, RELEASE
from build_portable import ROOT, personal_text_hits
from translation_patch import build, digest

BASE_HASH = '489f1cb783d1b4b5ea62b8cd1432b59ce57efb94d1b33a8dcb7bb18c8a7a9d58'
TARGET_HASH = 'c06e2d4044559c664fa7d9d304314390495befa9162bd222679bd110c0b63e28'
RUNTIME_HASH = '284e1a81623ce5ef2cc3a49ba727eac5f065a676bda52021f9e5c6c9ff0e7a1f'
PREFIX = 'Atualizacao-Traducao/'

README = '''Tradução inglesa natural — en-16 para en-17 (24/10/2026)

Primeiro instale o update do núcleo GoW3 de 24/10/2026.
Este pacote é um patch: não contém common.psarc nem o executável do PS4.
Atualiza a tradução, menus, textos fixos e prompts; inclui o launcher que
ativa os novos arquivos. Não altera runtime, saves, configurações ou Wine.

Requer Python 3.11 ou mais recente e aproximadamente 6,5 GB livres durante
a montagem. Depois de conferir a nova tradução, o aplicador exclui a antiga.
Não deixa um backup de 6,35 GB ocupando espaço após concluir.

1. Feche o jogo e o launcher e mantenha-os fechados durante a atualização.
2. Extraia este ZIP na pasta que contém City Shrouded in Shadow.exe.
3. Windows: execute Atualizacao-Traducao/Aplicar traducao.cmd.
   Linux/Wine: no terminal, execute
   sh Atualizacao-Traducao/Aplicar-no-Linux.sh
   Não use sudo. O jogo continua usando Wine/Proton.
4. Aguarde a mensagem de conclusão e abra pelo launcher habitual.
   O inglês deve usar a revisão natural, novos menus e dicas.

Para só conferir compatibilidade (sem gravar):
python Atualizacao-Traducao/apply_translation_update.py --check-only
No Linux, use python3 em vez de python.

O aplicador exige a tradução en-16 distribuída em 15/10 (hash conferido)
ou en-17 já instalada, verifica todos os arquivos e só então instala.
Versões diferentes são recusadas sem substituir arquivos.
Os saves e ajustes do jogador ficam intactos. O manifest.json original
continua identificando a distribuição base; transaction.json registra
os hashes de antes e depois, em data/update-backups.

Recuperação de uma instalação interrompida antes de concluir:
Feche jogo/launcher. Copie a pasta de backup informada pelo aplicador e use:
python Atualizacao-Traducao/apply_translation_update.py --rollback "PASTA_DO_BACKUP"
No Linux, use python3. A reversão restaura os arquivos anteriores e mantém
o núcleo GoW3 instalado. Após concluir com sucesso, a tradução antiga é
apagada por pedido do dono do projeto e essa reversão não está disponível.

Validação técnica sem abrir o jogo. A revisão en-17 e a migração precisam
da conferência visual do jogador; não há promessa de campanha revisada
por humano ou de ganho de desempenho. Não inclua saves/logs ao repassar.
'''


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--base', type=Path, required=True)
    parser.add_argument('--portable', type=Path, default=ROOT / 'build/City-Shrouded-in-Shadow-Portable')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    output = args.out.resolve()
    if output.exists() or not output.is_relative_to(ROOT / 'build'):
        raise ValueError('Choose a new ZIP inside ignored build/')
    source = args.portable.resolve()
    target = source / 'languages/en/common.psarc'
    if digest(args.base) != BASE_HASH or digest(target) != TARGET_HASH:
        raise ValueError('Translation archives differ from audited en-16/en-17')
    if digest(source / 'runtime/kyoei.exe') != RUNTIME_HASH:
        raise ValueError('Portable does not contain the expected GoW3 runtime')
    with tempfile.TemporaryDirectory(dir=ROOT / 'build') as temporary:
        patch = Path(temporary) / 'common.patch'
        info = build(args.base, target, patch)
        rows = []
        for name in sorted(FILES):
            path = source / name
            hits, _ = personal_text_hits(path.read_bytes(), [b'davyd', b'codex', b'claude', b'chatgpt', b'kyoei-toshi-work'])
            if hits:
                raise ValueError('Personal metadata in payload: ' + name)
            rows.append(dict(path=name, bytes=path.stat().st_size, sha256=digest(path)))
        metadata = dict(schema=1, release=RELEASE, title='CUSA03562', version='01.03',
                        required_runtime_sha256=RUNTIME_HASH, archive=info,
                        patch_sha256=digest(patch), files=rows)
        with zipfile.ZipFile(output, 'x', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
            archive.write(patch, PREFIX + 'common.patch')
            for row in rows:
                archive.write(source / row['path'], PREFIX + 'payload/' + row['path'])
            for name in ('apply_translation_update.py', 'translation_patch.py', 'apply_runtime_update_linux.py'):
                archive.write(ROOT / 'tools' / name, PREFIX + name)
            archive.writestr(PREFIX + 'translation-update.json', json.dumps(metadata, indent=2) + '\n')
            archive.writestr(PREFIX + 'LEIA-ME.txt', README.encode('utf-8-sig'))
            archive.writestr(PREFIX + 'Aplicar-no-Linux.sh',
                             '#!/bin/sh\nset -eu\ncd "$(dirname "$0")"\nexec python3 ./apply_translation_update.py "$@"\n')
            archive.writestr(PREFIX + 'Aplicar traducao.cmd',
                             '@echo off\r\nwhere py >nul 2>nul\r\n'
                             'if errorlevel 1 (\r\n python "%~dp0apply_translation_update.py" %*\r\n'
                             ') else (\r\n py -3 "%~dp0apply_translation_update.py" %*\r\n)\r\npause\r\n')
        report = dict(zip=output.name, bytes=output.stat().st_size, sha256=digest(output),
                      patch_bytes=patch.stat().st_size, archive=info,
                      payloads=rows, game_archive_included=False)
        output.with_suffix('.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
        print(json.dumps(report, indent=2), flush=True)


if __name__ == '__main__':
    main()
