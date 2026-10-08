# M1 — extração e catálogo

Concluído tecnicamente em 08/10/2026: 25 PSARC, 72.961 arquivos extraídos
e conferidos, totalizando 19.327.950.739 bytes. Nove testes sintéticos passaram.

Fonte: o dump CUSA03562, APP_VER 01.03, inventariado no M0.
Destino dos dados extraídos: `G:\kyoei-toshi-work\m1`.
O destino fica fora do repositório e fora do dump.

## Como repetir

Na raiz do projeto:

As ferramentas do M1 usam Python 3.11 ou mais recente, sem dependências externas.

```powershell
python tools/test_psarc.py
python tools/catalog_m1.py
python tools/catalog_m1.py --extract G:/kyoei-toshi-work/m1-nova-extracao
python tools/report_m1.py
```

O comando sem `--extract` apenas lista as entradas e grava `listing.json`
e `listing-summary.json` em `work/m1`, preservando a prova da extração.
A extração exige uma pasta nova; recusa sobrescrever um destino existente,
gravar dentro do dump ou dentro do repositório. Confere espaço disponível,
SHA-256 dos PSARC contra o M0, hashes dos caminhos e limites de blocos.
Depois de extrair, relê todas as cópias e compara seus tamanhos e hashes.

A prova local detalhada fica em `work/m1/catalog.json`, ignorado pelo Git.
Ela contém caminhos, tamanhos, offsets, hashes e prefixos curtos; nenhum
texto completo ou arquivo do jogo é distribuído no repositório.
`summary.json`, `formats.json` e [CATALOGO.md](CATALOGO.md) registram as
contagens, formatos e o hash do catálogo local.

Para conferir novamente uma extração já pronta, sem ocupar espaço com
outra cópia, executar `python tools/verify_m1.py`. Esse comando verifica
o hash do catálogo, a lista exata de arquivos, seus tamanhos e SHA-256,
e termina com `VALIDADO M1: 72961 arquivos; 19327950739 bytes`.

## O que foi encontrado

| Família | Local e exemplos | Evidência / limite |
|---|---|---|
| Scripts candidatos | `common.psarc/lua/*.luc`; `TextData.luc`, `MessageWindowData.luc`, `map_goal_text.luc`, `stage_*_news_text.luc` | 299 arquivos, prefixo próprio `00 63 75 6C`; o conteúdo interno ainda precisa de leitor |
| Tabelas candidatas a textos | `common.psarc/csv/*.csh`; `item_data.csh`, `reading_material.csh`, `LoadingTips.csh`, `loading_image_and_text.csh`, `operation_text.csh` | Os nomes indicam bons pontos de partida; `.csh` também armazena dados não textuais |
| Fontes | `common.psarc/font/utf8all*.tex` e `utf8all*_table.csh`, incluindo subpastas `hgg`, `hgg16`, `hgg32`, `hgg128`, `msg16`, `org`; texturas `GxCore/texture/font*` no `system.psarc` | Atlas e tabelas candidatos; cobertura de caracteres será provada no M3 |
| Imagens / texturas | `texture`, `model`, `font`, arquivos `.tex` | 51.266 arquivos `.tex`, incluindo mapas, interface e imagens de itens; há um cabeçalho próprio, não são PNG prontos |
| Sons | `sound.psarc/adx2` | `.acb` com assinatura `@UTF`; `.awb` com assinatura `AFS2`; há `.h`, `.acf` e `.acb_tmp` auxiliares |
| Vídeos | `movie.psarc` | Três `.usm` com assinatura `CRID` e um `.mp4` com marca `ftyp`; nomes `Title01.usm`, `Gzcm.usm`, `201news.usm`, `test.mp4` |
| Animação / modelos | `model`, `motion`, `motionSequence`, `motionSequencePackage`, `morpheme.psarc/LE64` | `.mdl`, `.mdc`, `.mpk`, `.msq`, `.pak`, `.nam`, `.lst`; formatos internos ainda não documentados completamente |
| Gráficos auxiliares | `system.psarc/GxEffect`, `GxShader` | Shaders e recursos com caminhos `shader_PS4` e `shader_5_0`; a presença destes nomes não comprova um port de PC existente |
| Dados de fase | `stage*.psarc`, famílias `rtd`, `collision`, `hud`, `map` | Scripts, geometria, sequências e parâmetros candidatos; interpretar cada formato exige investigação |

As categorias acima são um mapa para investigação, baseado em diretórios,
extensões e cabeçalhos observados. Ainda não foi provado onde estão **todos**
os textos, nem se há textos gravados diretamente em imagens ou vídeos.

## Continuação

M2 deve começar pelos contêineres `.luc` e `.csh` e pelos arquivos candidatos
listados acima. É necessário documentar a estrutura interna, localizar textos
e criar extração e reinserção com prova de ida e volta no shadPS4.
Nenhuma tradução, alteração de fonte ou reinserção foi feita no M1.

O leitor PSARC e suas referências estão descritos em [PSARC.md](PSARC.md).
Não há mudança na tela do jogo nesta etapa: o resultado visível é o catálogo
e a pasta local com os dados extraídos.
