# Diário do projeto

Memória do projeto entre sessões. Ler antes de começar cada sessão.

---

## 08/10/2026 — fechamento técnico do M0

- O diário estava vazio; havia inventário e importações ainda sem commit.
- Fonte local: `G:\GodzillaShadPS4PC\launcher\game_data\CUSA03562`.
- Identificação lida diretamente: CUSA03562, APP_VER 01.03, VERSION 01.00.
- Validados novamente os 87 arquivos (17.798.062.971 bytes), incluindo 25
  PSARC; todos os SHA-256 coincidiram com o inventário existente.
- SFO, SHA256SUMS e hashes das ferramentas também conferidos.
- Ferramenta oficial `G:\godzilla-port-codex\tools\imports.py` executada
  novamente: JSON idêntico, 704 importações, 61 módulos declarados,
  63 bibliotecas declaradas e 10 NIDs sem nome conhecido.
- Adicionados `tools/verify_m0.py`, `tools/report_m0.py` e resumo em
  `docs/m0/README.md`; relatório legível regenerado em UTF-8.
- Nenhum arquivo do dump alterado ou incluído no Git. A regra de usar
  exclusivamente o dump próprio do Davy permanece; hashes não certificam origem.
- M0 técnico validado. Próximo marco: M1, leitura e extração dos PSARC
  fora do repositório, com catálogo dos formatos encontrados.
- Não iniciado M6 e não copiado código compartilhado do Godzilla.

## 08/10/2026 — M1, extração dos PSARC

- Implementado leitor independente da variante PSARC encontrada nos 25
  arquivos: PSAR 1.4, zlib, blocos de 64 KiB, TOC em claro, flags 0x0D.
- Manifesto usa separadores nulos. A ordem do manifesto difere da TOC;
  ligação comprovada por MD5 dos caminhos em maiúsculas ASCII.
- Extraídos 72.961 arquivos (19.327.950.739 bytes) para
  `G:\kyoei-toshi-work\m1`, fora do repositório e do dump.
- Antes da extração, os SHA-256 dos 25 PSARC foram comparados com o M0.
- Releitura de todas as 72.961 cópias concluída com sucesso: tamanhos e
  SHA-256 idênticos aos calculados durante a decodificação dos PSARC.
  Os 25 pacotes terminaram com `VERIFICADO`; M1 concluído tecnicamente.
- Nove testes sintéticos passaram: blocos brutos e zlib, nomes fora de
  ordem, arquivos vazios, caminhos inválidos, colisões no Windows,
  truncamento, hashes de nomes, offsets de 40 bits, índices inválidos,
  corrupção zlib e rejeição de alterações no catálogo/extração de prova.
- Candidatos a textos: `lua/*.luc` e tabelas `csv/*.csh`, incluindo itens,
  dicas de carregamento, materiais de leitura e textos de objetivos/notícias.
  Os contêineres internos ainda precisam ser documentados no M2.
- Fontes candidatas em `font/utf8all*`, tabelas `.csh` e atlas `.tex`;
  cobertura de acentos ainda não comprovada (M3).
- Encontrados bancos de som com assinaturas @UTF/AFS2, três vídeos USM
  com CRID e um MP4; modelos, animações, colisões, interface e shaders.
- Ferramentas: `psarc.py`, `catalog_m1.py`, `report_m1.py`, `verify_m1.py`
  e `test_psarc.py`. Documentação em `docs/m1`; catálogo detalhado local
  em `work/m1/catalog.json`, ignorado pelo Git. Seu SHA-256 está registrado
  em `docs/m1/formats.json`, junto com contagens e assinaturas por extensão.
- Listagem sem extração e sintaxe das ferramentas finais também conferidas.
  A ferramenta de releitura agora usa buffer menor e mostra progresso;
  `verify_m1.py` permite repetir a prova sem uma nova extração.
- Nenhum texto completo ou dado do jogo adicionado ao repositório.
  Não feita reinserção, tradução, alteração de fonte ou trabalho no M6.
- Próximo marco: M2, documentar contêineres `.luc`/`.csh`, extrair e
  reinserir textos, começando com a prova sem alterações no shadPS4.

## 08/10/2026 — M2, ferramentas e prova de identidade

- Documentados envelopes BILZ, bytecode Lua 5.2 variante float32 e tabelas
  CSH. Corrigida a interpretação da tag CSH: três bits de tipo, 29 bits
  de comprimento. Dois valores opacos de oito bytes em item_data são
  preservados; o significado deles ainda é desconhecido.
- Implementadas extração e reinserção por IDs estáveis, com SHA-256 da base,
  verificação dos originais, preservação de dados não textuais e rejeição
  de IDs inválidos. Textos originais exportados ficam somente fora do Git.
- Auditoria completa: 2.032 CSH e 299 LUC, total 2.331 recursos; 235.069
  ocorrências de strings e 17.866 candidatas com caracteres japoneses.
  Zero falhas; todos idênticos após ida e volta estrutural e via JSON.
  Essas contagens incluem identificadores e listas; não são falas únicas.
- Seis testes sintéticos de texto e quatro de atualização PSARC passaram,
  incluindo acentos, crescimento, redução, aliases, dados opacos, bytecode,
  hashes, corrupção, blocos zlib/brutos e preservação de recursos vizinhos.
- Atualizador PSARC preserva o corpo original e anexa recursos alterados,
  ajustando a TOC. Sem alterações, mantém o pacote inteiro byte a byte.
- Construída cópia completa em
  `G:\kyoei-toshi-work\m2\prova-01\CUSA03562` após extrair e reinserir
  os 2.331 recursos. Os 87 arquivos (17.798.062.971 bytes) coincidem em
  tamanho e SHA-256 com o M0. Relatório em `docs/m2/dump-proof.json`.
- Tentativa isolada com shadPS4 instalado, usando diretório local separado
  `G:\kyoei-toshi-work\m2\shad-prova-01`; não foram copiados saves.
  O processo 37520 ficou num diálogo antes de iniciar o jogo, com log vazio.
  Backtrace confirmou espera em DialogBox; o processo depois encerrou sem
  uma janela de jogo confirmada nessa tentativa.
- Segunda tentativa com janela visível: shadPS4 0.16.0 reconheceu CUSA03562
  01.03 e executou a cópia até a cena inicial. Captura inspecionada mostra
  cenário renderizado e narração japonesa legível. Hashes do executável e
  da captura registrados em `docs/m2/runtime-proof.json`; imagem só local.
  Log contém erros sceKernelStat (arquivo não encontrado); não afirmar
  execução sem erros nem regressão completa. Não comparada execução da base.
- M2 NÃO concluído: execução inicial da cópia foi observada, mas falta
  confirmação da cobertura de todos os textos visíveis. Nada de tradução definitiva,
  alteração de fontes ou M6. Nenhum dado do jogo adicionado ao Git.
- Ferramentas e limites documentados em `docs/m2/README.md`.

## 08/10/2026 — reaproveitamento nativo do Godzilla

- Davy pediu priorizar a base do Godzilla seguindo Bloodborne/P.T.
  Conferido diário oficial: GPU funcionando, missões/ondas, controle e
  saves aprovados; condição M5 para iniciar M6 do Kyoei atendida.
