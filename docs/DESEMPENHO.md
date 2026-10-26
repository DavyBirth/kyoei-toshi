# Desempenho — objetivo de 60 FPS

## Verificações de 19/10/2026

Quatro rodadas ABBA com superfície interna1920x1080 deram36,82FPS com saída
1080p e36,70FPS com saída720p: diminuir só a janela não trouxe ganho útil
nesta vista parada. [Método e limites](re/SAIDA-720P-1080P.md). Não confundir
com escala interna ou eliminação de travadas em movimento.

As consultas de orçamentoVulkan também não explicaram picos grandes: nos
grupos reportados de5.760chamadas por cache, texturas somaram60,66ms e buffers
16,33ms; máximos0,2305/0,1157ms. A política de cache não foi alterada.

Também foi comparado `KYOEI_FRAME_STATS=1/0/0/1`: 41,10 / 41,37 /
40,24 / 40,38 FPS. Médias ponderadas: 40,74 ligado e 40,81 desligado
(diferença de 0,17%, sem benefício consistente). Mesma compilação
`55ddcde38c5807932d6b80baf6dc31dd68608705403a5e132a818a1e40714d21`,
janela e superfície 1920x1080, posição parada/HP100/câmera conferidos nas
quatro capturas, 60s de aquecimento após stage201 e cerca de20s por coleta.
Todos sem foco durante a coleta, exit0, zero descartes e nenhum frame>50ms.
`KYOEI_PRESENT_INTERVALS_ONLY=1` manteve a medição de apresentações em ambos;
trace ativo, ciclos por thread desligados em ambos. Isso não mede o custo
de todos os diagnósticos nem representa uma partida em movimento. Mantida
a configuração do launcher; evidência privada em `build/research/`, resumo
`stats-abba-summary-20261019.json` e relatórios `frame-stats-abba-{1..4}-20261019.json`.

## Resultado mais recente: 18/10/2026

A compactacao correta dos registros que protegem a memoria de upload ganhou
4,83% no A/B local: 36,01/37,75 FPS. Testes sinteticos e na GPU real conferiram
que cada regiao continua protegida pelo mesmo envio. A alternativa de cache
dos dados foi descartada por resultados inconsistentes.
[Metodo e limites](re/STREAM-WATCHES.md). Os maximos dessa coleta nao melhoraram;
a meta de 60 FPS e a eliminacao global das travadas continuam pendentes.

## Resultado mais recente: 17/10/2026, segunda rodada

Depois do ganho local de 34,8% com gravacao Vulkan e cache fetch, o cache com
duas entradas por grupo ganhou mais 4,8% em outro A/B na mesma partida:
33,70/35,32 FPS. A leitura de offsets tambem deixou de copiar o vetor de
atributos a cada draw. [Metodo, controles e limites](re/GRAVACAO-VULKAN.md).
Os percentuais pertencem a sessoes diferentes e nao formam uma medida conjunta.
A meta de 60 FPS e a eliminacao global de stutter continuam nao demonstradas.

## Sessão autônoma de 17/10/2026

Davy autorizou continuar os testes locais até resolver performance, mantendo
fidelidade ao PS4 e testes mudos. Confirmou parar ao restarem 13% da cota,
checar a cada cinco minutos, salvar e desligar o PC. O monitor privado desta
sessão está em `build/overnight/performance_watch.ps1`; PID, histórico e
checkpoints ficam na mesma pasta ignorada. Não integra launcher ou pacotes.

`scripts/check_usage.ps1 -SessionPath <rollout.jsonl> -ReservePercent 13`
consulta a telemetria da conversa explicitamente escolhida. Não selecionar
automaticamente o arquivo de um auxiliar mais recente. Leitura limitada a
8 MiB, ignora escrita final incompleta e marca dados antigos/futuros como
inválidos para disparar desligamento. `tools/checkpoint_runtime.py` guarda
diff binário, arquivos modificados/novos não ignorados e hashes, verifica o
ZIP e sincroniza sua gravação. Quatro regressões do leitor passaram.

O monitor lê e salva a cada 300 segundos; no limite fresco, bloqueia novas
partidas de teste, encerra tarefas identificadas do projeto, registra o diário,
verifica checkpoint final e solicita shutdown sem forçar descarte de outros
aplicativos. Falha de checkpoint impede desligamento automático. Para cancelar
esse monitor: criar `build/overnight/stop-performance-watch`. O marcador de
parada de testes é `build/overnight/performance-stop-requested`.

