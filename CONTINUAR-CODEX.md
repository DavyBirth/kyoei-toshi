# Retomada atual — mapa do quadro e investigação no backend (15/10 à noite)

## Continuação mais recente: tempo, tarefas e entidades

- Davy pediu aprofundar a engenharia reversa de tudo que precisarmos para
  otimizar e corrigir o jogo. Continua backend, sem abrir partida agora.
- Ler docs/re/TEMPO-TAREFAS-ENTIDADES.md. Correção importante da hipótese:
  0x6dfda0, estágio 44, escreve app+0xe8=0 em 0x6e08a6. O cálculo medido
  substitui o padrão fixo do construtor no caminho de inicialização identificado.
- Índice privado: build/research/guest-code.sqlite, 42.194 intervalos,
  2.929.970 instruções, cobertura contígua e integridade PASS. Não versionar.
  Ferramenta tools/index_guest_code.py; reutilizar banco atual se hash confere.
- Rastreamento conservador de ponteiros: tools/trace_guest_fields.py,
  docs/re/field-trace-facts.json; relatório build/research/field-provenance.json.
  1.692 funções selecionadas, 1.334 acessos/endereços, 126 leituras app+0xf0
  em 92 funções. Não é análise semântica completa nem prova de todas as escritas.
- setDeltaTimeFactor → 0x603ff0 → app+0xec; grupo e entidade escalam o passo
  antes de gravar entidade+0x14 (segundos) e +0x18 (unidades de quadro).
- Gestor de unidades 0x1336200/tabela0x125a2f8; tarefas de 48 bytes, método
  ímpar codifica slot virtual+1. Unidade HUD Cockpit: tabela0x12d7bb8,
  slots78/80/90 mapeados. Caminhada dos filhos copia os campos de tempo.
- Consumidor 0x1f6020 é GxUnitRealTimeDemo, não física do jogador. Usa
  subdivisão de passos. Evitar generalizar uma atualização por apresentação.
- Receita docs/re/system-map-facts.json: 138 funções, 116 ligações de
  chamadores, nove slots condicionais/sete leituras conferidos, sem limite/decode
  perdido. 32 testes estáticos PASS. Nenhum jogo executado ou runtime alterado.
- Próximos alvos: classes de personagem/física e módulos de fase, preenchimento
  e drenagem das tarefas, widgets concretos do marcador/minimapa. Não aplicar
  mudança de relógio/threads por palpite. Medição de gameplay fica para depois.

## Mapa inicial preservado

Davy autorizou engenharia reversa orientada ao ciclo do quadro, desempenho e
HUD. Pediu seguir no backend, sem testar/abrir o jogo agora. Linux nativo abaixo
continua pausado; não retomar seus testes automaticamente.

- Ler docs/re/QUADRO-HUD-TEMPO.md e docs/re/frame-flow-facts.json.
- Ferramenta: tools/inspect_frame_flow.py. Usa limites unwind/FDE e valida
  chamadas com LLVM; 42.194 funções indexadas, 75 analisadas, 90 ligações a
  chamadores, 254 chamadas indiretas. Relatório privado build/frame-flow-map.json.
- Laço 0x345140 → slot +0x60 → quadro Kyoei 0x6e5940 → motor 0x345170.
  Encontrados cálculos de passo fixo/medido, esperas de label/evento e caminhos
  distintos de marcador HUD e medição de linhas. Sem concluir modo ativo na cidade.
- Hipótese de slow motion agora tem base estática: app+0xe8 seleciona cálculo;
  +0xf0 é usado por atualizações. NÃO alterar tempo por palpite. Falta observar
  modo/campos/cadência em cena fixa antes de qualquer patch de sincronização.
- Doze testes sintéticos do analisador e quatro de referências ELF PASS.
  Nenhum arquivo de jogo, desmontagem ou texto original adicionado ao Git.
- Backend Windows anterior: bb8a84d, buffers de diagnóstico dispensáveis
  evitados e métricas novas; build/testes PASS, sem ganho de FPS medido.
  Portable continua anterior. Diário atualizado com ambas as etapas.

--- Histórico Linux preservado abaixo ---

# PAUSA IMEDIATA - Linux nativo em execucao (15/10/2026)

Davy pediu interromper e salvar por limite de uso. Trabalho salvo; nao
continuar testes automaticamente ate novo pedido. Ver ultima entrada DIARIO.