- Primeiro snapshot recusou mudanças não salvas da sessão do Godzilla.
  Depois do commit oficial, fixado `e976b59a3f056c5ea5d0ddfe1c5ef566cce2f45e`
  em `third_party/godzilla-core`, ignorado pelo Git. Fonte oficial não editada.
- Referências fixas: Bloodborne `daa71654083b42ea0730b3f0fecee2fc20942d02`
  e shadPS4 `3d03375a5bbfa52772a2142e151d6b73af48dc8d`. Hashes em
  `docs/godzilla-core.json`; licença GPL-2.0 incluída na raiz.
- Reaproveitadas bibliotecas gráficas já compiladas, copiadas com hashes.
  Compilados aqui carregador/runtime e adaptador de identidade CUSA03562.
  Nenhum fonte da cópia fixa alterado.
- Preparador usa código fixo com política Kyoei: SFO 01.03, hashes M0 dos
  executáveis usados e hash da imagem. Não ignorada checagem do jogo.
- Primeira preparação parou ao imprimir título japonês com cp1252,
  deixando native-01/out parcial. Adaptador corrigido para UTF-8;
  preparação completa em `G:\kyoei-toshi-work\native-02\out`.
- Eboot: imagem 22.408.592 bytes, 704 importações e 82.840 relocações.
  Com módulos ligados, tabela tem 939 importações e 335 ligações nativas;
  isso não comprova que as demais estejam todas resolvidas pelo runtime.
- `build/kyoei.exe` compilado e executado sem iniciar shadPS4.exe.
  App0 explícito do Kyoei; usuário isolado em native-02/user.
  Saída 1 antes da janela: carregador rejeita TLS estático do eboot.
- TLS medido: vaddr 19.906.656, filesz 0, memsz 40, alinhamento 8.
  Bloodborne já possui suporte ao TLS principal; adaptação Windows falta.
  Não removida a rejeição para executar com TLS inválido.
- Conferidos 1.834 arquivos da base fixa, todos com hashes intactos.
  Provas em `docs/native-prepare.json` e `docs/native-proof.json`;
  integração e próxima correção em `docs/PORT-NATIVO.md`.
- M6 iniciado, ainda SEM janela/jogo nativo. A correção compartilhada deve
  entrar primeiro na sessão do Godzilla, depois renovar a cópia daqui.
  P.T. é referência futura, sem código incorporado nesta etapa.

## 08/10/2026 — resultados M2 preservados antes da mudança de foco

- Índice local text-index-01.jsonl: 17.866 ocorrências japonesas UTF-8,
  13.712 strings únicas em 140 recursos. Metadados em docs/m2/text-map.json;
  texto completo só fora do Git. Categorias por nomes ainda são indicativas.
- Há 4.447 strings não UTF-8, principalmente flags/configurações; amostras
  aparentam codificação japonesa antiga, ainda não certificada. Preservadas.
- sceKernelStat também apresentou erros com o dump original. Rastreio
  mostrou consultas masterPS4/finalizedPS4 e DAT/module.psarc. Não criados
  arquivos falsos nem alterado o dump.
- Cópia no emulador terminou com Out of order flip IRQ depois da abertura;
  FIFO também reproduziu a falha. Original foi fechado normalmente após
  cerca de seis minutos; não afirmar reprodução da falha gráfica nele.
  Log separado original-baseline.log preservado fora do repositório.
- A tentativa de versão alternativa foi interrompida pelo pedido de mudar
  o foco. Conferência posterior não encontrou shadPS4 aberto. M2 continua
  pendente; não foram aprovados M3/M4.

## 08/10/2026 — autorização de desenvolvimento independente e TLS local

- Davy corrigiu explicitamente a regra anterior: o Godzilla é a base de
  partida, NÃO uma dependência de aprovação ou de receber correções antes
  do Kyoei. As melhorias necessárias são feitas e testadas aqui. Esta
  orientação substitui a exigência anterior de mudar primeiro o Godzilla.
- Snapshot oficial continua intacto. CMake gera uma adaptação local do
  loader em build, com validação TLS, configuração do template e handler
  de FS; implementação própria em src/kyoei_tls.c/h. Nenhum fonte oficial
  ou da cópia fixa foi alterado.
- TLS variante II segue o desenho do Bloodborne, adaptado para FLS Windows:
  template, parte zerada, alinhamento e bloco independente por thread;
  destruição automática na saída. Módulo 1 atendido por __tls_get_addr.
  A instrução medida mov r64, fs:[0] é tratada somente em código guest;
  GS/TEB do Windows permanecem intactos. Outros acessos não suportados
  continuam falhando explicitamente, sem valores inventados.
- Teste Windows real PASS: template de 8 bytes, total 40 bytes, padding
  zero, alinhamento e duas threads simultâneas com valores independentes;
  execução de instrução FS real e retorno do TCB. Binário test_kyoei_tls.
- Primeiro boot após TLS revelou três nomes vazios no aerolib. Adaptador
  do preparador normaliza só o símbolo textual como ?, preservando NID,
  biblioteca e posição. Reparadas as posições 95, 105 e 107 da saída local.
- Implementado sceLibcHeapGetTraceInfo com layout de 32 bytes e tabelas
  persistentes de mask/64 estados segundo kernel.cpp da referência.
  Implementado sceKernelIsNeoMode como PS4 padrão (0).
- Boot real: janela SDL 1280x720, Vulkan na RTX 2060, módulos libc/Fios2,
  fontes e áudio inicializados; entrou no eboot+0x80. 357 importações pelo
  runtime, 335 nativas e 247 ainda sem implementação nesse estágio.
- Após consulta do modelo e carga dos módulos de sistema, próximo bloqueio
  sceKernelLoadStartModule para /app0/module/prx/libSceSecure.prx, presente
  no M0 (119.054 bytes, SHA256 50196e67ca822d77d5fa5c47651ef08f25af4bdd5a232d107085ac1a777d8251).
  Não simulado sucesso: ainda falta suporte à carga desse módulo.
- TLS inicial superado; cena do jogo ainda não renderizada e M6 não concluído.
  Logs externos native-02/logs/20261008_203518.log, 203728.log e 203811.log.
  Não aguardar mudanças do Godzilla para continuar este projeto.

## 09/10/2026 — primeira imagem do jogo no port nativo

- Pedido de Davy: abrir o jogo na tela dele. Implementações próprias aqui,
  preservando Godzilla e snapshot; nenhum executável shadPS4 foi lançado.
- Preparação native-04 inclui libc/Fios2/NpToolkit2 do dump com hashes M0,
  firmware externo já existente e Secure ligado com inicialização adiada.
  `deferred.tsv` guarda endereços relativos e NIDs externamente. LoadStartModule
  chamou o inicializador real do Secure (retorno 0); resolução Dlsym por
  SHA1/salt/base64 PS4, handles próprios e rejeição de módulos desconhecidos.
