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

## Auditoria de páginas diretamente no PSARC

`tools/audit_font_pages.py` lê somente o PSARC indicado pelo M0 e gera relatório
privado novo em build/. Não extrai pixels, altera fontes ou reconstrói pacotes.
Metadados revisados em `font-pages-audit.json`: oito tabelas, 21 páginas TEX,
todas R8 linear de 2048×2048 e um nível de mip. Hashes das oito tabelas coincidem
com a auditoria anterior. A pasta parcial de inspeção tinha só as páginas 0;
isso não representa todos os atlas presentes no disco.

Principal e IM referenciam páginas 0/1/2 e possuem os arquivos correspondentes;
todos os retângulos lidos dessas seis páginas estão dentro dos limites. As
outras variantes usam entre uma e 24 páginas nas tabelas, sem TEXs de mesmo
prefixo em suas subpastas. Não presumir compartilhamento com os atlas raiz
nem selecionar variantes por nome. Há 15 páginas de nome utf8all e seis IM
no arquivo: não apagar as adicionais por não aparecerem nessas duas tabelas.

ASCII e os seis glifos de pontuação habilitados em FitHud ficam na página 0
das duas fontes. Esta auditoria não prova qual fonte a tela de objetivo usa,
não adiciona acentos e não certifica pixels de páginas ainda não inspecionadas.
