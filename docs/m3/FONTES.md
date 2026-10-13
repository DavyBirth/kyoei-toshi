# Fontes nativas e acentos

A fonte principal `common/font/utf8all_table.csh` contém 11.303 chaves.
As chaves são sequências UTF-8 de até quatro bytes, preenchidas com zeros,
guardadas em células inteiras CSH. Não são números de codepoint Unicode.
O leitor identifica página, posição e medidas nas outras oito colunas.
Interpretamos essas medidas por inspeção do atlas; comportamento do renderer
ainda exige confirmação no jogo.

Todos os 153 caracteres necessários aos rascunhos, ASCII, acentos e pontuação
estão na tabela principal. A folha privada de glifos confirmou visualmente
os acentos do português. O atlas principal é uma máscara R8 de 2048×2048,
com pixels lineares de um canal, não BC3.

As sete variantes em `common/font` e a fonte de sistema `font12color2`
faltam 52 caracteres desse conjunto: letras latinas acentuadas e traços
tipográficos. Isso inclui os acentos usados em português. IM significa o nome
do conjunto de arquivos; a aplicação exata de cada variante requer rastrear
qual fonte é carregada por cada tela. Não substituir todas as fontes sem essa
verificação, nem remover acentos da tradução para contornar o problema.

Próximos passos: identificar o seletor de fonte no HUD, verificar dimensões e
estilo das variantes usadas, adicionar glifos preservando os existentes e
validar tabela/atlas sem mudanças e com alterações, depois testar no jogo.
As fontes de apoio NotoSans/NotoSansCJK também contêm os caracteres conferidos;
isso não preenche automaticamente os atlas nativos.

Relatórios: `fallback-coverage.json`, `game-font-coverage.json` e
`native-glyph-sheet.json`. Atlas e folhas completas ficam fora do repositório.