- Adaptadas consultas de PlayGo (dump totalmente instalado), DiscMap local,
  carga interna de módulos, modo padrão/sem ASAN, usuários e serviços offline.
  Corrigidas diferenças de nomes no VideoOut e na configuração de screenshot.
  NpToolkit2 iniciou suas threads; callbacks de usuário/rede não anunciam
  transições de serviços online inexistentes. Troféus sem banco registrado
  retornam erro NOT_REGISTERED; gravação de vídeo não está disponível.
- Consulta de armazenamento usa espaço real via GetDiskFreeSpaceExW.
  Corrigida a interpretação de gz_widen (0 significa sucesso).
- Primeira abertura: `native-04/logs/20261009_011201.log`. Captura externa
  `native-04/window-01.png`: logo de Kyoei sobre cidade, conferido visualmente.
  DualSense reconhecido, botões físicos entregues ao jogo. Depois houve parada
  explícita na consulta de DLCs: 815 leituras / 104.056.609 bytes até essa falha.
- Implementado GetAddcontInfoList como lista vazia para a sessão sem DLCs;
  reaberto em `20261009_011550.log`. Captura `native-04/window-02.png` mostra
  prédios e a silhueta de monstro na abertura, confirmando renderização real.
  Janela trazida para frente e renomeada Kyoei Toshi - Port para PC.
- Compilação concluída; teste TLS real PASS novamente; diff --check sem erro.
  `native-current.json` registra hashes do executável e captura externa,
  e o hash do log no momento da consulta (o processo ainda escreve nele).
- Pedido de abertura alcançado. Não declarar port completo, missão jogável,
  tradução pronta ou salvamentos validados. M6 continua em desenvolvimento;
  deixar o jogo aberto para Davy testar.

## 09/10/2026 — varredura e limpeza de armazenamento

- Davy pediu limpeza antes de avançar no port. Medidas iniciais: projeto
  454.478.418 bytes; pasta de trabalho 37.980.574.605 bytes (total 38,44 GB).
  Os maiores responsáveis eram o dump duplicado da prova M2 (17,80 GB) e
  a extração completa M1 (19,32 GB); não o código nem a compilação.
- Antes de remover derivados, verify_m0.py passou: 87 arquivos do dump
  original, todos os hashes iguais ao M0, CUSA03562 APP_VER 01.03.
- Plano detalhado externo em cleanup-20261009/plan.json; remoção em
  PowerShell com validação de caminho absoluto, raiz, reparse points,
  lista fixa de diretórios e extensões de mídia. Uma validação inicial
  de DirectoryInfo interrompeu antes de apagar arquivos; corrigida e
  operação completa registrada em completed.json.
- Removidos 57.060 recursos extraídos de mídia (texturas/modelos/pacotes,
  sons/vídeos), a cópia m2/prova-01/CUSA03562 e native-01/02/03/out.
  Liberação: 37.290.822.447 bytes. Nenhum dado original foi apagado.
- Mantidos 15.901 recursos pequenos da extração, incluindo todos os 2.331
  CSH/LUC; catálogos completos, índices, provas, logs, capturas e saves.
  Base fixa, build e native-04/out/user preservados; executável permanece
  com SHA256 d0aa33bba7c3cf12c87cee3816c7263af120e2efd5733130784b7aa7604b83e4.
- restore_m1.py recupera um recurso ou pacote ausente a partir do dump,
  valida catálogo/PSARC e hash do recurso, sem sobrescrever. Prova real:
  system.psarc:GxCore/texture/null-black.tex recuperado e conferido (165 bytes).
- audit_m2.py novamente PASS: 2.331 recursos idênticos, 17.866 strings
  japonesas, zero falhas. Extração M1 agora parcial; verificação completa
  de M1 requer reextrair recursos ausentes. Não alterar a prova histórica.
- Medição final: projeto 454.489.617 bytes; trabalho 707.036.026 bytes;
  total 1,16 GB (redução de aproximadamente 97%). Inventário adiciona alguns
  MB de metadados e o log pode crescer; números representam o momento medido.
- Jogo continuou aberto durante a limpeza e depois foi fechado pelo usuário:
  log window closed by user, saída 0; 3.134 leituras / 169.224.749 bytes.
  Não avançado em novos recursos de gameplay nesta etapa.

## 09/10/2026 — pausa solicitada por Davy; retomada pelo menu

- Davy autorizou avançar até a primeira missão e validar salvamentos.
  O port foi reaberto; somente o dump original e native-04 foram usados.
- Corrigido capture_native.ps1 para coordenadas físicas com consciência de
  DPI por thread. A escala do Windows cortava parte da captura anterior;
  play-04.png/play-05.png externos agora mostram a janela inteira.
- Criado input_native.ps1: valida PID, caminho do executável e janela;
  envia pressionar/soltar somente ao Kyoei via PostMessage, sem teclas globais.
  Testes reais de Enter, Z e Space chegaram ao scePadReadState como
  0x8, 0x8000 e 0x4000, seguidos de liberação 0. DualSense também responde.
- A abertura animada continua repetindo (cidade/monstro), porém não foi
  comprovado menu de jogo novo nem controle do personagem. Não declarar
  primeira missão, salvamentos ou jogabilidade validados.
- Próxima investigação: localizar o que segura a transição da abertura
  para o menu (threads/serviços ou desenho da interface). Comando LLDB de
  inspeção foi interrompido pelo pedido de encerrar; nenhuma pilha foi
  obtida. Conferência após interrupção não encontrou LLDB ativo.
- Davy pediu encerrar por hoje e retomar amanhã. Jogo fechado, ferramentas
  e diário salvos. Não alterar o runtime nem recompilar nesta pausa.
  Retomar de build/native-latest.console.log e das capturas externas
  G:/kyoei-toshi-work/native-04/play-01.png a play-05.png; manter extração
  M1 parcial e não recriar grandes cópias de dados.

## 09/10/2026 — primeira fase e personagem controlável; pausa de Davy

- Pedido: superar a abertura que parecia uma sequência de wallpapers e
  chegar ao gameplay. Usado somente o dump original CUSA03562 01.03.
- Corrigido o diálogo de mensagem NONE (tipo 2, sem botões): permanece
  aberto até o jogo fechar, sem fabricar uma escolha. Adicionada a função
  sceMsgDialogGetStatus, compartilhando o estado de UpdateStatus.
  Adaptações de services/dialog_ui são geradas em build pelo CMake;
  snapshot e projeto do Godzilla permanecem intactos.
- Preparação externa nova native-05 inclui o módulo real adiado
  module/MasterORBIS/stage201/StageModule.prx. Hash M0 conferido:
  134260577eb0b9ac10f484d25fa33f05772c3a59917806fbee24afc84e4ddb8a.
  Inicializador real retornou 0, handle 0x4001. native-04 preservado.
  docs/native-prepare.json agora aponta para native-05/out. O preparador
  atual inclui somente a primeira fase, não todas as fases do jogo.
- Marcadores RazorCpu PushMarkerStatic/PopMarker atendidos sem coletor,
  retorno 0 como na referência; nenhuma lógica de gameplay substituída.
- Implementado src/kyoei_ime.c: teclado de nomes nativo do Windows,
  UTF16, limites, confirmação/cancelamento/aborto, estados e resultado.
  Filtro real do jogo chamado na thread guest por GetResult. Teste real:
  sobrenome e nome com limite de quatro caracteres, filtro e opções 0;
  ambos confirmados, resultado 0, e a introdução continuou. Parâmetros
  estendidos, outros tipos e opções ainda não suportados são rejeitados.
