# Port nativo: base do Godzilla integrada

## Resultado desta integração

O Kyoei agora tem um alvo próprio `build/kyoei.exe`, compilado com o
carregador e runtime do Godzilla e ligado à biblioteca gráfica já compilada
de lá. A execução não inicia o executável do shadPS4. O processamento
gráfico continua usando o código adaptado do shadPS4, como no Godzilla.

A base fixa está em `third_party/godzilla-core`, ignorada pelo Git,
no commit `e976b59a3f056c5ea5d0ddfe1c5ef566cce2f45e`. Os fontes vêm
do commit, incluindo as referências Bloodborne
`daa71654083b42ea0730b3f0fecee2fc20942d02` e shadPS4
`3d03375a5bbfa52772a2142e151d6b73af48dc8d`.
Fontes e bibliotecas locais têm hashes em `godzilla-core.json`.
Nenhum arquivo do jogo ou firmware está nessa cópia ou no Git.

O P.T. permanece referência para substituir o código original por uma
reimplementação no futuro. Não foi incorporado código do P.T. nesta etapa:
o carregamento nativo inicial segue a base Windows do Godzilla/Bloodborne.

## Adaptação específica do Kyoei

- `tools/prepare_native.py` usa os parsers e o ligador fixos do Godzilla,
  com política própria de identidade: CUSA03562 01.03 e hashes M0 do eboot,
  SFO e módulos do jogo usados. Não desliga a validação com uma variável.
- `src/kyoei_kernel.c` adapta `sceKernelGetAppInfo` para informar CUSA03562.
  O símbolo do resolvedor compartilhado é renomeado na compilação; nenhum
  fonte da cópia fixa é editado.
- `--app0` aponta explicitamente para o dump próprio do Kyoei.
  `--user` fica em `G:\kyoei-toshi-work\native-04\user`.
  O runtime de salvamento lê a identificação do SFO. Dados do Godzilla
  não são usados como diretório de saves do Kyoei.
- A GPU foi reaproveitada como bibliotecas locais com hashes. O CMake
  recompila os fontes do carregador/runtime e a adaptação do Kyoei.
  Este primeiro fluxo depende da instalação local; ainda não é um pacote
  de compilação portátil ou uma distribuição para outros usuários.

## Provas

Preparação e compilação concluídas. O eboot tem imagem de 22.408.592
bytes, 704 importações e 82.840 relocações. Ligados os módulos próprios
libc/Fios2 e os módulos externos de fontes/áudio já usados no Godzilla.
`native-prepare.json` registra a identidade e o hash da imagem.

**09/10/2026: a tela de título foi renderizada pelo port nativo.** Captura
externa `native-04/window-01.png` conferida visualmente: logo de Kyoei Toshi
sobre a cidade. O DualSense foi reconhecido e seus botões chegaram ao jogo.
Depois dessa primeira abertura, uma consulta de DLCs ainda não implementada
encerrou a execução; a lista vazia de conteúdo extra foi adaptada e o jogo
foi reaberto. A abertura não prova que uma missão inteira seja jogável.

`native-proof.json` conserva a falha histórica de TLS; `native-current.json`
registra a execução atual. O módulo Secure do dump tem inicialização adiada
até `sceKernelLoadStartModule`, com exports resolvidos por NID e handles
próprios. O NpToolkit2 também é ligado do dump original. Rede e transmissão
permanecem offline; não foi habilitado retorno zero geral para funções ausentes.

O TLS guarda valores separados para cada thread do jogo. O Godzilla
não precisava desse caso no eboot, então a implementação Windows o
rejeita explicitamente em `src/loader.c`. A base Bloodborne já tem
suporte ao TLS principal em `src/runtime.c`, `src/runtime_thread.c` e
`src/probe.c`, mas a adaptação para Windows não está feita: Windows usa
GS para seu próprio TEB, portanto a solução Linux não pode ser copiada
como uma troca simples de FS por GS.

## Desenvolvimento independente autorizado por Davy

Em 08/10/2026, Davy revogou expressamente a dependência de corrigir primeiro
o Godzilla: ele é a base de partida e continua intacto. Melhorias necessárias
ao Kyoei são implementadas e testadas **neste projeto**. O snapshot original
continua fixo; adaptações ficam em `src/` e no arquivo gerado em `build/`.

O adaptador local já implementa:

1. Validar e registrar o template TLS do eboot, tamanho e alinhamento,
   conservando módulo 1 para o eboot e 2..7 para módulos ligados.
2. Criar uma cópia por thread, inicializando os bytes do template e zerando
   a parte sem dados; assegurar isolamento entre threads e alinhamento.
3. Atender `__tls_get_addr` para o módulo 1 e a instrução medida
   `mov r64, fs:[0]`. A versão inicial atende por exceção; a otimizada
   troca os 580 `mov rax, fs:[0]` catalogados por leitura GS do slot que
   `TlsAlloc` reservou para o jogo. Não muda o registro GS nem dados de
   outros slots. Confere assinatura, limites e layout contra `TlsGetValue`
   antes de alterar código somente em memória, antes das threads guest.
4. Criar TLS por thread e liberar via FLS na saída. Entradas do eboot e
   pthreads guest inicializam seu bloco antes de executar instruções
   otimizadas. Novos caminhos de callback host para guest devem fazer o
   mesmo. `KYOEI_TLS_FAST=0` mantém o caminho por exceção para comparação;
   metadados ausentes ou slot/layout incompatível também usam esse caminho.
5. Teste Windows real com template sintético, parte zerada, alinhamento,
   acesso FS legado/direto e duas threads simultâneas: PASS. O boot real avançou além
   de TLS, libc/Fios2/fontes/áudio e entrou no eboot.

Não é uma implementação completa de todas as instruções FS ou formatos
TLS possíveis. O adaptador atende a forma observada e mantém os demais
casos como falhas identificáveis.

Dados reais do template estão em `native-proof.json`, seção `main_tls`;
o relatório detalhado local fica em `native-02/out/link.json`.
O projeto oficial e a cópia fixa não foram alterados.

Também normalizados três nomes vazios vindos do índice aerolib como `?`
na tabela local; seus NIDs e posições são preservados. Implementados
`sceLibcHeapGetTraceInfo` com tabelas persistentes conforme a referência
e `sceKernelIsNeoMode` como PS4 padrão. Não foi ligado o modo de retornar
sucesso para todas as funções ausentes.

## Comandos locais

```powershell
python tools/snapshot_godzilla.py
python tools/prepare_native.py
cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --parallel 4
powershell -ExecutionPolicy Bypass -File scripts/run_native.ps1
```

Snapshot e preparação recusam sobrescrever uma saída existente. Nesta
instalação já foram executados; não é preciso repeti-los para compilar.
`native-01/out` é uma tentativa parcial que parou ao imprimir o título
japonês com a codificação do terminal. `native-04/out` é a preparação atual,
incluindo Secure e NpToolkit2. O adaptador configura a saída do Python como UTF-8.

Ao executar a versão atual, espera-se a tela de título com o logo sobre a
cidade. O controle conectado é reconhecido. Jogabilidade, missões e gravação
de saves ainda precisam ser validadas por avanço real dentro do jogo.
