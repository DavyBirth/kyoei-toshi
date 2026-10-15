# Linux x64 e portable compartilhado

Pedido de Davy em 15/10/2026: manter x64; Windows e Linux na mesma pasta,
sem duplicar os dados do jogo e sem encher a raiz de ferramentas.
RX 580 + i5-9400 é um alvo para avaliação, não um mínimo já comprovado.

Estado: **ainda não existe executável Linux do Kyoei**. A preparação do ambiente
avançou em 15/10: `scripts/setup_linux_build.ps1` foi executado com elevação do
Windows, habilitando WSL/VirtualMachinePlatform com `-NoRestart`. Resultado em
`build/linux-setup-status.json`: `restart_required`. Nenhum reinício solicitado
ou realizado. Ubuntu e toolchain ainda não instalados; após reinício voluntário
de Davy, retomar o mesmo script e conferir a distribuição antes de compilar.

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