- Prova real em native-05/logs/20261009_164159.log: carregamento chegou a
  100%, scripts registraram stage start e StageTitle_end 201. Capturas
  gameplay-15/17/18 mostram carga e primeira rua; gameplay-21 mostra
  personagem, HP 100/100 e minimapa. Controle físico gerou deslocamento
  no analógico e ação de esquiva. Primeira chegada ao gameplay comprovada;
  missão inteira e salvamentos/recarregamento ainda não validados.
- Mundo 3D fica desfocado durante gameplay; a ajuda de controle e o HUD
  aparecem. Não determinada a causa do desfoque. Menu inicial também
  apresentou opções invisíveis sobre o fundo, embora fosse navegável.
- Falha intermitente fatal da GPU: Out of order flip IRQ (saída 23),
  reproduzida na abertura e depois do início do gameplay. Experimento
  de um quadro pendente por vez, esperando count e liberação da superfície,
  passou no teste isolado mas NÃO resolveu o jogo: falhou novamente em
  20261009_164802.log. Experimento e teste removidos dos fontes finais.
- Diagnóstico local de VideoOut compila a unidade da cópia fixa, mantendo
  o assert. Depende apenas de headers do cache externo configurável
  KYOEI_GPU_HEADERS (padrão G:/godzilla-port-codex/build/_deps), lidos sem
  alterar o Godzilla. Demais unidades GPU continuam na biblioteca fixa.
  ABI STL dessa unidade configurada como na biblioteca (iterator level 0).
- Última prova 20261009_165400.log: buffer=1, label=0, previous=0,
  count=2115, pending=0, gc=0, arg=0 antes do assert. Próxima investigação:
  comparar registro dos callbacks de flip com a ordem dos pacotes NOP
  PatchedFlip e das escritas de etiquetas; verificar reuso/submissão dos
  command buffers. Isso é hipótese, não causa confirmada. Não remover o
  assert nem inventar valor 1 para a etiqueta.
- Build final compilou; teste TLS Windows PASS e prepare_native.py passou
  na compilação Python. Executável final com diagnóstico:
  SHA256 74dd4d4c2225cb27256ffa7317db078fda1543aa4c3e6e4fdcaafe505ad751e1.
  Capturas e provas externas preservadas em native-05/session-20261009;
  docs/native-gameplay.json registra os hashes, sem arquivos do jogo.
- Davy pediu salvar e interromper. Nenhum processo Kyoei, LLDB, Ninja
  ou Clang ativo na conferência final. Não executar novos testes ou
  recompilar após essa pausa; retomar dos logs/capturas acima quando
  solicitado. M6 continua em desenvolvimento: gameplay inicial funciona,
  mas o port ainda não é estável nem completo.

## 12/10/2026 — retomada e histórico de callbacks de apresentação

- Davy pediu continuar. Retomada pela falha intermitente Out of order flip
  IRQ, ainda sem causa confirmada. Snapshot e projeto Godzilla preservados.
- Adicionado src/kyoei_flip_trace.h, integrado somente à unidade VideoOut
  gerada em build. Guarda os últimos 64 registros/execuções de callbacks,
  com sequência, identificação do pedido, thread, tempo, buffer e etiqueta.
  Histórico impresso se a etiqueta falhar; assert e agendamento preservados.
  Registro ocorre antes da inserção na fila IRQ: em chamadas concorrentes,
  a sequência observada não prova sozinha a ordem de inserção da fila.
- Inspeção confirmou: GNM registra callback e modifica o command buffer
  para escrever etiqueta 1 antes do NOP PatchedFlip; Liverpool sinaliza
  GfxFlip nesse NOP e consome um callback da fila global. Próximo diagnóstico
  deve distinguir reordenação de pedidos, reuso de command buffers e
  alteração da etiqueta pela apresentação. Nenhuma dessas causas comprovada.
- Compilação passou, test_kyoei_tls.exe PASS, git diff --check sem erros.
  Executável SHA256:
  20f36412562d717abc6044c4ab4e9f1e7009f0cec7ade35fd35d3a61b8ed7364.
- Port aberto com scripts/run_native.ps1, PID 21776, registro externo
  native-05/logs/20261012_150241.log e console build/native-latest.console.log.
  Captura local ignorada build/resume-20261012-01.png mostra personagem na
  rua, HP 100/100 e menu de pausa com opções visíveis. Controle físico
  entregou botões e movimentos do analógico ao jogo durante a observação.
- A execução terminou com saída 30: após movimento pelo controle e eventos
  da fase, o jogo registrou Stall during rendering at flush, syncLabel=61096,
  expectedLabel=61104. Interrupção guest `cd 41` em eboot+0x4331ba, thread
  11140, reportada pelo Windows como access violation. Não houve mensagem
  Out of order flip IRQ nem descarga do novo histórico nesta execução.
  Não tratar essa interrupção deliberada após o stall como simples leitura
  de ponteiro inválido; causa da espera gráfica ainda desconhecida.
- Console registra 8.312 leituras / 1.236.744.684 bytes, zero escritas de
  arquivos. Desfoque, missão completa e salvar/recarregar continuam pendentes.
  Alteração desta sessão é diagnóstico, não correção comprovada.
- Terminal isolado falhou ao iniciar (setup refresh); comandos precisaram
  de execução aprovada fora do isolamento. Git usou safe.directory apenas
  por comando; nenhuma configuração global alterada. Política de scripts
  contornada somente no processo de teste com ExecutionPolicy Bypass.
- Davy pediu salvar para iniciar outro comando e resolver as permissões.
  Não abrir outra execução nesta pausa. Retomar do log acima: investigar
  stall de flush/syncLabel e chamada GNM imediatamente anterior ao `cd 41`,
  além da falha IRQ anterior. Última busca de fontes foi interrompida;
  nenhuma nova correção aplicada depois da parada do jogo.

## 12/10/2026 — corrida de liberação de superfície e novo teste

- Davy relatou que o fechamento no gameplay anterior ocorreu ao andar
  perto da luta de Ultraman e de explosões na cidade; mundo ainda desfocado.
  Comparou também o áudio caótico com vídeo do PS4 e confirmou comportamento
  semelhante na cena. Não alterar áudio só por sobreposição de vozes.
- Desmontagem local do dump confirmou loop em eboot+0x4330b0: após
  10.000.000 leituras sem atingir o valor esperado, imprime stall, chama
  sceGnmDebugHardwareStatus e executa int 0x41. Limite de iterações,
  não timeout em segundos. Compilação de efeitos pode atrasar a thread GPU,
  mas essa causa ainda não foi comprovada. Não pular a interrupção.
- Tentativa LLDB: primeiro faltou python3.dll; PATH do processo com Python
  3.14 existente permitiu conectar. Porém lldb-server falhou com C0000005
  durante o teste, perdeu a conexão; jogo saiu com -1073740972 (C0000374).
  Nenhuma pilha válida capturada. Prova build/render-stall-threads.log e
  log externo 20261012_151249.log. Ferramenta LLDB experimental removida.
