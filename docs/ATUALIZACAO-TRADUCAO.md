# Atualização pequena da tradução (en-17)

## Como funciona
A tradução inteira (recursos do `common.psarc` e correções de HUD) é um único
arquivo `languages/en/translation.kydelta` (~8,3 MB, formato KYDELTA1 de
`tools/psarc_delta.py`) aplicado pelo runtime, na leitura, sobre o
`game/DAT/common.psarc` original e intocado. Não existe mais cópia traduzida de
6,35 GB. O delta confere tamanho e TOC do arquivo base; se não bater, o jogo
lê o original.

- `tools/build_translation_delta.py ORIGINAL TRADUZIDO SAIDA --overlay HUD.kydelta`
  gera o delta a partir de um arquivo traduzido feito por `psarc_patch`.
- `tools/build_portable.py` já monta a portátil assim; o launcher usa
  `translation.kydelta` quando existe e, em instalações antigas, continua com
  `languages/en/common.psarc`.
- Runtime: com `KYOEI_ARCHIVE_DELTA` e sem `KYOEI_TRANSLATION_ARCHIVE`,
  `kyoei_translation_mount.h` não monta cópia e o delta faz o trabalho.

## Pacote para amigos
```
cd tools
python build_translation_update.py --runtime-update ../build/Kyoei-Windows-Update-20261024-GoW3.zip --out ../build/Kyoei-English-Translation-Update-AAAAMMDD.zip
python test_translation_update.py ../build/Kyoei-English-Translation-Update-AAAAMMDD.zip
```
Conteúdo: runtime novo, launcher, delta, prompts en/ja, textos do executável e
artes de UI. Aceita qualquer runtime listado nos updates anteriores. Windows:
`Aplicar traducao.cmd` (PowerShell, sem instalar nada). Linux/Wine:
`sh Atualizacao-Traducao/Aplicar-no-Linux.sh` (Python 3.11+).

O aplicador confere hashes, prepara cópias temporárias, troca os arquivos
guardando os antigos pequenos em `data/update-backups/`, desfaz tudo se algo
falhar e, só no fim, exclui `languages/en/common.psarc` (6 GB) e
`ui-corrections.kydelta`. Saves, `game/` e configurações não são tocados.

Atualizações seguintes da tradução: regenerar a portátil e o pacote; só o
delta (e o que mais mudar) vai junto.