Referência de produto: [OpenAI Docs sobre uso compartilhado de Work/Codex e
consulta dos limites](https://learn.chatgpt.com/docs/pricing?translationFallback=pt-BR).
Os percentuais observados vêm dos registros locais, não de estimativa por tokens.

Aprofundamento em [TEMPO-TAREFAS-ENTIDADES.md](re/TEMPO-TAREFAS-ENTIDADES.md):
o estágio 44 da inicialização do Kyoei troca para o cálculo de tempo medido.
O padrão fixo do construtor base não explica, sozinho, o gameplay. Mapeadas
escalas de tempo, fila de tarefas e atualização concreta da HUD; nenhum ganho
de FPS ou correção de slow motion foi aplicado/comprovado nesta investigação.

Mapa estático do quadro, cálculo do tempo e caminhos de HUD:
[QUADRO-HUD-TEMPO.md](re/QUADRO-HUD-TEMPO.md). Identifica o laço e os dois
cálculos de timestep com evidências reproduzíveis; o modo ativo na cidade
e a causa da câmera lenta continuam pendentes de observação.

## Backend Windows — 15/10 à noite

Removida a limpeza incondicional dos buffers de captura raster em cada desenho
direto/indireto. `BeginDraw()` só limpa quando `KYOEI_RASTER_TRACE=1`. O estado
é exclusivo do diagnóstico, sem acesso quando ele está desligado. Quando está
ligado, a limpeza continua antes mesmo do pedido F9, pois o pedido pode chegar
entre o começo do desenho e a leitura dos recursos. Isso reduz trabalho CPU
desnecessário; não há ganho de FPS medido nem correção comprovada de slow motion.

`KYOEI_FRAME_STATS=1` agora mede `command-batch` (drenagem de comandos no
worker, incluindo esperas internas) e `present-pacing` (espera do temporizador
da apresentação). Não são tempos GPU nem CPU ativo; não somar suas médias a
readback/scheduler nem tratá-las como etapas independentes de cada quadro.

Cada janela também registra contagens reais de intervalos acima de 16,67 ms,
33,33 ms, 50 ms e 100 ms, antes de arredondar. O analisador soma essas contagens
separando captura diagnóstica e janelas sem captura. Logs antigos continuam
válidos, mas não ganham contagens inventadas. Oscilação perto de 16,67 ms pode
ultrapassar o limite mesmo em apresentação média de 60 Hz: não é contagem de
quadros perdidos, taxa da simulação nem prova de câmera lenta.

`tools/review_ui_session.ps1 -PerformanceStats` prepara um perfil separado e
registro com data, sem sobrescrever a evidência de UI. Desliga captura raster,
stress de flip, trace de HUD/display/quebra e diagnóstico dos prompts. Não
abrir automaticamente: Davy pediu seguir só no backend e testar depois.

Evidência anterior: registro pessoal de 15/10 resumido em
`build/performance-current-20261015.json`: 302 janelas; mínima por janela de
16,78 FPS e scheduler-wait médio de 10,94 ms nas cinco janelas disponíveis.
Mistura menus/jogo; a média global não caracteriza a cidade. Sessão isolada
aberta antes do pedido de backend chegou à abertura, registrou as novas
métricas e encerrou com código 0, sem carregar gameplay. Não usá-la para
comparar desempenho. Portable ainda não atualizado com estas mudanças.

## Correção do timeout de encerramento — 15/10

Antes, o encerramento enfileirava Sync no thread gráfico e esperava três
segundos. Fixture ocupando esse thread real por 4,2 segundos reproduziu timeout
antes da captura (phase=0, exit=24): build/cache-timeout-repro/busy-before.log.

Agora o blob Vulkan é capturado após warmup e quando há pipelines novos,
com checagem amortizada e intervalo de 30 segundos por thread. Captura fica no
thread proprietário; apenas bytes seguem para gravador CPU, fila limitada e
substituição atômica. Saída aguarda até três segundos só pelas gravações já
enviadas, sem tarefa nova na fila gráfica. Últimos pipelines desde a captura
podem ser recompilados depois. Não fecha concorrentemente o banco de shaders.

Mesmo teste ocupado passou: flush em 0,03 ms, exit=0 e arquivo válido salvo
(busy-after.log). Testes unitários cobrem persistência, dados inválidos,
preservação do cache anterior, fila limitada, espera com prazo e agendamento.
Nenhum código do jogo executado; não é ganho de FPS medido. Blob da fixture
tem só 36 bytes; cópia do cache pessoal encontrou perfil incompatível e não
comprova recuperação de pipelines de uma partida. Disco lento ainda pode
exceder o prazo; conserva-se a versão atômica anterior. Tempo de captura Vulkan
é registrado para avaliar custo durante gameplay.

## Teste portable de Davy e continuação em 15/10

`build/portable-performance-20261015.json`: 174 janelas, 20.880 intervalos,
23,64 FPS agregados; janelas de 11,90 a 60,06 FPS, pausa máxima 767,32 ms.
Menus/carga/jogo misturados; não é benchmark comparável de uma mesma rota.
Compilações observadas: shader até 22,91 ms, pipeline gráfico até 39,59 ms.
Readback-finish: 11 janelas, média 14,98 ms, máximo 37,83 ms; leituras de
uma imagem/256 bytes. Não somar médias de threads nem atribuir toda pausa
à compilação. Prepare 0,21 ms e Present 0,63 ms agregados.

Mudanças compiladas/testadas nesta continuação:

- Mensagens de sucesso de mapeamento/desmapeamento ficam opt-in com
  `KYOEI_MEMORY_TRACE=1`. Erros/relatórios continuam visíveis. Evita lock,
  stdout e fflush por mapeamento durante streaming; ganho ainda não medido.
- Novas métricas `scheduler-submit`, `scheduler-wait` e `queue-lock` separam
  preparação/submissão, espera por timeline e disputa pelo mutex da fila.
  Tempos de parede por thread, não timestamps GPU. Sem remover Finish.
- A referência Bloodborne consultada também usa Finish antes da publicação
  de readback síncrono. Copiar somente Flush não preserva esse contrato.

Próxima medição: mesma rota com cache quente/frio, usando o portable atualizado;
observar as três métricas novas junto a readback e compilações. Meta de hardware
RX 580/i5-9400 ainda sem teste nessa máquina. Linux x64: avaliação em LINUX.md.

## Continuação de 15/10

Log 20261015_023230:73 janelas/8.760 intervalos,31,66 FPS agregados,
17,73–60,08 por janela e máximo516,21ms. Inclui menus/transições: não é percurso
controlado. Prepare0,15ms, Present0,48ms; cinco janelas readback-finish10,86ms
em média (máximo28,47ms). Não somar tempos de threads como caminho crítico.

Cache Vulkan complementar ao cache de shaders: vendor/device/pipelineCacheUUID,
cabeçalho versão1,32..64MiB, fallback vazio se rejeitado. Criar antes do WarmUp.
O portátil começa sem cache. Runtime usa TerminateProcess; saída normal pede
Sync pela fila GPU com espera máxima3s e substituição atômica. Erros não gravam.
Unidade verifica identidade, truncamento, troca atômica/configuração ausente.
Build PASS; benefício real pendente do teste de partida bloqueado por revisão.

KYOEI_FRAME_STATS acrescenta shader-compile/graphics-pipeline/compute-pipeline.
Agrega120 chamadas por thread/categoria. Até oito pausas por categoria/thread
acima de16,67ms ficam registradas individualmente, para aberturas com poucas
compilações. Analisador não interpreta esses registros como contagem completa.
Isso não mede tempo GPU isolado nem demonstra60 FPS.

Davy pediu em 14/10/2026 análise minuciosa com objetivo de pelo menos 60 FPS.
Orçamento de referência: 16,67 ms por quadro. Objetivo ainda não atingido nem
garantido pelo hardware/código atual. Avaliar estabilidade dos tempos, não só
média; não obter FPS artificial repetindo frames ou acelerando lógica do jogo.

## Correções de segurança e identificação em 15/10

O perfil serializado exige exatamente o tamanho da estrutura antes de comparar
bytes. Um cache truncado não pode provocar leitura além do arquivo. Teste cobre
perfil correto, diferente, curto e vazio. O bridge gerado identifica
CUSA03562/Kyoei Toshi/01.03. Usuário padrão Player, fontes relativas no portátil;
o launcher de desenvolvimento informa seu caminho explicitamente. Snapshot e
projeto irmão permanecem intactos.

Referências: [dados do cache Vulkan](https://docs.vulkan.org/refpages/latest/refpages/source/vkGetPipelineCacheData.html)
e [sincronização](https://docs.vulkan.org/refpages/latest/refpages/source/VkPipelineCacheCreateFlagBits.html).
O cache otimiza preparação; não desbloqueia a taxa da simulação.

## Evidência disponível

Percursos observados em ANALISE-COMECO.md: 22,74/25,41 FPS, misturando eventos
e rua. Não são comparação controlada entre versões. TLS zero nesses trechos;
PrepareFrame ~0,16–0,18 ms, Present ~0,39–0,43 ms; uma janela de leitura linear
~11,65 ms. Tempos de threads diferentes não podem ser somados como caminho
crítico. Percentual global baixo de CPU/GPU não exclui thread ou fila saturada.

Reanálise offline de 20261013_135113.log: 100 janelas/12000 intervalos,
34,31 FPS agregados incluindo menus/transições; janela mínima 17,02 e máxima
60,05 FPS. Não representa FPS da cidade. Linear-downloads: seis janelas,
média 10,46 ms e máximo 21,50 ms; seis amostras de readback tinham uma imagem,
uma espera e apenas 256 bytes. Investigar sincronização/frequência, não supor
que a largura de banda desses 256 bytes seja o problema. Relatório privado:
build/performance-audit-20261014.json. Log antigo não marca atividade F9;
ausência da marca significa desconhecido, não medição limpa.

## Ordem de investigação

1. Rastrear consumidor dos readbacks e dependências/fences: o que exige resultado
   imediato, quantas submissões/esperas ocorrem e se um lote realmente as reduz.
   Preservar coerência, visibilidade de memória e resultados do jogo.
2. Medir threads de lógica, tradução de comandos, compilação de shaders, fila
   GPU e esperas. Work stats atuais medem tempo de parede de trechos da CPU;
   não são timestamps GPU nem atribuição completa do gargalo.
3. Investigar pacing/limites e se a simulação assume 30 Hz antes de desbloquear
   taxa. Confirmar movimento, animação, áudio, física e cutscenes na taxa maior.
4. Comparar builds na mesma rota/save, resolução, idioma e opções; separar
   cache frio/quente, carga/menus, diagnóstico F9 e gameplay. Fazer uma mudança
   por comparação. Traduzir texto não prova nem exclui regressão de desempenho.
5. Validar imagem e correção junto ao ganho. Não reduzir qualidade, pular
   sincronização ou habilitar batch readback por palpite.

Sem jogo: revisar caminhos, testar dependências com fixtures e analisar logs.
Próxima coleta autorizada: tempos brutos/identificação de cena quando possível,
CPU por thread e GPU correlacionados. Não abrir partida automaticamente.

O analisador agora preserva diagnostic=0/1 e legado desconhecido, separados
para frame/work stats; mantém também totais e pausas de todas as janelas.
Relata orçamento de 60 FPS, mas resumos de 120 intervalos não permitem calcular
FPS mínimo, contar todos os quadros atrasados nem obter p95 global exato.
Valores impressos são arredondados; médias junto ao limite requerem dados brutos.

## Investigação de código e build — 14/10/2026

- Confirmado `RelWithDebInfo`: não era um build Debug sem otimização.
- DownloadImageMemory chama Scheduler::Finish, que submete comandos e espera
  o tick da submissão. ProcessDownloadImages é chamado antes de EventWriteEos,
  EventWriteEop e ReleaseMem sinalizarem eventos ao convidado. Não remover
  espera nem adiar publicação sem tratar essas dependências.
- GoW3 consultado na versão de FONTES-CODIGO.md implementa readback diferido
  com proteção de páginas, fila de staging, cancelamento e espera sob demanda.
  Não equivale a simplesmente trocar Finish por Flush. Kyoei mantém batch opt-in;
  amostras com uma imagem por lote não demonstram benefício desse agrupamento.
- Adicionados scopes opt-in readback-finish, readback-map e readback-publish.
  Finish inclui submissão e espera, não é tempo GPU isolado. Publish inclui
  invalidação de cache e TryWriteBacking. Map inclui possíveis esperas do ring.
  Janelas de 120 chamadas são por thread; não somar suas médias como frame.
  Teste verifica independência das amostras da thread de publicação diferida.
- Confirmado erro de hash visto também no Bloodborne: MarkAsMaybeDirty usa
  guest_size inteiro; RefreshImage comparava apenas os primeiros 8x8 pixels.
  Comparação agora usa a mesma região completa. Evita primeira invalidação
  espúria e detecta alterações posteriores fora do prefixo. Não limpa flags
  GPU nem ignora escrita CPU confirmada. Adaptação na cópia gerada por CMake,
  core fixo preservado. Não se sabe ainda a frequência desse caso no gameplay.
- Teste compila trechos dos métodos reais com XXH3 e imagem sintética: original
  reproduz falha sem alteração; corrigido preserva imagem intacta, detecta
  último byte e mantém caminho CpuDirty. Build do port e CheckOnly EN PASS.
- Apresentador usa vblank configurado e flip_rate+1; jogo pode solicitar pular
  ciclos com sceVideoOutSetFlipRate. Não provado qual valor usa na cidade.
  Próxima investigação de taxa: identificar chamadas e vínculo com simulação.

Executável normal build/kyoei.exe atualizado; diagnóstico orb-pending anterior
continua separado. Nenhuma partida iniciada, nenhum ganho de FPS medido.

## Espera GPU com menor atraso — 15/10

Adapter próprio `kyoei_wait_real_label` usava Sleep(1) entre leituras da
conclusão real. Isso pode arredondar uma conclusão curta ao tick do Windows.
Agora retorna imediatamente quando já concluída; faz espera ativa limitada
a 200 microssegundos e, se necessário, usa timer de alta resolução de 1 ms.
Hosts que não criam esse timer seguem no fallback Sleep(1). Timeout, leitura
acquire e retorno ao poll/trap original preservados. Não escreve labels,
não adianta eventos e não remove a espera do readback.

Benchmark `benchmark_kyoei_gpu_wait`, produtor separado, 200 amostras por
implementação/duração, alternando versão antiga e nova. Com pedido explícito
de timer de 1 ms: conclusões simuladas de 50/200/800 us foram detectadas em
média em 1,881/1,922/1,974 ms antes e 0,074/1,703/1,711 ms depois.
Sem esse pedido: 15,494/14,288/14,196 ms antes e 0,078/1,332/1,371 ms depois.
Resultados locais de fixture, não tempos do jogo nem comparação de FPS.
Não extrapolar o caso de tick padrão para uma partida cujo timer pode diferir.

Tradeoff medido: mais ciclos de CPU na thread que aguarda (aproximadamente
192 mil/610 mil/607 mil ciclos contra 42 mil/54 mil/57 mil, no teste 1 ms).
Espere até 200 us de trabalho ativo por chamada longa. Ciclos não são tempo
de CPU convertido por frequência presumida. Não foi medido o impacto na
RX580/i5-9400. Arquivos privados `build/gpu-wait-benchmark*.txt`.

Build Windows PASS; testes de fence atrasado, timeout sem alteração do label,
retorno imediato, relay SysV e 100 timeouts sem vazamento de handles PASS.
Portable atualizado e bootstrap EN sem GPU PASS. Ganho de FPS/stutter em
gameplay continua não medido, respeitando a instrução de não iniciar partida.


### Teste real do começo — 15/10/2026, 22:59

Davy testou o começo com ação e relatou stutter inicial seguido de melhora.
Fechamento normal, exit0; log privado native-05/logs/20261015_225918.log
(543.291 bytes). Análise em build/research/performance-20261015-225918.json.

- 124 janelas de120 intervalos:14.880 intervalos,373,502 segundos medidos.
- Média global39,84 FPS; médias das janelas17,35–60,05 FPS. Não são FPS mínimo
  e máximo instantâneos. Menus/cenas/carregamentos sem marcação se misturam.
- Maior intervalo584,04 ms;1.107 intervalos acima de50 ms,35 acima de100 ms.
- command-batch:168 janelas de120 chamadas, média18,35 ms/chamada, máximo
  519,30 ms. Mede o escopo CPU que processa lotes; não é tempo puro de GPU
  nem pode ser somado aos outros escopos sobrepostos. Principal alvo a detalhar.
- Shaders:600 chamadas resumidas, máximo13,02 ms; pipelines gráficos:600,
  máximo16,03 ms. Nenhum compile-stall registrado. Isso não prova ausência
  de custos em amostras incompletas, mas não sustenta culpar shaders pelas
  maiores travadas. TLS0; nenhuma linha de falha detectada pelo analisador.
- Pacing médio16,34 ms/chamada. Não equivale a FPS do jogo.

Serve como referência inicial reproduzível do começo; sem comparação da mesma
sequência na versão anterior, não quantifica ganho causado pelas mudanças.
Próximo: detalhar o lote de comandos e correlacionar pausas sem alterar o jogo.


### 15/10/2026 — detalhamento das pausas de comandos

- Integração CMake preserva snapshot: mede callbacks de comandos, espera
  WaitVoLabel, Draw/DrawIndirect, DispatchDirect/Indirect e CpSync.
  Conferidos1 callback,1 label wait,16 draws,9 dispatches e1 CpSync na cópia
  gerada. Escopos encerram antes de qualquer co_yield; não cobrem a vida
  inteira de uma corrotina. Medem duração de chamada CPU, incluindo esperas
  e subchamadas; não são tempos GPU e não podem ser somados entre si.
- Novas categorias14–18 no frame_stats. Pausas>=50ms nas categorias12–18
  geram até8 registros por categoria/thread, com término no relógio monotônico
  para conferir sobreposição. Registro esparso cobre janelas incompletas;
  limite não é contagem total de travadas. Analisador preserva atribuição
  diagnostic e timestamps, sem inferir causalidade automaticamente.
- Compilação kyoei/test_kyoei_frame_stats PASS; teste C++ ativo/desligado,
  limite8 de10 pausas e analisador Python PASS. Launcher EN PerformanceStats
  CheckOnly PASS. Nenhum jogo iniciado. Warning u8path preexistente.
- build/kyoei.exe atualizado; mesmo Medir desempenho atual EN.cmd. Portable
  intacto. Próximo teste: repetir começo para distinguir espera de apresentação
  e chamadas de renderização. Ainda sem correção de performance nova: medir
  antes de escolher alteração de sincronização.


### 15/10/2026 — segundo teste e correção do custo de diagnóstico

- Davy testou novamente; log privado20261015_231142.log,5.728.756 bytes,
  exit0.46 janelas/5.520 intervalos/188,749s,29,25 FPS globais mistos,
  intervalo máximo551,09ms;40 intervalos>100ms. Não comparar diretamente
  aos39,84 FPS da sessão anterior: sequência/duração diferentes e coleta mudou.
- video-label-wait: máximo30,57ms; scheduler-wait241,76ms, readback-finish
  241,97ms, linear-downloads241,98ms; prepare292,74ms. Essas durações são
  inclusivas, não somar. A espera de liberação da imagem não explica sozinha
  as maiores pausas; leitura sincronizada de dados GPU é alvo concreto, sem
  prova de que explique todas as travadas. command-batch máximo491,50ms.
- Falha na escolha de frequência do diagnóstico:54.419 janelas raster-draw
  (6.530.280 chamadas medidas) e570 dispatch. Mais de54mil linhas geradas;
  timers/sort/log podem perturbar desempenho. Sessão não serve como A/B limpo.
- Corrigido: raster-draw/dispatch coletam1 em128 chamadas, antes de ler relógio.
  Agregados mantêm120 amostras e indicam sample_stride=128; analisador preserva
  metadado e explica que máximos/pausas cobrem só as chamadas selecionadas.
  Demais categorias permanecem integrais. Sem alegar ganho medido no jogo.
- Build kyoei e teste C++ PASS, modo desligado PASS, teste Python PASS.
  Teste confirma1 registro por15.360 chamadas amostradas, limite de pausas
  preservado. Nenhuma partida aberta pelo agente. Portable intacto.
- Próximo: investigar a sincronização de readback; não ativar batching como
  solução automática, pois logs conhecidos trazem lotes de1 imagem.


### 15/10/2026 — readback: investigação e evidência esparsa

- Cadeia conferida: FindTexture(Storage)/FindRenderTarget marcam imagens lineares
  modificadas; ProcessDownloadImages grava image->buffer, Finish submete a fila
  e aguarda o tick antes de publicar em memória guest. Não remover a espera
  sem resolver dependências: o jogo pode consumir esses resultados.
- Lotes registrados no segundo teste:1 imagem/1 espera/256bytes. Não basta
  habilitar batching; tamanho de transferência sozinho não explica fila longa.
  Não foi estabelecido que o máximo de242ms pertença ao mesmo lote256bytes.
- Medição nova preserva essa correlação: até8 esperas>=50ms em readback-finish
  e scheduler-wait por thread/categoria. Readback síncrono inclui bytes,largura,
  altura e término monotônico. Assim compara fila/espera/cópia sem per-frame spam.
- Build/teste C++ ativo/desligado e analisador Python PASS, incluindo registro
  de256bytes/8x8 sintético. Nenhum jogo iniciado, fence alterado ou ganho prometido.


### 15/10/2026 — base da investigação: contadores da fila GPU

- Davy pediu começar pelo começo. Sem nova sessão após231142; não é possível
  atribuir a pausa242ms a um recurso específico nos registros antigos.
- Conferidos Scheduler::Finish/Wait e MasterSemaphore::Wait: Finish submete
  antes de aguardar; Wait só faz flush adicional se tick ainda não enviado;
  MasterSemaphore retorna cedo quando tick conhecido/com refresh já terminou.
  Nenhuma espera duplicada removível demonstrada. Snapshot preservado.
- Integração Kyoei passa a registrar requested/next lógico/known_before/
  known_after/forced_flush em Scheduler::Wait>=50ms, até8 por thread. Usa
  contadores já disponíveis, sem consulta Vulkan adicional. Tempos e diagnostic
  correlacionam com readback. Contadores known são cache, não timestamps GPU,
  e diferença de ticks não representa quantidade de desenhos nem tempoGPU.
- Build kyoei/teste C++ PASS; teste limiar49ms e limite8/10 PASS; analisador
  Python PASS; launcher PerformanceStats CheckOnly PASS. Jogo não iniciado.
  Sem nova otimização, cache apagado ou promessaFPS. Updates ZIP anteriores
  permanecem identificados e não foram regenerados para esta coleta local.
- Próximo: Davy repetir começo em Medir desempenho atual EN.cmd para capturar
  readback bytes/dimensões junto dos contadores. Só então escolher correção.


### 15/10/2026 — teste233924 e primeira redução de consultas por desenho

- Log privado20261015_233924.log,424.152bytes,exit0.41 janelas/4.920 intervalos,
  149,489s,32,91 FPS globais mistos,máximo567,34ms.753 intervalos>50ms,
  34>100ms. Médias não são comparação controlada com sessões anteriores.
- Nenhuma esperaScheduler/readback>=50ms nesta sessão; janelas completas:
  scheduler-wait máximo27,16ms,readback-finish27,31ms,linear27,33ms.
  video-label máximo23,26ms. A pausa242ms anterior não se reproduziu;
  não explica sozinha as travadas recorrentes. Sem erro detectado no analisador.
- Sampler128 ativo:354 janelas raster-draw,contra54.419 na coleta antiga;
  ainda indica milhões de chamadas no trecho. Não extrapolar duração exata
  totalGPU a partir das amostras. command-batch máximo440,02ms.
- Achado concreto no código: Draw/Dispatch chamam PopPendingOperations; antes
  havia Refresh/getSemaphoreCounterValue mesmo com fila pending_ops vazia.
  Cópia integrada Kyoei agora retorna cedo quando a fila está vazia.
  Fila com callbacks mantém Refresh/FIFO/IsFree/publicação originais.
- Conferidos ResourcePool::CommitResource (Refresh quando busca falha),
  Scheduler::IsFree (Refresh quando necessário), SubmitExecution (Refresh
  após envio antes de alocar commandbuffers), MasterSemaphore::Wait (Refresh
  antes de bloquear). Não removidas essas atualizações nem fences/readbacks.
- Chave KYOEI_POLL_EMPTY_OPS=1 restaura polling vazio anterior no mesmoexe
  para comparação. Padrão ativa retorno cedo. Snapshot/Godzilla intocados.
- Build PASS; testes existentes readback-batch/GPU-wait/frame-stats e
  analisador PASS; launcher CheckOnly PASS. Esses testes não substituem
  gameplay ou provam ganhoFPS. Novo build local pronto para comparar começo.
  Não iniciado jogo nem regenerados ZIPs dos amigos antes da validação.


### 15/10/2026 — teste234620 do retorno cedo com pending_ops vazio

- Log privado20261015_234620.log,392.698bytes,exit0;52 janelas/6.240 intervalos,
  198,611s,31,42FPS globais,máximo666,10ms;855 intervalos>50ms,36>100ms.
  Sessão anterior149,489s/32,91FPS:não comparar médias como regressão/ganho.
- Blocos por ordem de1200 intervalos:56,25/52,02/25,03/24,65/23,92FPS e
  último parcial32,35. Antes:56,43/52,14/23,91/23,35 e parcial33,04.
  Pequena diferença nos blocos lentos é indício,sem alinhamento de cena não
  é ganho comprovado. Nenhum erro detectado; não implica validação visual.
- Correlação nova concreta: imagem1x1,pitch/download256bytes; leitura91,41ms
  termina40348010,28ms,esperaScheduler90,69ms termina40348009,72ms,
  requested9339/next9340/known_before9332/known_after9339,forced_flush0.
- Segunda imagem1x1/256bytes:readback132,83ms termina40362401,54ms,
  espera132,29ms termina40362401,17ms,requested10011/next10012/
  known_before10010/known_after10011,forced_flush0. Escopos inclusivos.
  O tick exigido já estava submetido; não houve flush extra nesse caminho.
  Não é largura de banda da cópia por si só; investigar trabalho anterior,
  agendamento e ponto de consumo. Contador antes é cache, não fila GPU viva.
- video-label máximo22,68ms; shaders máximo8,56ms nos600 registros resumidos;
  command-batch máximo665,27ms. Essas leituras não explicam todas as travadas.
- Otimização de fila vazia permanece candidata local; sem aumentoFPS validado
  nem update novo para amigos. Próximo backend: identificar produtor/consumidor
  da imagem1x1 antes de considerar leitura adiada. Não remover fences.

### 25/10/2026 — giro: varredura dos registradores sujos
- Perfil do giro (amostrador, sem FSR): PostDraw era 16,6% "self" da thread GpuCommandProcessor:
  no Windows o laço testava cada um dos blocos de registradores com bitset::test por desenho.
  Patch kyoei_dirty_blocks_scan_cpp varre as palavras do bitset (como o caminho libstdc++).
  Depois: PostDraw 4,7%; a thread passa a esperar mais.
- FPS e latência envio→flip não mudaram (ABBA com ruído de outros programas: 49-50 nas duas;
  submit_to_flip ~26 ms nas duas). DrawRec fica ~33% ociosa esperando pacotes; a thread de
  comandos espera o fim da gravação no fim da submissão. O quadro inteiro só começa a ser
  traduzido quando o jogo o envia, então a latência é a soma das etapas por quadro.
- Mantido por ser mais barato e equivalente.
- Perfil de todas as threads no giro (KYOEI_SAMPLE_THREADS=*): nenhuma satura. Ocupação
  aproximada: guest main ~58%, 4 threads de tarefas do jogo ~43%, GpuCommandProcessor ~50%
  (24% em ZwDelayExecution dentro de DrainDrawPipe), DrawRec ~60%, VkRecorder ~59% (21% no
  driver), DrawPrep/DrawScan/Copy quase ociosas. O quadro é uma corrente: jogo -> comandos ->
  gravação -> driver, cada etapa esperando a anterior.
- Próximo alvo: os esvaziamentos do draw pipe (DrainDrawPipe: fim de submissão, compute,
  ReasonDraw...). Medir por local com GOW3_FRAME_STATS (drain_sites) e ver se o Kyoei
  esvazia várias vezes por quadro (há 3 dispatches de compute fc5139ae no meio das
  transparências). Cuidado: o esvaziamento no fim da submissão protege os rótulos de fence
  (heap do jogo corrompido se escritos tarde).
- Medido (25/10, giro, GOW3_FRAME_STATS=1, run drains-1): o único esvaziamento que espera é
  o do fim da submissão (liverpool.cpp:184), ~1 por quadro (44-59/s), 18-25% do tempo da
  thread de comandos. Compute, ConstRam, ReasonDraw e os demais não esperam: a hipótese de
  vários esvaziamentos por quadro está descartada.
- Leitura: essa espera é a thread de comandos aguardando a gravação (DrawRec, 50-64%
  ocupada) terminar o quadro antes de avisar GpuIdle ao jogo. Não é trabalho desperdiçado;
  só encolhe se a gravação ficar mais rápida, ou se as escritas de fence puderem ser
  sinalizadas depois sem o jogo liberar a memória antes (arriscado: ver a nota acima).
  Próximo alvo: o custo por draw do DrawRec.
- Perfil da thread DrawRec no giro (25/10, prof2): ~30% das amostras dentro do flip
  (SubmitFlip -> PrepareFrame), esperando a GPU: Scheduler::Wait do limite GOW3_FRAMES_AHEAD=1
  (~13%) e GetRenderFrame esperando o present_done de um quadro anterior (~16%). O resto:
  DrawRecord/BindResources/BindTextures.
- REGRESSÃO achada (25/10): desde 53e5cf8 (22/10, overlay do Kyoei no presenter GoW3) o
  jogo caía de ~58 para ~33 FPS parado. Ao fechar um diálogo (o de carregar o save) a
  revisão do overlay mudava, mas só era confirmada quando o overlay estava visível: depois,
  kyoei_overlay_needs_redraw ficava verdadeiro para sempre e o driver chamava DrawLastFrame
  em todo vblank livre, disputando o presenter com os flips do jogo. Busca binária com
  builds em worktree (bis-*: a241010/bf83e49 ~53 parado; 5370d4a/d44aac1/4c5a9cf ~33).
  Correção: Gow3Overlay::Visible() confirma a revisão quando nada do overlay está visível
  (kyoei_gow3_stubs.cpp). Depois: parado 57-59, girando 41-50 (Mailbox sem limite).
- As esperas no flip e o ganho de GOW3_FRAMES_AHEAD=2 (~6%) eram efeito da regressão:
  com a correção, ABBA 1/2/2/1 deu o mesmo (57-58 parado, 41-50 girando). Mantido 1.
- 25/10 noite (após a correção da regressão), modo normal do jogador (Fifo, vblank 60), PC com
  qBittorrent/Wallpaper Engine abertos: parado 50-56 FPS, girando 37-45, picos 47-63 ms.
  Testado sem ganho no giro: Mailbox, GOW3_FRAMES_AHEAD=2, GOW3_GC_BUDGET_MB=5000,
  GZ_VBLANK_HZ=0/GZ_FPS_LIMIT=0, KYOEI_FRAME_CAP=0, GZ_GPU_LOG=warning e todas juntas.
  Medições com GOW3_FRAME_STATS=1 (bench-exe.sh) também não servem para FPS absoluto.
  Perfil da DrawRec: ~82% esperando trabalho; o gargalo já não é a gravação. Próximo: perfil
  de todas as threads no giro (guest main, GpuCommandProcessor) com o PC livre.
- Amostragem de todas as threads só nos quadros lentos (KYOEI_SAMPLE_THREADS=*,
  KYOEI_SAMPLE_SLOW_MS=20, run samp-all2): guest main ~49% em espera de condição, threads do
  jogo ~68%, GpuCommandProcessor ~42% esperando comandos + 20% em DrainDrawPipe, DrawRec e
  VkRecorder ~60% parados. Ninguém satura: o quadro é uma corrente serial (jogo -> tradução de
  comandos ~12-13 ms -> gravação ~4 ms -> GPU/apresentação ~11 ms; submit_to_flip ~27 ms) e
  o jogo espera o fim dela antes do próximo quadro. Ganho real no giro = encurtar a corrente,
  sobretudo ProcessGraphics/PostDraw/Draw na thread de comandos (58% dela) e a espera do fim
  da submissão. Trabalho longo; nenhuma opção de apresentação resolve.

### 26/10/2026 — Codex: lista de blocos descartada; medição de trabalho

- Ramo `desempenho-codex`, referência `3d4c516`, executável SHA256
  `e4d6b5df075d394b93e42524315a2435e0363816c5973c641e499407e1dd43a4`.
  Xeon E5-2640 v3 (8 núcleos/16 threads); Davy fechou Wallpaper Engine e qBittorrent.
  Rota original em 1920x1080/Fifo, mudo, medidor leve, sem `GOW3_FRAME_STATS`.
- Candidata: `RegDirty::Mark` mantém uma lista sem repetições dos blocos alterados;
  `PostDraw` e `RegDelta::Capture` leem essa lista. Os blocos não se sobrepõem, portanto
  a ordem da cópia não muda os registros. Teste compara 2.000 replays com o arquivo
  completo de registradores; 31 executáveis C/C++ passam (33 invocações com fixtures/modos).
- ABBA curto, **três últimas linhas de estatísticas do log**, médias harmônicas:

  | Rota | Versão | Giro (FPS) | Máximo nessas janelas (ms) |
  | --- | --- | ---: | ---: |
  | `codex-sparse-abba-a1` | Referência | 40,68 | 50,24 |
  | `codex-sparse-abba-b1r` | Candidata | 43,11 | 51,00 |
  | `codex-sparse-abba-b2` | Candidata | 37,37 | 50,43 |
  | `codex-sparse-abba-a2` | Referência | 38,10 | 50,39 |

  Agregado: 39,34 → 40,03 FPS (+1,75%), menor que a variação entre rodadas.
  Usando apenas as três últimas janelas **dentro** dos bytes marcados pela rota,
  o agregado fica praticamente igual (40,83 → 40,71). **Sem ganho de FPS comprovado.**
  Nenhuma compilação de shader dentro desses intervalos. Duas rodadas preliminares
  foram descartadas: cache frio e leitura PowerShell da telemetria presa consumindo CPU.
- Verificação separada antiga/nova com `GOW3_PIPE_VERIFY=16`, `codex-reference-verify`
  e `codex-sparse-verify`: encerramento 0, sem mensagens de diferença nos registradores
  ou de tabelas alteradas. Capturas com R reposicionando a câmera não mostraram defeito
  novo. O desenho azul perto do save também aparece na referência: ângulo/animação,
  não regressão demonstrada. FPS dessas rodadas diagnósticas não conta.
- A inclusão de um cabeçalho adaptado novo deixou objetos antigos com o layout anterior
  (`vk_draw_prep`/`vk_pipeline_cache` de 22/10), causando falha em SearchBinaryInfo antes
  da abertura dos arquivos. A integração agora invalida os objetos quando o conjunto dos
  cabeçalhos adaptados muda. Núcleo inteiro recompilado e partida conferida em seguida.
- ABBA prolongado: clone privado da mesma rota, mesmo save/aquecimento,
  cerca de 62 s girando em vez de 10 s. Usar janelas completas após o início do giro,
  excluindo a primeira janela que mistura câmera parada e giro. Evidência privada em
  `build/research/desempenho-codex-20261026/`. Meta de 60 FPS continua pendente.
- Resultado longo (janelas completas do giro): antiga 38,55; nova 34,17; nova 31,96;
  antiga 31,71 FPS. Agregados por tempo/intervalos: 35,09 → 33,03 FPS. A referência
  também caiu muito entre as pontas, portanto não atribuir toda a diferença à candidata.
  **Lista descartada por não demonstrar benefício.** Patch/teste guardados no checkpoint
  privado `checkpoint-20261026-024142.zip`; fontes e executáveis de teste obsoletos removidos.
- Sondagem do modo já existente `GOW3_DRAW_PIPE=0`, com a referência original:
  `codex-nopipe-probe`, 34,45 FPS nas três últimas linhas, sem compilação durante a coleta.
  Não é ABBA nem prova de ganho; não adotado.
- Mantida a correção de compilação: a lista de cabeçalhos adaptados muda o comando do
  compilador, e um manifesto elimina cabeçalhos gerados cuja adaptação saiu. Bloco real
  testado em fixture: remove só o obsoleto, conserva o atual, reconfigura sem alteração e
  recusa caminho fora de `adapted/` antes de apagar. Snapshot fixo intacto.
- Medidor leve agora acrescenta `draws_per_frame` lendo o contador existente a cada
  120 flips; não adiciona operações por desenho. `codex-unchanged-abba-a1` (referência
  com a métrica): 39,86 FPS no giro; ~1.390 desenhos/quadro parado e 1.917–2.173 nas três
  últimas janelas do giro. GPU observada em P0/1.860–1.890 MHz. Compilação e os **28 alvos
  atuais** de testes C/C++ passam (30 chamadas com modos/fixtures). As execuções anteriores
  de 31 testes incluíam dois binários de alvos já removidos na limpeza de 26/10; apagados.
- Em 02:55 foi identificado `gzport.exe` do Godzilla usando ~145% de um núcleo. Essa
  instância começou em 02:54:51, depois da última coleta Kyoei, e não explica por si as
  variações anteriores. Próximas medições suspensas aguardando Davy pausar os testes dele;
  revisão/compilação do Kyoei continuam. Não encerrar nem alterar trabalho do outro projeto.