- Adicionados kyoei_crash.c/h: MiniDumpWriteDump na chamada de diagnóstico
  de stall e no encerramento por fault. Encaminha diagnóstico GNM ao
  original; não substitui sua resposta. Integração no loader gerado e
  resolver só no executável principal (KYOEI_CRASH_CAPTURE), teste TLS
  separado. Dumps na pasta do executável, atualmente build, nunca distribuir;
  .dmp/.mdmp ignorados globalmente por poderem conter memória do jogo.
  Compilação passou; captura de dump em falha real ainda não comprovada.
- Execução 20261012_152005.log fechou com saída 23, na abertura/transição:
  Out of order flip IRQ. Histórico de 64 eventos mostrou registros e IRQs
  na mesma ordem, todos pela mesma thread registradora; último pedido 4531,
  buffer=0, etiqueta 1 no registro e 0 no IRQ, previous=1, count=4530,
  pending=0. Não houve fault/stall: caminho do minidump não foi chamado.
- Identificada janela de corrida no driver de referência: Flip publica
  status e dispara eventos antes de zerar etiqueta da superfície anterior.
  Consumidor acordado pode reusar essa superfície, escrever 1 para seu
  próximo quadro e ter essa escrita apagada pelo reset atrasado para 0.
- Adaptação local do driver.cpp gerada em build pelo CMake: liberação da
  superfície anterior e atualização de prev_index passam a ocorrer dentro
  do port_mutex, antes da publicação do status e dos eventos. Nenhuma escrita
  artificial de etiqueta 1; assert de ordem continua. Snapshot e Godzilla
  intactos; nova unidade usa os mesmos includes/ABI do VideoOut local.
- tools/test_videoout_order.py compila o método Flip real de cada fonte
  contra consumidor imediato do evento: referência perde a nova etiqueta
  (falha esperada), adaptação preserva (PASS). Não usa dados do jogo.
  Compilação final passou, teste TLS PASS, diff --check sem erros.
- Execução com apenas a ordem corrigida: PID 23040, 20261012_152415.log,
  console build/native-release-order.console.log. Falhou novamente na
  transição, pedido 4101, buffer 0 etiqueta 0, previous=1, count=4100,
  saída 23. A correção da notificação sozinha NÃO resolve o jogo.
  SHA256 desse executável intermediário:
  71f82a3055e4033398110b58c7091b545e430d6199b75cb81e9b9e245029cbf1.
- Adaptação adicional do Liverpool gerada em build: WRITE_DATA de 4 bytes
  com valor 1 para endereço de etiqueta VideoOut espera o valor real 0 antes
  de escrever. Usa YIELD_GFX da própria coroutine para permitir a liberação
  pela apresentação. Outros WRITE_DATA não alterados; nenhum valor forjado
  nem assert removido. Cabeçalho xxHash lido do cache externo existente.
- Teste ampliado compila o trecho real antes do WRITE_DATA contra coroutine
  e liberação tardia da superfície: referência perde trava antecipada,
  adaptação espera a liberação e preserva a nova trava. Ambos os cenários
  de teste (evento e escrita antecipada) PASS; TLS PASS novamente.
- Executável com as duas adaptações:
  39d27a785202a59b9cffa0f564619a749b6fe7810656b0fb9150b4865237024f.
  Execução PID 29280, log 20261012_152834.log, console
  build/native-surface-wait.console.log. Captura build/surface-wait-01.png
  confirma abertura renderizada e botões enviados chegam ao jogo. Ainda
  falta comprovar estabilidade durante gameplay e reproduzir o stall de
  flush; não declarar o port ou a primeira missão completos.
- Captura posterior build/surface-wait-02.png mostra o menu inicial inteiro
  com New Game, Load Game, Option e ???? legíveis; progresso visual observado
  com a espera de superfície. Processo segue aberto para teste, sem LLDB.
- Confirmação de New Game avançou ao carregamento e ao stage start da fase
  201. Captura build/surface-wait-03.png mostra a primeira rua na introdução,
  com prédios/placas definidos; ainda não é prova do desfoque resolvido no
  gameplay controlável. Última conferência: PID 29280 ativo, sem mensagem
  de falha IRQ ou stall nesta execução; deixar aberto para Davy testar.
- Continuam pendentes: validar essa corrida em jogo, diagnosticar stall,
  corrigir desfoque/menu inicial invisível e validar missão/salvar/recarregar.

## 12/10/2026 — retorno de Davy: câmeras e nova falha no gameplay

- Davy confirmou gameplay nesta execução. L1 alterna três modos de câmera:
  dois ficam nítidos e o modo normal permanece desfocado. Esse resultado
  restringe a investigação ao comportamento específico da câmera/efeitos;
  não comprova ainda erro de profundidade de campo ou causa no shader.
- A execução 20261012_152834.log terminou novamente com Out of order flip
  IRQ, saída 23. Último pedido 20623, buffer 0, etiqueta 1 ao registrar e 0
  no callback, previous=1, count=20622, pending=0. Uma única thread registradora
  nos últimos 64 eventos, callbacks na mesma ordem. 8.723 leituras,
  1.344.730.149 bytes, zero escritas. Não houve minidump: caminho atual de
  captura cobre fault/stall, enquanto esta saída é assert GPU.
- As duas adaptações de superfície passaram nos testes isolados, porém
  NÃO eliminaram a falha real; não declarar correção concluída. Investigar
  também as escritas reais de etiquetas e a condição de entrada da espera
  no Liverpool, além da sequência de liberação pelo driver. Histórico atual
  não registra essas operações e não permite atribuir a causa final.
- Preservados console build/native-surface-wait.console.log e log externo.
  Nenhum novo executável aberto nesta conferência; processo anterior saiu.

## 12/10/2026 — marcas de flip privadas e espera cooperativa da GPU

- Davy pediu corrigir os fechamentos e o desfoque. Histórico ampliado para
  256 eventos: escritas de etiquetas, correspondência do port, liberações,
  submissões e NOPs PatchedFlip. KYOEI_FLIP_STRESS=1 acrescenta 2 ms após
  escritas de etiqueta somente para reprodução; padrão desativado.
- Teste direto 20261012_154718.log: pedido 2344 falhou sem WRITE_DATA
  correspondente. Port e endereço de etiqueta conferidos pelo rastreamento.
  Cópia compartilhada original (GZ_COPY_GPU_BUFFERS=1) também falhou em
  20261012_154856.log, pedido 9521, durante a transição para a fase.
- Liverpool agora conserva cópias próprias de DCB/CCB como parâmetros de
  uma coroutine por envio, até processamento completo. Não usa a arena
  compartilhada que SubmitDone reinicia. Teste isolado comprova conservação
  após reutilização da origem, suspensão de dois envios e liberação dos
  frames de coroutine. Essa mudança sozinha ainda falhou em
  20261012_155436.log, pedido 8696.
- Esse último histórico identificou NOP PatchedFlip com header c09e1000,
  fora do NOP específico produzido pelo patch, sem trava precedente. Causa
  investigada: PatchFlipRequest modificava a memória do jogo e deixava sua
  marca interna ali; reutilização do NOP podia conservar o payload antigo.
