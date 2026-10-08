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
  `--user` fica em `G:\kyoei-toshi-work\native-02\user`.
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

**O jogo ainda não abriu pelo port.** A primeira execução terminou com
código 1, antes da janela, porque o carregador rejeita TLS estático do
eboot. `native-proof.json` registra o executável, os hashes e o bloqueio.
Não confundir a execução anterior no emulador com execução deste port.

O TLS guarda valores separados para cada thread do jogo. O Godzilla
não precisava desse caso no eboot, então a implementação Windows o
rejeita explicitamente em `src/loader.c`. A base Bloodborne já tem
suporte ao TLS principal em `src/runtime.c`, `src/runtime_thread.c` e
`src/probe.c`, mas a adaptação para Windows não está feita: Windows usa
GS para seu próprio TEB, portanto a solução Linux não pode ser copiada
como uma troca simples de FS por GS.

## Correção compartilhada necessária

Pela regra de código compartilhado do projeto, implementar **primeiro na
sessão do Godzilla** e depois renovar esta cópia fixa:

1. Validar e registrar o template TLS do eboot, tamanho e alinhamento,
   conservando módulo 1 para o eboot e 2..7 para módulos ligados.
2. Criar uma cópia por thread, inicializando os bytes do template e zerando
   a parte sem dados; assegurar isolamento entre threads e alinhamento.
3. Atender `__tls_get_addr` para o módulo 1 e os acessos diretos FS do
   executável sem alterar o TEB do Windows ou o TLS das bibliotecas host.
4. Integrar criação e liberação de TLS na thread principal e nas threads
   PS4 criadas pelo runtime. Não remover apenas a rejeição do carregador.
5. Testar com template sintético, parte zerada e duas threads; repetir o
   teste integrado e uma execução do Godzilla para evitar regressão.
6. Commitar na fonte oficial, renovar o snapshot e repetir o boot do Kyoei.

Dados reais do template estão em `native-proof.json`, seção `main_tls`;
o relatório detalhado local fica em `native-02/out/link.json`.
Esta sessão não alterou o projeto oficial nem a cópia fixa.

## Comandos locais

```powershell
python tools/snapshot_godzilla.py
python tools/prepare_native.py
cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build --parallel 4
powershell -ExecutionPolicy Bypass -File scripts/run_native.ps1
```

Snapshot e preparação recusam sobrescrever uma saída existente. Nesta
instalação já foram executados; não é preciso repeti-los para compilar.
`native-01/out` é uma tentativa parcial que parou ao imprimir o título
japonês com a codificação do terminal. `native-02/out` é a preparação
concluída. O adaptador agora configura a saída do Python como UTF-8.

Ao executar a versão atual, espera-se a mensagem de TLS e código 1;
ainda não se espera a tela do jogo. Após a correção compartilhada, o
próximo teste deve mostrar avanço do carregador além desse ponto.
