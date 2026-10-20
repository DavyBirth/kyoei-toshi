# Texturas traduzidas e controles — alinhamento em 20/10/2026

Davy deixou as texturas traduzidas e os diagramas com Claude. O trabalho do
Codex segue em modo foto, desempenho e opcoes graficas. Nao alterar os mesmos
geradores/arquivos em paralelo sem conferir o diff atual. Este documento e o
repasse local solicitado por Davy; nao foi enviado por servico externo.

## Comandos ja implementados

- Controle fisico: **X confirma/interage; bola volta/cancela**. A troca tambem
  vale para as acoes desses botoes durante a partida. SOUTH envia0x2000 e EAST
  envia0x4000 ao jogo. Nao trocar novamente o pad nem as mascaras nativas.
- Teclado: E/Enter confirma/interage; Espaco/Backspace envia a acao da bola
  fisica. Esc/Tab e Options/pausa na partida. Demais comandos permanecem em
  `src/kyoei_pc_controls.h`. O modo foto usa comandos proprios; X fotografa.
- Commit `fc44d09`: runtime e prompts americanos, corrigindo os residuos no
  marcador de salvar/conversar. `docs/re/CONTROLES-AMERICANOS.md` tem detalhes.

## Diagramas pendentes

Recursos em `common.psarc/texture/controller_operation/`:

| Recurso | Dimensoes | Mips | Formato |
| --- | --- | --- | --- |
| operation_normal.tex | 1920x1080 | 11 | BC3 |
| operation_normal_s.tex | 616x732 | 10 | BC3 |
| operation_normal_207.tex | 1920x1080 | 11 | BC3 |
| operation_normal_207_s.tex | 616x732 | 10 | BC3 |

Trocar as instrucoes e as linhas de chamada para o padrao americano. No
desenho fisico do controle, X continua embaixo e bola continua a direita:
trocar apenas o simbolo impresso no desenho faria uma indicacao errada.
Nas listas, X indica investigar/conversar/confirmar; bola indica cancelar e
as demais acoes antes associadas ao X. Conferir os textos de cada variante;
a variante207 possui instrucoes adicionais de container/empilhadeira.

## Preservar o que ja foi corrigido

Os marcadores `texture/text marker/talk_icon_01..06.tex` incluem cabeca,
haste e areas adicionais. A troca inicial apagava o fundo para alpha0 e
deixava residuos. Agora `tools/make_american_icons.py` reutiliza o glifo
nativo, conserva o fundo opaco e preserva os blocos de mip nao alterados.
Nao redesenhar X com duas linhas nem recomprimir toda a textura se so muda
o simbolo. Preservar dimensoes, alfa, padding, quantidade de mips e formatos.

Prova privada: `build/research/photo-mode/transfer-20261020/native-glyph-loaded.png`.
Teste: `build/image-tools/Scripts/python.exe tools/test_american_icons.py BASE`.
BASE e o common.psarc do dump proprio ou a derivacao privada compativel.
Confere sete interiores opacos, pares de pixels nativos e igualdade exata
das seis hastes. Nao versionar imagens/arquivos derivados do jogo.

## Integracao com a traducao

`tools/build_prompt_pack.py BASE SAIDA` gera KYPROMP2: original para reconhecer
a imagem carregada, teclado e controle americano. **Reconstruir o pacote
usando exatamente o arquivo base da nova traducao** quando as texturas mudarem.
O runtime reconhece os pixels, nao o nome do recurso. Pacote antigo pode
deixar de reconhecer texturas de um arquivo novo. Os prompts americanos
atuais sao gerados em cima dos botoes originais japoneses: evitar trocar o
mesmo simbolo duas vezes entre arquivo traduzido e pacote americano.

Nao colocar diagramas grandes indiscriminadamente no reconhecimento por
upload de `kyoei_live_prompts.h`: comparar imagens1920x1080 repetidamente
pode adicionar custo justamente na etapa de desempenho. Preferir o patch
de recursos na leitura do arquivo, mantendo a referencia original intacta.
`tools/psarc_delta.py` e `tools/build_ui_corrections.py` mostram o caminho;
ha somente um `KYOEI_ARCHIVE_DELTA` ativo, portanto reunir as substituicoes
no mesmo delta, em vez de sobrescrever as correcoes anteriores.

Gerador da distribuicao: `tools/build_portable.py`. O lancador local usa
`build/prompts-en.kypack` e `build/prompts-ja.kypack`. Nao modificar o dump,
saves normais, referencia third_party/godzilla-core ou projeto Godzilla.

Validar a imagem no tamanho real e no jogo, tanto teclado quanto controle.
Testes abertos pelo agente devem ser mudos (`SDL_AUDIO_DRIVER=dummy`) e usar
perfil privado; preservar audio dos atalhos normais. Atualizar o diario e
fazer commit apenas dos scripts/textos/metadados proprios.