Novo executavel: build/linux-native/kyoei-linux (ELF, compilacao PASS).
Referencia POSIX integrada, nao apenas biblioteca GPU. libc + nove modulos
inicializam; entrada real executa, memoria e threads funcionam. SDL/Vulkan
inicializam com 1726 simbolos HLE. Ainda sem primeiro frame ou jogo jogavel.
Teste GPU anterior chegou a sceErrorDialogInitialize; implementado depois.
Ultimo teste interrompido por pedido do usuario durante inicializacao.

Build (Ubuntu WSL):
cmake -S /mnt/g/kyoei-toshi -B /mnt/g/kyoei-toshi/build/linux-native -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DKYOEI_BUILD_LINUX_NATIVE=ON -DKYOEI_LINUX_PREPARED=/mnt/g/kyoei-toshi-work/native-05/out
cmake --build /mnt/g/kyoei-toshi/build/linux-native --target kyoei-linux --parallel 6

Execucao a partir de /mnt/g/kyoei-toshi:
build/linux-native/kyoei-linux /mnt/g/kyoei-toshi-work/native-05/out/boot-linked.bin --app0 /mnt/g/kyoei-toshi/build/City-Shrouded-in-Shadow-Portable/game --user /mnt/g/kyoei-toshi/build/linux-native-test-user --timeout 45
Acrescentar --cpu-only para isolar runtime (para em VideoOutOpen).
Logs: build/linux-full/native-{configure,build,cpu-run,gpu-run}.log.
Generator: tools/prepare_linux_native.py. Fontes adaptadas em native-src.
Biblioteca: build/linux-full/gpu/libbbgpu.so. Snapshot e Godzilla intocados.
Novos contratos ainda sem suite propria; implementar testes antes de anunciar.
Traducao, bindings Kyoei, dialogos completos, saves e empacotamento pendentes.
Nao publicar binarios experimentais: caminhos de desenvolvimento ainda existem.
Friends-Test Windows e saves pessoais preservados.

--- Notas anteriores (estado abaixo pode estar superado) ---

# Retomada para o Codex — 15/10/2026 (noite)

Davy pausou o Claude Code. Ler docs/DIARIO.md (últimas entradas) e docs/LINUX.md.

## Estado
- Revisão Windows mais recente: ler docs/REVISAO-HUD-20261015.md e últimas
  entradas do diário. Portable atualizado; exportação limpa para amigos em
  build/City-Shrouded-in-Shadow-Friends-Test (só compartilhar se manifest
  privacy_scan_passed=true; final PASS, 129 arquivos). Windows: e52aa75.
  Linux: build/linux-full/gpu/libbbgpu.so compilada, ELF x86-64.
  Procedimento: scripts/build_linux_gpu_probe.sh. Ainda sem jogo nativo;
  biblioteca de referencia nao integrada ao runtime Kyoei.
- Não reintroduzir escrita no campo overflow +0xc do medidor: travou loading.
  O rollback voltou a carregar. Quebra universal e legenda inferior/bússola
  seguem pendentes. Alguns ícones de fonte não reconhecidos; Xbox não pronto.
- Retomada Codex: RTC e semáforos/eventos POSIX implementados em cópias de
  build; quatro testes Linux PASS. Auditoria real: 10 imports no runtime,
  418 nos módulos, 738 stubs. Ainda sem execução do jogo/GPU Linux.
- Portable Windows atualizado em build/City-Shrouded-in-Shadow-Portable (en-16,
  deltas de ícones EN/JA, arte do Return to Desktop, "Start on Linux.sh" via Wine).
- Base anterior: 5f3ada9. Saves de Davy preservados. O estado mais recente
  desta lista prevalece sobre as notas históricas abaixo.

## Pendências
1. Port NATIVO Linux (pedido explícito de Davy). Carregador ELF + TLS prontos
   (src/linux, tools/prepare_linux_bootstrap.py). Falta runtime POSIX.
   Uso de Win32 por arquivo (contagem): rt_services 31, loader 27, rt_sync 26,
   rt_vmem 21, rt_file 21, rt_sema 20, rt_thread 18, dialog_ui 11, rt_memory 9,
   rt_kernel 8, rt_savedata 7; gpu/ quase portável (bridge/self_test 2 cada).
   Referência POSIX: build/reference-bloodborne/src (runtime_memory.c mmap/memfd,
   signals, pthreads). Build gráfico Linux falhou por cabeçalhos Vulkan do sistema
   (usar Vulkan-Headers do submódulo).
   Wine no WSL (llvmpipe) roda código do jogo e para antes de VideoOut; sem GPU
   real no WSL (dzn ausente), não comprovado.
