# Linux x64 e portable compartilhado

### Runtime POSIX inicial — retomada de 15/10

Calendário/horário (`rt_rtc`) e semáforos/event flags (`rt_sema`) agora são
derivados da referência fixa com âncoras conferidas. Adaptadores em `tools/`
geram apenas cópias em build; o snapshot permanece intacto. O backend usa
clock_gettime/localtime_r, mutexes pthread e condition variables com relógio
monotônico. Exclusão libera os objetos POSIX após a saída dos esperadores.
Mantém as limitações da referência: prioridade aproximada pela fila, resolução
de espera em milissegundos e conversão de fuso usando o offset atual.

Build Ubuntu x86-64 com Clang e quatro testes CTest PASS: RTC, semáforos/eventos,
TLS e recusas do carregador. Testes incluem calendário/bissextos/época Unix,
hora real, UTC/Bahia/offset fracionário, tokens, timeout e cancelamento/exclusão
com threads bloqueadas. A imagem real agora liga **10 imports ao runtime POSIX**,
418 aos módulos nativos e deixa **738 stubs**. Evidência privada em
`build/linux/bootstrap-posix-runtime.log`. O carregador continua recusando
executar o jogo; memória, threads PS4, arquivos, serviços, saves e GPU ainda
precisam de implementação/integração. Nenhum iniciador nativo foi publicado.

### Primeiro carregador Linux e TLS — 15/10, após instalação

Existe agora **`build/linux/kyoei-linux-bootstrap`, ELF x86-64 compilado em
Ubuntu**, reutilizando o parser BBPROBE5, validações de segmentos/importações,
relocações e ligações nativas do carregador fixo do Godzilla. O snapshot não
foi modificado: `tools/prepare_linux_bootstrap.py` deriva uma cópia de build
com âncoras verificadas e host POSIX em `src/linux/`. A configuração Linux
do CMake não usa as bibliotecas COFF Windows.

Prova com a imagem real preparada do próprio dump: 29.540.440 bytes mapeados
em 0x800000000, 36 segmentos, 88.561 relocações, 418 importações ligadas aos
módulos nativos e 748 ainda sem runtime Linux. Logs privados em
`build/linux/bootstrap-real.log`. O binário exige `--list-stubs` e recusa
executar o convidado. Não foi instalado como iniciador do portable.

TLS principal implementado com pthreads/posix_memalign e GS, mantendo FS da
glibc. Layout segue o adapter Windows, com alinhamento do bloco convidado
independente do alinhamento da alocação do host. Inicialização por thread,
template/zeros, DTV, entradas aninhadas, restauração de GS e liberação por
destrutor. Assinaturas das **580 instruções reais** conferidas antes de
trocar FS por GS na imagem gravável; somente depois aplica mprotect.
Probe do host confere GS e restaura seu valor. Nenhum código do jogo executado.

CTest: TLS em quatro threads com isolamento, errno/FS intacto, alinhamento e
restauração de GS PASS; rejeição de execução/magic/truncamento/limites/TLS
inválido/módulos inválidos PASS. Ainda faltam runtime PS4, TLS de módulos,
signals/fault handling, memória direta/coerência GPU, Vulkan/janela/input,
saves e launcher Linux. **O jogo ainda não roda em Linux.**

Reprodução no Ubuntu: `cmake -S /mnt/g/kyoei-toshi -B /mnt/g/kyoei-toshi/build/linux -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++`,
depois `cmake --build /mnt/g/kyoei-toshi/build/linux` e
`ctest --test-dir /mnt/g/kyoei-toshi/build/linux --output-on-failure`.
Carregador: `build/linux/kyoei-linux-bootstrap /mnt/g/kyoei-toshi-work/native-05/out --list-stubs`.

Registro anterior da instalação do ambiente:

Pedido de Davy em 15/10/2026: manter x64; Windows e Linux na mesma pasta,
sem duplicar os dados do jogo e sem encher a raiz de ferramentas.
RX 580 + i5-9400 é um alvo para avaliação, não um mínimo já comprovado.

Estado após reinício de Davy em 15/10: **ainda não existe executável Linux do
Kyoei**, mas o ambiente já funciona. Instalados WSL 3.0.3 e Ubuntu 26.04.1 LTS
em WSL2; `uname -m` confirma x86_64. Clang 21.1.8, CMake 4.2.3, Ninja 1.13.2,
Vulkan 1.4.341, SDL3 3.4.2 e bibliotecas de desenvolvimento FFmpeg disponíveis.
`scripts/setup_linux_toolchain.sh --verify-only` compilou e executou um pequeno
programa C, identificado como ELF x86-64. É prova do ambiente, não do port.
Saída privada em `build/linux-environment/compiler-smoke`.
O script sem `--verify-only` instala as dependências, executado como root
no Ubuntu. Nenhuma conta pessoal ou senha configurada; uso de desenvolvimento
por `wsl -d Ubuntu -u root`. Nenhuma partida iniciada. Próximo passo continua
sendo o backend POSIX e integração das dependências ELF descritos abaixo.

