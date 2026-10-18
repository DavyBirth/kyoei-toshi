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

Para batches novos (stage12), `address/bytes` guardam respectivamente chamadas
host de draw/dispatch visitadas naquele batch/thread, reutilizando os contadores
da amostragem. Nao sao enderecos, bytes, primitivas GPU nem quantidade de quadros.
Traces anteriores deixavam ambos em zero. Serve para conferir variacao de carga;
nao elimina esperas aninhadas e um batch pode incluir varias submissoes.

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

## Rota com contagem por batch

`stutter-counts-route-20261018`:3.383intervalos/90,121s,37,54FPS;
p99=50,06ms,p99,9=83,14ms,max133,30ms;35>50ms e2>100ms. Exit0,
38.882eventos/zero descartes, sem capturas ou amostrador dentro da janela.
Contexto final HP90, diferente da primeira rota HP100: nao comparar medias
dessas partidas como ganho. Nenhum tuning novo ativo; stutter reproduzido.

O batch associado ao maior intervalo leva134,26ms e visita2.043draw/17dispatch;
callback8,48ms explica parte pequena. Outro de109,31ms visita1.509draw/18dispatch,
com readback256bytes20,26ms. Na cena parada as medianas eram1.383..1.388draw.
Mais chamadas contribuem em alguns casos, mas nao explicam sozinhas a variacao.
Antes de outro tuning, distinguir execucao CPU de desagendamento/esperas;
ciclos de thread permitem uma coleta adicional, descrita abaixo.

## Ciclos da thread por batch (Windows, opt-in)

`KYOEI_STUTTER_CYCLES=1`, junto de `KYOEI_FRAME_STATS=1` e do caminho do
trace, consulta `QueryThreadCycleTime` apenas na entrada/saida do batch12.
O stage112 conserva exatamente thread/end_ms/duration_ms do stage12; address
guarda a diferenca de ciclos e bytes fica0. Falha da API, troca de thread ou
contador regressivo omitem a amostra; zero ciclos valido continua registrado.
O analisador associa esse metadado ao batch exato como `batch_thread_cycles`,
sem criar outro escopo ou alterar os percentis de apresentacao. Traces antigos
continuam aceitos. Limite/overflow/fechamento do recorder continuam iguais;
o evento adicional consome uma entrada do limite existente.

Sao ciclos da thread durante o batch inteiro, incluindo trabalho aninhado;
nao sao ciclos da GPU, milissegundos ou porcentagem de utilizacao. Nao converter
usando GHz presumidos: frequencia e escalonamento podem variar. Comparar ciclos
por chamada e duracao em contextos equivalentes ajuda a decidir a proxima
investigacao, mas nao identifica sozinho a origem de uma espera. As consultas
adicionam custo e ficam desligadas por padrao, sem consulta por draw.

Testes cobrem falha/regressao/troca de thread/zero valido, API real Windows,
associacao exata entre eventos e preservacao dos percentis no analisador.

Coleta real `stutter-cycles-route-20261018`: exit0,45.752eventos/zero descartes,
2.825intervalos/90,071s,31,36FPS,p99=50,32ms,max117,01ms;56>50ms/2>100ms.
Todos2.825batches inteiros na janela tinham par de ciclos. Medianas do batch:
32,196ms,46.328.322ciclos,1.642draw e27.043ciclos/draw (medianas independentes).
Maior batch124,597ms:2.038draw/17dispatch,255.536.242ciclos,125.386ciclos/draw;
o outro de103,659ms:1.772draw/17dispatch,216.284.345ciclos. Assim, nesses picos
ha tambem consumo elevado de ciclos, alem da variacao no numero de chamadas.
Isso favorece perfilar trabalho interno da thread; nao isola funcao culpada,
nao exclui esperas/desagendamento e nao converte ciclos em tempo de CPU.

Captura posterior: personagem em pe/HP90, fogo e desfoque presentes;
163/163checks sem foco. Sem compilacao/captura/amostrador na janela medida.
Nao e A/B da instrumentacao nem prova de ganho/regressao: trajetoria e contexto
podem variar, e as consultas extras tem custo. Stutter continua reproduzido.
