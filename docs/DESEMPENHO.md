# Desempenho — objetivo de 60 FPS

## Verificações de 19/10/2026

Quatro rodadas ABBA com superfície interna1920x1080 deram36,82FPS com saída
1080p e36,70FPS com saída720p: diminuir só a janela não trouxe ganho útil
nesta vista parada. [Método e limites](re/SAIDA-720P-1080P.md). Não confundir
com escala interna ou eliminação de travadas em movimento.

As consultas de orçamentoVulkan também não explicaram picos grandes: nos
grupos reportados de5.760chamadas por cache, texturas somaram60,66ms e buffers
16,33ms; máximos0,2305/0,1157ms. A política de cache não foi alterada.

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
