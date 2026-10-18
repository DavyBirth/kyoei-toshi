# Serie individual de quadros e escopos - 18/10/2026

Os resumos antigos agrupam 120 intervalos, e os relatos de chamadas lentas
param depois de oito registros por categoria/thread. Isso permite perder
evidencias de picos que aparecem depois do aquecimento ou durante movimento.

`KYOEI_STUTTER_TRACE=<arquivo.csv>` junto de `KYOEI_FRAME_STATS=1` coleta
intervalos individuais de apresentacao e escopos CPU de pelo menos 8 ms.
Os eventos ficam num vetor reservado, limitado a 131.072 registros e protegido
por mutex. Nao ha escrita por quadro nem crescimento depois da reserva.
O dump ocorre no encerramento; fechamento e escritores sao sincronizados.
Escritas depois do fechamento sao ignoradas. Excesso de capacidade e declarado
como `dropped`, nunca sobrescrito silenciosamente. O analisador recusa traces
truncados para comparacoes de cauda. O caminho fica inativo por padrao.

CSV: `stage,thread,end_ms,duration_ms,address,bytes,diagnostic`. Relogio
monotonico do host, sem alteracao dos relogios do jogo. Stage100 e um intervalo
entre apresentacoes; stages0..21 seguem as categorias de `kyoei_frame_stats.h`.
Escopos de draw/dispatch conservam a amostragem1/128; outros escopos mantem
as chamadas instrumentadas originais. Bytes/endereco sao metadados opcionais.
O marcador diagnostic identifica sobreposicao com F9; nao prova ausencia de
outras interferencias. O proprio trace tem custo e deve ser igual nos controles.

`tools/analyze_stutter_trace.py` calcula percentis de intervalos individuais,
limites estritos >50/>100ms e escopos que se sobrepoem aos20piores intervalos.
Nao soma escopos aninhados. Sobreposicao temporal nao prova causalidade nem
mede execucao da GPU. `--range` exige limites monotonicos validos; os scripts
privados registram QPC em `start_clock_ms/end_clock_ms` para excluir menus,
aquecimento e capturas posteriores. Testes verificam percentis, bordas,
diagnosticos e sobreposicao aninhada, alem do fechamento concorrente do writer.

## Primeiras evidencias

Referencia parada, `stutter-baseline-20261018`, sem trace: cinco minutos depois
do aquecimento, 12.960 intervalos resumidos, 43,21 FPS, dois intervalos >50ms,
nenhum >100ms, maximo51,89ms. Isso nao reproduz suficientemente o problema
durante movimentacao e nao demonstra stutter resolvido.

Rota `stutter-route-20261018`: giro, caminhada e corrida no inicio, efeitos da
rua, resto da janela sem novos comandos. Sem capturas nos90s medidos. Trace
gravado com38.306eventos, zero descartados. Janela selecionada:3.176intervalos,
90,169s, 35,22FPS; p95=34,19ms, p99=50,06ms, p99,9=98,19ms, max131,35ms.
Foram37intervalos >50ms e tres >100ms. Exit0,163/163 verificacoes sem foco.

Os maiores intervalos coincidem com batches de comandos longos. No intervalo
de117,53ms ha uma leitura de256bytes esperando21,25ms; nao explica sozinha
o intervalo inteiro. No de131,35ms o batch ocupa128,31ms da janela, sem outro
escopo longo especifico que identifique ainda o trabalho interno. Nao atribuir
todos os picos ao readback. Sao duas cargas diferentes; nao comparar suas
medias como regressao da instrumentacao. Mais investigacao e necessaria.

Dados detalhados, janelas e capturas permanecem privados em `build/research`.
Nenhuma correcao de stutter foi demonstrada apenas por esta instrumentacao.

## Detalhamento opcional dos pacotes

`KYOEI_STUTTER_PACKETS=1`, junto do trace, mede cada pacote, draw e operacao
de imagem ja coberta por `KyoeiStreamingTrace::Scope`. Registra apenas duracoes
>=8ms: stages200..206 para pacote/draw/find-image/refresh-image/texture-gc/
image-buffer/command-callbacks;299 para outros. O endereco de pacote representa
o opcode PM4. Stage210 mede espera >=1ms pelo backlog da gravacao Vulkan;
211 mede chunks >=8ms, com bytes de payload do host. Nao sao tempos da GPU.
Este modo acrescenta consultas de relogio por pacote: serve para localizar
trabalho, sem usar seu FPS como prova de ganho ou regressao.

A rota cooperativa `stutter-route-coop-20261018` teve2.287intervalos/90,405s,
25,30FPS,408>50ms/10>100ms e max166,18ms, sem descartes. A captura final
mostra personagem caido e HP80, enquanto o controle terminou em pe com HP100.
Os mesmos comandos por tempo nao garantem o mesmo contexto. Nao atribuir a
diferenca numerica inteira ao readback cooperativo nem reivindicar A/B
controlado. Caminho permanece desligado por padrao, sem beneficio demonstrado.