- GNM local gerado em build passa a aplicar PatchFlipRequest numa cópia
  privada do último DCB e encaminhar essa cópia ao Liverpool, que a conserva.
  Arrays e comandos originais intactos. Preservados retornos de erro e
  callbacks reais, sem fabricar etiquetas ou retirar o assert de flip.
  Guards sem janela mantidos; reserva da antiga arena removida na unidade
  local. Godzilla e snapshot intactos; unidades adaptadas: VideoOut, driver,
  Liverpool e GNM. Não depende de GZ_COPY_GPU_BUFFERS para copiar envios.
- tools/test_flip_patch.py usa o wrapper real com patch mutante controlado:
  referência deixa marca na origem (falha esperada); adaptação mantém origem,
  encaminha comandos alterados e propaga erro sem submeter (PASS).
  tools/test_gpu_snapshot.py, test_videoout_order.py e test_kyoei_tls PASS.
- Execução 20261012_160128.log com patch privado e stress atingiu gameplay
  e menus. Não falhou por flip nessa tentativa, mas saiu 30 por stall ao
  aproximar-se de Ultraman; Davy relatou fechamento quando foi pisado.
  syncLabel=45535, expectedLabel=45543, trap em eboot+0x4331ba.
- Captura interna comprovada: render-stall e fault .dmp em build, cerca de
  0,9 MB cada. Análise LLDB offline funcionou sem conectar a processo vivo.
  No dump de fault, etiqueta real em 0x1038255300 já tinha 0xb1e7 (45543),
  exatamente o esperado. Thread Liverpool ociosa no dump de stall. O trabalho
  terminou depois do limite de tentativas, antes do encerramento; isso
  fundamenta adaptar a espera, sem falsificar conclusão gráfica.
- src/kyoei_gpu_wait.c/h: antes da rotina original de espera, lê a etiqueta
  real e cede CPU com Sleep(1), por até 3000 ms. Ao terminar/expirar entra
  na rotina original: comparação, 10 milhões de tentativas e int 0x41
  preservados. Caso de etiqueta nula segue a seleção frame-local original.
  Relay de 29 bytes próximo à imagem, argumentos SysV preservados, código
  RX após montagem e proteção da página original restaurada.
- Adaptação só aceita a assinatura dos 316 bytes de eboot+0x4330b0:
  FNV64 fa40be72aa1369e9, SHA256
  f70d547733e932e9a82473aeed96abb2658b1c393b02ce235f5e0ab699de872d.
  Não muda os arquivos preparados nem o dump; instala desvio na memória.
  test_kyoei_gpu_wait PASS: fence atrasada, timeout sem escrever etiquetas,
  relay preservando três argumentos mesmo com callback que os altera.
- Executável final desta etapa SHA256
  1382c5f4995a13303866919d87761d41cdad2b43e39dd5025dcc5868088ddee0.
  Diagnóstico --list-stubs sem janela passou após GNM local (451 runtime,
  418 módulos do jogo, 297 stubs); esses números não validam gameplay.
- Execução com espera e sem stress abriu, assinatura aceita; fechada
  normalmente pelo agente (CloseMainWindow) para comparar opção gráfica,
  saída 0. Nenhuma falha observada nesse teste limitado de abertura.
- Davy confirmou em vídeo do PS4 que câmera normal fica nítida; aqui dois
  modos alternativos são nítidos e o normal ainda desfocado. Q adicionado
  ao input_native.ps1 para L1. Capturas camera-private-01/02 em build; a
  segunda coincidiu com menu aberto por Davy, não é comparação controlada.
- Pista externa, não comprovação: issue shadPS4 #5057 relata névoa incorreta
  em CUSA03562 01.00 sem Readback Linear Graphics. Fonte:
  https://github.com/shadps4-emu/shadps4/issues/5057 . Nosso dump é 01.03.
  Opção existente GZ_READBACK_LINEAR=1 testada, sem desativar efeito do jogo.
- Nova execução de comparação: PID 28832, 20261012_161617.log,
  build/native-linear-wait.console.log, sem stress, com leitura linear e
  espera cooperativa. Ainda falta conferir câmera normal e passar pelos
  efeitos/Ultraman nessa execução. Não declarar estabilidade ou missão
  completa; salvamento/recarregamento seguem pendentes.

### 12/10/2026 — câmera nítida, PNG e primeira medição de desempenho

- Conclusão da execução 20261012_161617.log: captura
  build/linear-wait-06.png mostrou gameplay com câmera normal nítida,
  antes de alternar L1. Um modo alternativo também foi capturado nítido;
  a captura do terceiro estava minimizada e não serve de prova.
  GZ_READBACK_LINEAR=1 agora é padrão no lançador e na integração nativa;
  valor explícito 0 continua disponível para diagnóstico. Não removemos
  o efeito de foco do jogo. Capturador agora rejeita janela minimizada.
- Nessa execução a espera real de GPU terminou após 78 ms,
  expected=actual=192888, reached=1, sem forjar etiquetas. O jogo avançou
  até pedir scePngEncQueryMemorySize e encerrou com stub ausente (saída 20).
  Houve escrita de imagens em download0 e referência a
  SV_ID_201_AFTER_POLICE, mas isso não comprova save/load completo.
- Implementado src/kyoei_png.c/h: QueryMemorySize, Create, Encode e Delete,
  ABI SysV, codificação PNG real com miniz já existente, RGB/RGBA a partir
  de RGBA/BGRA, pitch e filtros PNG, limites e erros sem sobrescrever a
  saída quando o buffer é insuficiente. Sem nova biblioteca baixada.
- Testes: 64 PNGs decodificados independentemente com Python/zlib, CRC,
  pixels e filtros conferidos; formato, alpha, padding, buffer pequeno,
  entrada truncada e ciclo de handles PASS. Testes flip privado, snapshots,
  ordenação VideoOut, espera GPU e TLS PASS na compilação otimizada.
- Encontrado build local em Debug. Reconfigurado para RelWithDebInfo
  (otimização com símbolos), alinhado ao CRT release da biblioteca GPU.
  Build final passou; SHA256 do executável:
  3869508734a87cc6e06175978ca772ce7695226ae7967f0895f22b64e45f8aeb.
  --list-stubs passou: 455 runtime, 418 módulos, 293 stubs restantes.
- KYOEI_FRAME_STATS=1 mede intervalos entre apresentações concluídas do
  jogo, sem contar redesenhos ociosos. Relatório a cada 120 intervalos:
  FPS médio, média, p50/p95 e máximo. Não mede CPU/GPU separadamente.
  Nova execução em build/native-png-release.console.log, PID inicial
  22556, sem stress. Primeiras janelas após abertura perto de 60 FPS,
  mas tela/cena não certificada; não usar como ganho de gameplay.
  Primeira captura release-png-open.png preta, não comprova renderização.
- Pedido de Davy: começar inglês e melhorar desempenho. Plano salvo em
  docs/PLANO.md: estabilidade/save-load e medição em paralelo a um piloto
  de uma frase, depois 20–30 textos, cobertura M2/fontes M3 e revisão por
  cena; só depois tradução ampla/patch. Nenhuma tradução visível validada
  ainda. PNG precisa ser exercitado na partida; missão completa e ganho
  de FPS em percurso controlado continuam pendentes.