Histórico antes do reinício: a preparação do ambiente
avançou em 15/10: `scripts/setup_linux_build.ps1` foi executado com elevação do
Windows, habilitando WSL/VirtualMachinePlatform com `-NoRestart`. Resultado em
`build/linux-setup-status.json` era `restart_required`. O agente não solicitou
nem realizou reinício. Ubuntu e toolchain ainda não estavam instalados; após reinício voluntário
de Davy, o mesmo script foi retomado e a distribuição conferida.

As tentativas iniciais de `wsl --install` sem token administrativo retornaram
código1. A preparação administrativa posterior resolveu a ativação dos
componentes, mas não elimina o trabalho de backend descrito abaixo.
Renomear o PE ou adicionar um script Wine não produz um port Linux nativo.

A referência local `build/reference-bloodborne`, commit registrado em
FONTES-CODIGO.md, já contém implementação Linux em `src/probe.c`,
`src/runtime_memory.c` e `build.sh`: mmap/memfd, signals/ucontext e pthreads.
Ela é uma referência concreta para o backend do host. Seu runtime não pode
substituir o atual sem revalidar importações, memória, TLS, saves e gráficos.

Bloqueios confirmados no Kyoei:

| Área | Dependência atual | Trabalho para Linux |
| --- | --- | --- |
| Build | libraries.cmake aponta bibliotecas COFF `.lib` Windows | compilar dependências ELF com SDL3/Vulkan/FFmpeg |
| Carregador | VirtualAlloc, VEH, Win32 e PE do host | backend mmap/signals e carregador ELF do host |
| TLS | otimização baseada em slot de thread Windows | backend por thread e nova prova das 580 instruções adaptadas |
| Runtime | threads, locks, arquivos, relógios e memória Win32 | adapters POSIX, mantendo os contratos PS4 |
| Janela | HWND, IsIconic, ícone Win32 | usar superfície SDL Linux e eventos de minimização |
| Diálogos/saves | fallback Win32, SRWLOCK, substituição MoveFileExW | manter overlay Vulkan e backend de arquivos POSIX |
| Launcher | C# WinForms | launcher nativo simples com os mesmos caminhos relativos |

Organização pretendida: apenas os dois iniciadores na raiz; `game/`,
`languages/`, `assets/` e `licenses/` compartilhados. Binários/dependências
específicos dentro de `runtime/windows/` e `runtime/linux/`. Preservar `data/`
e saves existentes, sem converter ou migrar saves automaticamente. Caches
de driver são locais ao dispositivo/driver e não devem ser distribuídos.
Não alterar agora os caminhos do portable Windows que já funciona.

Critério para publicar o segundo iniciador: build real em Linux, bootstrap
sem gráficos, testes de TLS/memória/importações, Vulkan, input/foco, tradução,
save/load e um percurso controlado. Até isso acontecer, o portable continua
Windows x64. A câmera adicionada nesta sessão usa SDL3 e poderá ser reutilizada.

Tentativa gráfica de 15/10/2026: scripts/build_linux_gpu_probe.sh compila
apenas a biblioteca da referência local, mantendo o snapshot fixo intacto.
Vulkan-Headers oficiais isolados em build/linux-full/Vulkan-Headers, commit
c46850864f4661461b0f6cb9922c058ffea4915e. Supera a ausência de extensões
recentes nos headers do sistema. Build em build/linux-full/gpu; logs privados
configure-current.log e build-current.log no diretório pai. A biblioteca
não é um executável Kyoei nem deve ser anunciada como port Linux pronto.

O segundo bloqueio foi Xbyak ausente no Linux. Cabeçalhos oficiais isolados
em build/linux-full/Xbyak, commit cd6bad3cad2e07a3e277c23d2b1f065320024717,
expostos somente por link em build/linux-full/gpu/include/xbyak. Nenhum
arquivo do snapshot fixo ou do projeto Godzilla foi modificado.

Resultado: biblioteca build/linux-full/gpu/libbbgpu.so compilada com saída 0,
ELF x86-64 confirmado por file. Quatro CTests Linux passaram novamente.
Não foi ligada ao runtime Kyoei, não foi executado o jogo nativo e esse
artefato de desenvolvimento não foi colocado na entrega para amigos.
