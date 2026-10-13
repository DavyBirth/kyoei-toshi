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