- Registro da nova execução: 20261012_201311.log. Captura posterior
  build/release-png-stage.png mostra tela de carregamento da primeira fase,
  58%, cenário da estação e narração visíveis. Durante esse carregamento
  relatório caiu a 13,14 FPS (p95 234,82 ms, máximo 665,95 ms); isso reforça
  separar carga/transição de gameplay. Os ~60 FPS anteriores não são
  evidência de desempenho na cidade. Partida deixada aberta para Davy.

### 12/10/2026 — erro real de salvamento, parada de módulo e profiling

- Davy relatou visual correto, desempenho abaixo de 30 FPS e fechamento
  após problema ao salvar. Log 20261012_201311 confirma vários relatórios
  de 9,77–21,42 FPS durante a fase, além de trechos perto de 30. Não houve
  melhora de fluidez comprovada pela compilação otimizada.
- PNG realmente exercitado: 228x128 RGB, 51.699 bytes. Depois o jogo pediu
  SaveDataDialog LIST (mode=1), não suportado no runtime fixo. Open retornou
  0x80b8000f, erro mostrado ao usuário; nenhuma escrita de save nessa partida.
  Mais tarde limpou os recursos da fase e fechou por stub de
  libkernel::sceKernelStopUnloadModule, eboot+0x6c31ac, handle 0x4001.
  O fechamento não foi uma prova de corrupção de save nem falha do PNG.
- Adaptação local de services/dialog_ui: lista de slots com navegação pelo
  controle/setas, OK/cancelar e novo save quando permitido. Resultado preserva
  ponteiros do chamador, usa diretório vazio para novo slot e buttonId INVALID
  conforme referência. Confirmações SYSTEM_MSG CONFIRM/OVERWRITE têm escolha
  real Sim/Não; demais tipos ainda são recusados explicitamente. A lista
  mostra nomes de diretórios, sem miniaturas/metadados de título; parâmetros
  do save selecionado ainda não são copiados para result.param. Carregamento
  e salvamento completos dependem de validação real, não declarar M6 concluído.
- Teste tools/test_save_list.py usa funções reais do services adaptado com
  UI controlada: novo/existente, cancelar, limites, ponteiros preservados,
  retorno Não e mensagem não suportada PASS.
- kyoei_modules suporta StopUnloadModule chamando DT_FINI real do módulo.
  Finalizadores verificados contra hashes M0 por tools/update_module_stops.py:
  libSceSecure offset 0x157a0; StageModule offset 0x140. Metadados F registrados
  só na saída preparada, preparador atualizado para futuras saídas. Após
  parada bem-sucedida marca módulo parado, impede dlsym e admite novo start;
  imagem fixa continua mapeada, não fingir que memória foi desmapeada.
  test_kyoei_modules PASS: argumentos/resultado, falha preservando estado,
  handle inválido e parada duplicada. Finalizador real na partida pendente.
- Profiling opcional: custo de PrepareFrame, Present e ProcessDownloadImages
  em médias/p95/máximo de 120 chamadas. TextureCache copiado/adaptado localmente;
  nenhuma mudança no algoritmo de leitura. Caminho existente espera GPU por
  imagem; hipótese de gargalo, ainda sem medição de gameplay que a confirme.
  Preparação/apresentação iniciais abaixo de 1 ms em média, carga posterior
  apresentou 17–26 FPS, ainda não atribuir causa. Readback continua habilitado.
- Execução nova: build/native-save-profile.console.log, PID inicial 32544,
  KYOEI_FRAME_STATS=1, stress=0. Executável em uso SHA256
  ae57e0f4d0e71f70a872faf936ed2e4ad537fbcacb5ce68f0a36e894ea0ae4ff.
  Ajuste posterior de conversão de handle evita overflow de inteiro no fonte;
  test_kyoei_modules já o compila, executável da partida ainda é o anterior.
  Não recompilar/substituir o executável em uso para não interromper Davy.

### 12/10/2026 — Mount2 após escolha de slot e medição de TLS

- Davy confirmou novo fechamento ao clicar para salvar e nenhuma melhora
  de desempenho percebida. Execução 20261012_202521.log mostrou cancelamento
  da lista funcionando e, na tentativa seguinte, seleção de novo slot
  funcionando: selected=0, directory vazio, result=0. Gerou PNG 228x128
  de 51.699 bytes e entrou no progresso de gravação. Fechou por stub
  sceSaveDataMount2 em eboot+0x6d2826, não por falha da lista. Writes=0.
- src/kyoei_savedata.c implementa Mount2 (64 bytes, mode offset 24) adaptando
  ao Mount real já existente (80 bytes, mode offset 40). Preserva usuário,
  diretório, blocos, modo, ponteiro de resultado e erros do backend. Usando
  título configurado a partir do SFO, não o título do Godzilla.
- Conferidas todas as 13 importações libSceSaveData do eboot. As duas outras
  ausentes eram GetProgress/ClearProgress, relativas ao backup assíncrono.
  Cópia adaptada rt_savedata em build adiciona consultas com validação de
  inicialização e ponteiro; zero representa ausência de trabalho de backup.
  Não reporta sucesso de gravação nem implementa backups inexistentes.
- Build passou. test_kyoei_savedata PASS (ABI/encaminhamento e erros),
  test_kyoei_tls PASS. tools/test_savedata_storage.py usa o backend real
  e pasta isolada build/test-save-storage/user, sem tocar nos saves de Davy:
  payload e metadados escritos, desmontados, terminados, reabertos e lidos
  idênticos; traversal inválido recusado, progresso e inicialização conferidos.
  Mapeamento guest no teste é controlado; isso não comprova save/load do jogo.
- Profiling da partida anterior: grupos de leitura linear com média 23,39 ms
  no primeiro relatório e 12,35 ms no seguinte, p95 18 ms no segundo.
  Prepare/Present perto de 0,15/0,5 ms. Readback tem custo relevante, mas
  ainda não explica sozinho a lentidão. Não removido, preservando visual.
- Adicionado contador de exceções FS:[0] tratadas por TLS e relatório por
  120 apresentações, para investigar custo CPU específico do Kyoei. Ainda
  não otimizado esse caminho nem feita afirmação de ganho de FPS.
- Nova partida aberta: build/native-mount2.console.log, stress=0, métricas=1.
  Retestar save e load no jogo; estabilidade completa e performance pendentes.

### 12/10/2026 — gravação confirmada; consulta de módulo ao dar load

- Davy relatou fechamento ao dar load e leve melhora de desempenho.
  Execução 20261012_203450.log confirma gravação real: SAVEDATA.DAT
  254.576 bytes, SYSTEM.DAT 26.344 bytes, dois param.bin de 1.328 bytes,
  ícones PNG. Ambos os jobs de gravação retornaram 0. Runtime registrou
  138 escritas. Arquivos ficaram no user separado, não no dump/repositório.
