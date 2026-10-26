# Briefing para o Codex — desempenho do giro de câmera (26/10/2026)

Leia antes: `AGENTS.md`, o fim de `docs/DIARIO.md` e `docs/DESEMPENHO.md` (seções de 25/10).

## Onde trabalhar
- Crie um worktree/ramo próprio a partir de `nucleo-gow3`: `desempenho-codex`. Não mexa em
  `translations/`, `tools/translate_batch.py` nem na pasta `G:/kyoei-toshi-work/translation/pt-work`
  (Claude está fazendo o português lá).
- Há um único núcleo gráfico agora: `build/kyoei.exe` (GoW3 PC core, `gpu-gow3/`). O núcleo
  antigo (Godzilla/kyoei_videoout) foi removido em 26/10; não procure por ele.
- Não modifique `third_party/` (cópias fixas). Adaptações vão em `gpu-gow3/CMakeLists.txt`
  (`gow3_adapt`) ou em `gpu-gow3/patches/*.patch` (`gow3_patch`).

## O problema
Parado ~50-60 FPS; girando a câmera ~35-45 FPS (PC do Davy, 1080p). O quadro é uma corrente
serial: jogo -> tradução de comandos (~12-13 ms, thread `shadPS4:GpuCommandProcessor`) ->
gravação (~4 ms, `gow3:DrawRec`) -> GPU/apresentação (~11 ms). O jogo espera o fim para
começar o próximo quadro; ninguém satura. Alvo: encurtar a tradução de comandos
(`Liverpool::ProcessGraphics`, `Rasterizer::PostDraw`, `Rasterizer::Draw` ~58% dessa thread)
e a espera do fim da submissão (`DrainDrawPipe`, ReasonSubmissionEnd). Sem mudar a imagem e
sem mexer na simulação do jogo.

Já testado sem ganho (não repetir): Mailbox, `GOW3_FRAMES_AHEAD=2`, `GOW3_GC_BUDGET_MB=5000`,
`GZ_VBLANK_HZ=0`/`GZ_FPS_LIMIT=0`, `KYOEI_FRAME_CAP=0`, `GOW3_STALE_READBACK=1`,
`GOW3_ASYNC_SHADERS=1`. Não antecipar escritas de fence/GpuIdle: corrompe o heap do jogo
(ver nota em DESEMPENHO.md).

## Como medir (importante)
- Rota de teste em segundo plano, mudo, rua do início com giro interno:
  `powershell -File build/research/gow3-first-20261021/route.ps1 -Run <nome>`
  com `KYOEI_BENCH_STATS=1 KYOEI_BENCH_WIDTH=1920 KYOEI_BENCH_HEIGHT=1080`.
  FPS: linhas "Kyoei frame stats" em `game-<nome>.log` (as 3 últimas = giro).
  Captura final: `scene-<nome>.png`.
- NUNCA medir FPS com `GOW3_FRAME_STATS=1` (o diagnóstico derruba o FPS). Ele serve só
  para ver as esperas (`Draw pipe:` no console).
- Sempre ABBA (antiga, nova, nova, antiga) com o mesmo ambiente. Guarde uma cópia do exe de
  referência antes de mudar.
- Peça ao Davy para fechar Wallpaper Engine e qBittorrent antes de medir; com eles abertos
  os números variam muito.
- Perfil: `KYOEI_SAMPLE_THREADS=*` (ou um nome) + `KYOEI_SAMPLE_SLOW_MS=20` (só quadros
  lentos) + `KYOEI_SAMPLE_START/SECONDS`; o relatório sai no fim da janela, no log.
- Testes abertos pelo agente ficam no mudo (o `run.ps1` da rota usa `SDL_AUDIO_DRIVER=dummy`).

## Regras
- Cada mudança: compilar (`cmake --build build`), rodar os testes (`build/test_kyoei_*.exe`),
  ABBA, comparar a captura antes/depois. Se o FPS cair, compare com um exe antigo antes de
  supor ruído (a regressão de 22/10 passou dias despercebida assim).
- Um commit por avanço que funciona, com números no diário (`docs/DIARIO.md`) e em
  `docs/DESEMPENHO.md`. Fale com o Davy em português simples, dizendo o que ele deve ver.
