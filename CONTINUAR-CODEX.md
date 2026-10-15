# Retomada para o Codex — 15/10/2026 (noite)

Davy pausou o Claude Code. Ler docs/DIARIO.md (últimas entradas) e docs/LINUX.md.

## Estado
- Retomada Codex: RTC e semáforos/eventos POSIX implementados em cópias de
  build; quatro testes Linux PASS. Auditoria real: 10 imports no runtime,
  418 nos módulos, 738 stubs. Ainda sem execução do jogo/GPU Linux.
- Portable Windows atualizado em build/City-Shrouded-in-Shadow-Portable (en-16,
  deltas de ícones EN/JA, arte do Return to Desktop, "Start on Linux.sh" via Wine).
- Tudo commitado (último: d3862aa). Saves de Davy preservados.

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
2. L1/R1 do menu de pausa → setas/A-D no PC. Hook de UnitMenuManager 0xb2c600
   (vtable 0x12e56c0, slot 15) testado; campo +0x178 não muda ao abrir o menu.
   Revertido. Falta achar o sinal de "menu aberto".
3. Verificar em jogo: quebra por palavras (hook 0x2e2b10), ícones novos,
   ↑ no Return to Desktop. Pedir a Davy uma cena com palavra cortada.
