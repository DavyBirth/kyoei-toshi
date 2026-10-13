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

## Evidência da partida de Davy — 13/10, 12:28–12:35 Bahia

Davy abriu o jogo pelo launcher, em inglês, 1280×720, diálogos internos
habilitados; PID observado 28332. O agente não iniciou nem reiniciou a partida.
Capturas privadas em build/minimap-user-20261013-01.png e -02.png, obtidas
com capture_native.ps1 sem -Focus e sem enviar controles.

A primeira captura confirma a interação truncada como `Listen to t…`, apesar
da placa branca continuar à direita do texto. O objetivo acima do minimapa
também está cortado à direita. A diferença entre largura da placa e limite
do texto reforça investigar clipping/limite do nó, além do fundo da caixa.
O rótulo `Park: meet your friend` aparece separado da seta na borda direita;
esta imagem parada não prova a causa nem o comportamento com câmera em movimento.

A segunda captura mostra a fala de `Man sheltering from the rain` sem corte
visível no nome. Davy confirmou que a ação `Peek inside` pertence a esse alvo.
O verbo original de p0/k30 não especifica um interior: rascunho EN corrigido
para `Take a closer look`, com nota sobre outros usos compartilhados. Validação
de IDs, tokens e reinserção EN PASS. Pacote da partida permanece intacto;
correção textual ainda não publicada nem observada no runtime.

Davy relatou desempenho pior. Não há compiladores concorrentes na observação,
e o log atual não contém medições de FPS comparáveis. A composição do overlay
é condicionada a modal ativo no presenter; não executa passagem de desenho
durante caminhada sem diálogo. Isso não mede custo total nem exclui regressão.
Próximo diagnóstico: comparação com frame stats na mesma cena/configuração,
quando houver uma próxima abertura autorizada, incluindo diálogos ligados e
desligados. Não atribuir a perda à tradução sem evidência.

## Caminho de truncamento encontrado após as capturas

Desmontagem em fronteiras de função confirma `0xa6d9a0`: conta caracteres
com `0x31cd90`; se comprimento >= 13, copia os primeiros 11 e acrescenta
U+2026, literal em `0x10c7911`. O resultado para a tradução da captura é
exatamente `Listen to t…`. Depois atribui o texto ao nó armazenado em
objeto+0x278, string em nó+0x110. A rotina usa contagem para dimensionar
outro nó em objeto+0x238: valor em nó+0x80 = n*0,80555558+0,88888890;
quando truncado, n=12. Isso demonstra corte anterior ao renderer e cálculo
por quantidade, não por advances da fonte. A associação à captura é forte,
mas ainda não foi confirmada por trace da chamada na partida.

Chamadores candidatos `0xa1de11` e `0xa6d2cf`: próxima etapa é verificar
instruções e ligar o nó à carga do recurso, antes de alterar comportamento.
Não mudar globalmente `0x31cd90`: é utilitário compartilhado de strings.
Uma correção local precisa preservar limites de tela e medir placa/texto
pela fonte real, além de retirar o corte antecipado. Dump e partida intactos.

Outros consumidores de text_marker: LEAs confirmadas em `0xaea76c` e
`0xb025b1`; o primeiro tem outro limite, >=9 corta para sete + U+2026.
Logo não há um único limite global para todas as caixas. Relatório técnico
privado: build/interaction-hud-references.json. Não reutilizar números de
uma rotina como correção universal.

Ligação ao recurso confirmada em seguida: LEA `0xa6cf3b` carrega
`common/hud/cmnd_marker/cmnd_marker`, chamada `0xa6cf47` -> `0x350e40`.
O consumidor atribui o nó índice 0 a objeto+0x238 (base), índice 1 a
objeto+0x230 (pino) e índice 2 a objeto+0x278 (texto). Após criar/ligar os
nós, `0xa6d2cf` chama a rotina de truncamento `0xa6d9a0`. O recurso contém
exatamente esses três nós técnicos. O outro chamador `0xa1de11` atualiza o
mesmo objeto recebido do controlador. A ligação está demonstrada no código;
a execução específica ainda não foi registrada por trace.

Restante antes da correção: identificar a escala/métrica de fonte efetiva
e o limite de tela. Advances das duas tabelas para o texto inteiro são
481/518 unidades; para os 11 caracteres, 179/196. Esses valores diferem
da contagem multiplicada por largura fixa. Não usar a escala deduzida de uma
captura como medida geral para todas as resoluções.

Diagnóstico opt-in preparado: src/kyoei_hud_trace.c, somente com
KYOEI_HUD_TRACE=1. Assinatura FNV da rotina completa 0x1da bytes é
0x28c6cdeef5969e0d. Relay SysV preserva prólogo, argumentos e retorno;
original executa sem alterações de texto/layout. Registra somente contagens,
geometria e campos técnicos da fonte nas primeiras 24 atualizações; nenhuma
fala original vai no log. Desabilitado por padrão; assinatura diferente
faz o diagnóstico ser ignorado. Teste sintético de relay e instalação
verificada no bloco real PASS, sem executar código do jogo.

Teste oferecido a Davy: Testar HUD EN.cmd (EN 1280×720, diálogos internos,
frame stats e HUD trace). O agente compilou e executou apenas -CheckOnly.
O teste visual ainda deve confirmar escala/fonte e medir fluidez. O atalho
recusa iniciar com outra partida aberta; não fecha nenhuma partida.

Minimapa também tem cálculo por contagem: `0xa5bff0` conta a string do nó,
`0xa5c028` multiplica pelo campo nó+0x19c e constante; depois usa a extensão
em clamps contra as bordas, escrevendo posição em +0x60/+0x64. Outro caminho
em `0xa5f4a5` multiplica contagem pelo mesmo campo para calcular a placa.
Isso aponta um mecanismo concreto para investigar a posição deslocada com
latim; ainda não prova que toda diferença ao girar é defeito. O diagnóstico
da interação registra também +0x19c e os campos vizinhos para identificar
tamanho/seletor antes de substituir contagem por advances efetivos.

Primeiro runtime observado no teste de Davy (log 20261013_130354): rótulo
Listen, seis caracteres, scale_x=5,7222, base x=24 e texto x=50. O limite
de 24 eventos foi atingido repetindo o mesmo rótulo. Corrigida a seleção dos
registros para mudanças de conteúdo/nó; hashes somente, sem texto no log.
Setter de string 0x31e4a0 confirma comprimento em bytes em +8 e dados em +0,
com buffer inline +0x10 para strings curtas. Diagnóstico usa esse comprimento
com guardas em vez de VirtualQuery por caractere. Parte de fonte +0x19c
preparada para uma próxima execução; não trocar binário durante a partida.