- Load selecionou SAVEDATA-000, montou existente read-only, terminou
  o diálogo e começou desmontagem da fase. Fechou depois em
  libc.prx+0x71178 por sceKernelGetModuleInfoFromAddr, addr=0x801c2c020,
  flags=2. Consulta foi feita dentro do finalizador real de StageModule.
  Não há evidência de save corrompido; gameplay restaurado ainda não provado.
- Implementada consulta por endereço com ABI de 424 bytes, segmentos
  offset 352, limites exatos, flags 0..2 e erros reais para parâmetros
  inválidos/endereço desconhecido. Responde só módulos diferidos cujos
  metadados foram certificados; outros endereços retornam ESRCH.
  Não pega lock de ciclo de vida, pois finalizador já o possui.
- tools/native_module_metadata.py deriva segmentos PT_LOAD/SCE_RELRO,
  finalizador e EH frame de ELF próprio verificado. Aceita EH versão 1 com
  ponteiro pcrel sdata4 observado; rejeita outros formatos. Segmentos usam
  bytes efetivamente preparados, sem anunciar páginas extras do emulador.
  Preparador e atualizador de metadados usam o mesmo gerador. Nenhum código
  ou texto de jogo foi versionado; tabela fica na saída privada preparada.
- test_kyoei_modules PASS: fronteiras, flags, nulo, chamada de info dentro
  de finalizador sem deadlock, estado/erros de stop. Build e --list-stubs
  passaram: 460 runtime, 418 módulos do jogo, 288 stubs restantes.
- Executável SHA256
  037a4cbfd892604d1fbb618d8c6429d1004570a1c8d9d9738a711844ce994878.
  Nova partida aberta em build/native-load-info.console.log, stress=0,
  métricas=1; saves anteriores preservados. Load completo continua pendente.
- Performance: medição anterior mostrou aproximadamente 21.000 exceções
  TLS tratadas por apresentação em alguns trechos; linear-downloads chegou
  a média 9,90 ms/p95 15,93 ms. TLS é forte candidato de custo CPU a otimizar;
  ainda não foi alterado o mecanismo nem comprovado ganho controlado.
  Leve melhora percebida por Davy é relato de teste, não benchmark.

### 12/10/2026 — load confirmado; Continue e TLS direto

- Davy confirmou que load restaurou gameplay, jogou, morreu e o Continue
  fechou. Save/load inicial agora tem confirmação humana; retomada após
  morte e missão completa permanecem pendentes. Partida anterior
  20261012_204140.log fechou por __elf_phdr_match_addr dentro de
  libc+0x711e4, durante StopUnloadModule da fase, não por leitura do save.
- Implementada comparação real de endereço contra segmentos executáveis
  de ModuleInfoEx. Fonte primária de comportamento FreeBSD:
  https://lists.freebsd.org/pipermail/svn-src-head/2010-August/019849.html
  Disassembly do libc próprio em build/libc-inspect.elf confirmou que o
  PS4 passa o mesmo ModuleInfoEx devolvido por GetModuleInfoFromAddr,
  e não o dl_phdr_info do FreeBSD. Não retornar sucesso fixo para ignorar
  destructors. Testes de segmento executável/dados, limites e nulo PASS.
- Performance anterior: 19–33 mil exceções TLS por apresentação em
  vários trechos; linear-downloads média 15,12 ms/p95 24,28 ms em relatório.
  Implementado caminho direto para as 580 leituras RAX FS:[0] já catalogadas.
  Slot próprio TlsAlloc abaixo de 64, layout x64 TEB inline conferido contra
  TlsGetValue com sentinela antes de patch. Instrução GS mantém registrador,
  tamanho e flags; nenhuma chamada/trampoline/red-zone. Proteções restauradas
  e instruction cache limpo. Alteração em memória antes das threads guest;
  dump e arquivo preparado intactos. Entradas main/pthread preinicializam
  TCB. Outros caminhos host->guest novos precisam do mesmo cuidado.
- KYOEI_TLS_FAST=0 permite baseline. Sem metadados ou slot/layout compatível
  mantém exceções. Gerador/update_tls_sites.py verifica eboot M0 e exporta
  só offsets privados para tls-sites.tsv; runtime verifica assinatura exata
  de cada instrução, recusa incompatibilidade. Cópia de rt_thread gerada
  no build; core fixo e Godzilla preservados.
- Teste TLS com duas threads e 10.000 leituras por thread: legado
  32,37/32,74 ms e 20.002 exceções; direto 0,02/0,10 ms e zero exceções.
  Template, zeragem, isolamento, alinhamento e cleanup PASS nos dois modos.
  Ganho é do teste de TLS; ainda não é comparação controlada de FPS na cidade.
- Build, test_kyoei_modules, test_kyoei_tls nos dois modos e --list-stubs
  PASS. 461 runtime, 418 módulos, 287 stubs. As 580 assinaturas reais
  foram aceitas; slot 8. Executável SHA256
  82e1714070e7c6e420a365320cac389de907f73d0f9cc418aeee7faea6b548c8.
- Nova execução build/native-continue-fast.console.log, PID inicial 32680,
  fast=1, métricas=1, stress=0. Abertura com handled_total=0. Relatórios
  iniciais de 60 FPS ainda são abertura, não prova de desempenho na cidade.
  Partida deixada aberta para confirmar Continue e fluidez após load.

### 12/10/2026 — limpeza de destructors de threads ao continuar

- Davy relatou rodar melhor, mas Continue após morte ainda fechou.
  20261012_205113.log confirma que o finalizador avançou além de
  __elf_phdr_match_addr e fechou na próxima API ausente:
  libkernel::__pthread_cxa_finalize, libc+0x71309, chamada no stop da fase.
  TLS handled_total permaneceu zero ao longo da partida. Relatórios finais
  incluíram 33–41 FPS e quedas a 19–25; melhoria percebida não significa
  performance concluída nem comparação de rota controlada.
- Referência de semântica: limpeza de handlers atfork e destructors TSD ao
  descarregar biblioteca. Runtime nativo não aceita registros atfork/sinais
  e não tem filas desses handlers; tem tabela real de chaves TSD.
  Fonte primária para essa tabela:
  https://raw.githubusercontent.com/freebsd/freebsd-src/main/lib/libthr/thread/thr_spec.c
- src/kyoei_thread_finalize.h, incluído na cópia local rt_thread gerada,
  implementa __pthread_cxa_finalize retirando só destructors de chaves
  usados cujo endereço pertence aos segmentos executáveis da fase.
  Mantém chave alocada e valor, como _thr_tsd_unload de referência;
  mantém destructors de outros módulos. Não executa destructors nesse
  ponto, não termina threads, não retorna sucesso fixo para ignorar limpeza.
- tools/test_thread_finalize.py usa as operações reais de chaves do core
  e o adaptador real: callback da fase removido, callback estrangeiro e de
  região não executável preservados, chave/valor mantidos, repetição idempotente,
  cleanup posterior chama callback restante. PASS. Build, test_kyoei_modules
  e --list-stubs PASS. 462 runtime, 418 módulos, 286 stubs.
- Nova execução build/native-pthread-finalize.console.log, fast=1,
  métricas=1, stress=0. Saves preservados. Continue precisa de nova validação
  no jogo; não declarar estabilidade completa. Godzilla/core fixo intactos.