2. Histórico anterior à revisão: L1/R1 do menu de pausa → setas/A-D no PC.
   Implementado agora por contexto das texturas; ver revisão HUD. O antigo
   hook de UnitMenuManager 0xb2c600
   (vtable 0x12e56c0, slot 15) testado; campo +0x178 não muda ao abrir o menu.
   Revertido. Falta achar o sinal de "menu aberto".
3. Verificar em jogo: quebra por palavras (hook 0x2e2b10), ícones novos,
   ↑ no Return to Desktop. Pedir a Davy uma cena com palavra cortada.


Continuação RE personagem (15/10/2026): ver docs/re/PERSONAGEM-FISICA.md.
Ferramenta tools/inspect_guest_types.py, relatório privado
build/research/movement-types.json (59 tipos/1.039 registros conservadores).
Próximo: identificar self+0x6a0 em 0x719a90, resolver movimento e passo físico.
UnitPlayer slot78=0x9fc1a0 segue base condicionalmente ao bit0 de self+69a.
Não confundir UnitPlayerBase (criação nula) com pai registrado UnitCharacter.
Continuar backend sem abrir jogo; desempenho real ainda precisa medição em cena.


Próxima medição (15/10, após teste real): build/kyoei.exe já inclui categorias
command-callback, video-label-wait, raster-draw, raster-dispatch e cp-sync.
Mesmo Medir desempenho atual EN.cmd; agente não iniciou partida. Aguardar
teste do começo pelo Davy; analisar work_stalls com end_ms no relatório.
Até8 eventos>=50ms por categoria/thread; scopes CPU inclusivos, não somar
com command-batch nem chamar de tempo puro GPU. Build/testes PASS.

Segundo teste231142 analisado: ver fim de docs/DESEMPENHO.md. Coleta por draw
foi excessiva (54.419 linhas). Corrigida para1/128, build/testes PASS, sem
partida do agente. Sessão29,25FPS não é comparação A/B limpa. Readback-finish
atingiu241,97ms, labelwait30,57ms; investigar leitura sincronizada GPU.


Update amigos preparado: build/Kyoei-Windows-Update-20261015.zip (7,3MB),
só Windows x64/runtime próprio; docs/ATUALIZACOES-AMIGOS.md explica aplicação
e backup. Baseline Friends-Test original intacta. Não enviado externamente.
Linux requer pacote próprio ainda não preparado. Readback: adicionada correlação
bytes/dimensões/tempo para esperas>50ms; build/testes PASS, fence preservado.
Não afirmar ganho de FPS. Código de atualização tem3 testes sintéticos PASS.


CORREÇÃO Linux: a distribuição Friends-Test usa Wine/Proton (Start on Linux.sh),
portanto compartilha o PE Windows. Update pronto:
build/Kyoei-Linux-Wine-Update-20261015.zip,7,3MB,aplicador Python>=3.11.
Extrair na raiz: sh Atualizacao-Kyoei/Aplicar-no-Linux.sh. Quatro testes WSL
PASS e preflight real PASS; backup/saves/prefixo preservados. Não é atualização
do port ELF nativo. Ver docs/ATUALIZACOES-AMIGOS.md. Não enviado aos amigos.

Coleta local mais recente: Scheduler::Wait>=50ms agora inclui contadores
requested/next/known_before/known_after/forced_flush (máximo8/thread), sem nova
consulta Vulkan. Aguardando novo teste do começo: logs antigos não contêm isso.
Build/testes PASS. ZIPs amigos permanecem na revisão anterior identificada.

Teste233924:32,91FPS globais,sem esperaScheduler/readback>=50ms. Não culpar
readback sozinho. Candidato real implementado: PopPendingOperations retorna
cedo com fila vazia,poupando consulta Vulkan por Draw/Dispatch. Fences/filas
ativas intactas. KYOEI_POLL_EMPTY_OPS=1 restaura caminho antigo para A/B.
Build/testes PASS; aguardando validação gameplay no começo pelo Davy.
ZIPs antigos preservados; só atualizar amigos após validar candidato.

Teste234620 analisado:ganho do earlyreturn ainda não comprovado. Agora
confirmado readback1x1/256bytes correlacionado a esperas91/133ms,sem flush
extra e tick já submetido. Próximo: produtor/consumidor dessa imagem,
sem desativar sincronização. Não pedir repetição idêntica sem hipótese nova.
ZIPs amigos ainda anteriores ao candidato de polling vazio.
