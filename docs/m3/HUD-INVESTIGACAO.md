# HUD: fontes, largura e ancoragem

Investigação offline de 13/10/2026. Nenhum jogo executado.

Davy relatou que caixas de interação próximas aos personagens mantêm o
tamanho usado em japonês e cortam algumas traduções inglesas com `...`.
Isso é distinto das reticências escritas deliberadamente nas falas. Investigar
medição com a fonte efetiva, largura, quebra e crescimento da placa, sem
encurtar traduções ou remover reticências narrativas indiscriminadamente.

O recurso Lua `hud_data.luc` contém os identificadores `COCKPIT_MAP` e
`MAP_ROTATION`. A constante `map_goal_text.luc` contém o conteúdo do objetivo,
mas não demonstra por si só como o motor mede ou posiciona o texto.

Há duas versões candidatas de minimapa na extração privada:
`common/hud/cockpit_map.hud` e `common/hud/cockpit/map/cockpit_map.hud`.
Não assumir que a primeira versão encontrada é a usada pelo jogo.

A segunda contém os nós técnicos `way_goal_iconG`, `way_goal_textG`,
`navi_textG`, `text_plate_goal_lG`, `text_plate_goal_cG`,
`text_plate_goal_rG` e `text_plate_textG`, além de máscaras laterais.
A presença separada de placa e texto é um ponto para investigar crescimento
da placa junto com a medição do rótulo e transformações herdadas.

Os HUDs examinados têm magic `00 64 75 68`, versão 2 e BILZ no offset 128,
com zlib depois do offset 144. O payload é diferente de CSH e Lua; não deve
ser enviado ao reinseridor de textos. Os identificadores encontrados são
precedidos por comprimentos little-endian. Campos numéricos próximos ainda
não têm semântica demonstrada. Não editar floats ou IDs por tentativa.

A varredura ASCII de 149 HUDs não encontrou nomes de atlas `utf8all`.
Portanto o seletor pode ser numérico ou resolvido pelo executável; a ausência
de um nome literal não prova a fonte usada nem justifica substituir variantes.

Próximo passo: identificar os registros/propriedades do nó de texto e mapear
font ID, limites, clipping e alinhamento contra seus consumidores no motor.
Localizar também o HUD da interação. Reproduzir o cálculo offline antes de
alterar dados. Fonte, ancoragem e truncamento visual permanecem pendentes.

## Desmontagem do executável próprio

ELF preparado: native-05/out/eboot.elf, SHA256
`1968814b70f69ec85ff3e556c4b66b53ec298c0dea71120e4f0ba594906e418e`.
Endereços abaixo são relativos ao eboot, não endereços de uma partida.

- `0x6dc030`: rotina com cargas de `common/font/utf8all` e
  `common/font/utf8all_IM`. LEAs em `0x6dc062`, `0x6dc081`, `0x6dc0bf`,
  `0x6dc0de`; alvos `0x109c51e` e `0x109c532`.
- Após essas cargas, chamadas virtuais no slot `0x38` recebem respectivamente
  ECX=0 e ECX=1. Possível índice de registro de fonte: hipótese, ainda não
  prova de que o objetivo seleciona o índice 1.
- `0xa59aa9`: LEA da string `common/hud/cockpit/map/cockpit_map`, seguido
  de chamada `0x350e40` em `0xa59ab5`. Há caminho também para a versão wide.
  Isso confirma um consumidor da versão da subpasta, sem certificar qual
  ramo foi usado na partida de Davy.

Essas LEAs foram conferidas com llvm-objdump. O utilitário
`tools/inspect_elf_references.py` salva candidatos em `elf-hud-references.json`;
varredura de bytes sozinha não prova fronteiras de instrução. Trecho privado
adicional: build/hud-map-init.disassembly.txt; não versionar desmontagem completa.

`tools/measure_translation_layout.py` calcula somas de advances nativos para
as 130 entradas EN contra as duas tabelas. Relatório:
`english-layout-estimates.json`. Sem escala, kerning, clipping, quebra automática
ou fonte efetiva do HUD, esses valores são estimativas em unidades do atlas.
Controles de nome/ícone ficam marcados como largura desconhecida.

Exemplos: p0/k18 cresce 1,266× na principal e 1,439× na IM; p0/k57 cresce
1,382× e 1,625×. Outras frases inglesas encolhem. Portanto contagem de letras
ou aumento fixo uniforme não basta para ajustar as caixas. Próxima correção
deve medir texto/fonte efetivos e crescer placa/quebrar dentro da área útil.
