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
