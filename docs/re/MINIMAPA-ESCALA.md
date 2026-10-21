# Escala do minimapa — investigação de 20/10/2026

Relato de Davy: ao mudar a resolução, vida e stamina diminuem, mas o minimapa
continua grande. Causa ainda não confirmada; nenhuma alteração de runtime ou
prova visual nesta etapa.

## Saída e medição de texto

`scripts/run_native.ps1` passa Width/Height em `KYOEI_WINDOW_WIDTH/HEIGHT`.
`src/kyoei_window_options.h` lê essas opções e a integração em
`CMakeLists.txt` as aplica a `SDL_CreateWindow`, registrando tamanho em pixels.
Esse caminho configura a saída; não demonstra mudança de resolução interna.

`src/kyoei_hud_trace.c`, `measure_map_text`, adapta a medida de textos nos
sites `0xa5bff0` e `0xa5f4a5`. Não reduz o conjunto do minimapa nem oferece
uma escala dependente da resolução.

## Estrutura do executável próprio

ELF do disco CUSA03562 01.03, SHA256
`1968814b70f69ec85ff3e556c4b66b53ec298c0dea71120e4f0ba594906e418e`.
Endereços relativos à imagem conferidos com llvm-objdump. Os ponteiros ativos
da partida ainda não foram observados.

| Rotina/site | Observação estática |
| --- | --- |
| `0xa599c0` | Início com prólogo completo; guarda RSI em RBX e lê `[RBX+0x40]` em R14 em `0xa59a01`. |
| `0xa59ab5` | Carrega `common/hud/cockpit/map/cockpit_map`; resultado em `[R14+0x10]`. |
| `0xa59ae1` | Carrega recurso alternativo wide; resultado em `[R14+0x20]`. |
| `0xa59a27/0xa59a56/0xa59a85` | Cria objetos via `0x355db0`, guardados em `+0x18/+0x28/+0x30` de R14. Tipo e posse ainda pendentes. |
| `0xa59b2b/0xa59e6d` | Chamadas de `0x2f05a0` com objeto e recurso normal/alternativo. |
| `0xa5a560` | Início confirmado de função; lê `[RDI+0x40]`, verifica byte inicial e compara modo local com valor derivado de `0x13bda60`. |
| `0xa5a52d/0xa5a601` | Chamadas de `0xa5fc20` com modo em ESI e EDX=1. |
| `0xa5fc20` | Compara ESI com `[RDI+4]`; caminhos 0/1/2, imediatos ou com transição conforme DL. Usa objetos `+0x18/+0x28` com subobjetos `+0x80`. |
| `0xa5ff67` | Persiste modo em `+4/+8/+0x19b630`; limpa flag `+0x19b62c`. |

Modos e recurso wide não provam relação com resolução de tela ou com o
defeito. Não forçar `0x13bda60` sem identificar origem e outros consumidores.
O subobjeto `+0x80` usado em transições não demonstra que escrever um float
ali redimensione corretamente conjunto e recorte.

Relatório privado: `build/minimap-scale-references-20261020.json`, gerado com
`tools/inspect_elf_references.py`. A busca de CALL direto para `0xa599c0`
não encontrou candidatos; entrada indireta continua possível. Buscar rótulos
curtos não cobre LEAs para o início dos caminhos completos. Ausência de
candidatos não prova ausência de uso.

## Próxima prova

1. Capturar a mesma cena, idioma e estado do mapa em 1280×720 e 1920×1080,
   registrando resolução interna. Testes do agente mudos e com saves privados;
   preservar qualquer partida já aberta.
2. Medir mapa e barras como fração da área efetiva do jogo, descontando bordas.
   Distinguir saída, tela cheia, resolução interna e modo normal/wide.
3. Rastrear origem de `0x13bda60`, transformações de `+0x18/+0x28` e recorte.
   Identificar conjunto com fundo, ícones, objetivos e máscara antes de propor
   escala opcional restaurável.
4. Comparar com referência PS4/shadPS4; verificar legibilidade, alinhamento e
   transições antes de habilitar correção.
