# Desempenho — objetivo de 60 FPS

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
