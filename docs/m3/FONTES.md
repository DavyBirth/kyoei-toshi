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

## Caminho estático do índice de fonte no nó bitmap

ELF próprio com hash 1968814b70f69ec85ff3e556c4b66b53ec298c0dea71120e4f0ba594906e418e.
Conferido com llvm-objdump, sem executar instruções do jogo:

- registro do tipo GxUnitFontBmp em 0x2e4c69; fábrica 0x2e4cd0 aloca 0x1f0
  bytes e chama 0x2e5230, que usa construtor base 0x2e4570;
- base inicializa string em +0x110 e tamanho float 24 em +0x19c;
- desenho copia nó+0x1a8 em 0x2e5aeb para parâmetros+0x10. Chamada 0x2e5b9e
  passa esse bloco a 0x2de990;
- consumidor lê parâmetros+0x10 em 0x2de9f6, exige índice <=3 e usa stride
  0x100 na estrutura do gerenciador. Este é o caminho de seleção do font slot,
  separado do tamanho +0x19c e dos outros campos +0x1a0/+0x1a4.

HUD trace agora registra font_index_1a8 com leitura protegida, somente quando
o diagnóstico está ativo. O registro anterior de oito palavras em +0x188
terminava em +0x1a4, portanto não incluía esse campo. Referências técnicas
privadas em build/font-selector-references-01.json e trecho limitado em
build/font-bitmap-node-disassembly.txt; não versionar desmontagem completa.

A seleção concreta do nó do objetivo ainda precisa ser ligada ao recurso HUD
ou observada em runtime. Não concluir pelo tamanho 38 que a fonte é a principal,
nem substituir atlas ou remover acentos. Referências de campos em outras
classes não são evidência do font slot deste nó.

## Tipos serializados do objetivo — 14/10/2026

HUD privado `common/hud/cockpit/map/cockpit_map.hud`, SHA-256
`c94095095aa27488279e43450f8af00ca5c9263ce98e951b59e888d8e200227d`.
Payload BILZ: 11971 bytes. Sondagem reproduzível:

```powershell
python tools/inspect_hud_node_types.py G:/kyoei-toshi-work/m1/common/hud/cockpit/map/cockpit_map.hud --label way_goal_text --label navi_text --type GxUnitFontBmp --type UnitFontEx
```

- way_goal_text: offset 3716, comprimento 13, tipo 0x86aac3f5 (UnitFontEx).
- navi_text: offset 5557, comprimento 9, tipo 0x0518af9e (GxUnitFontBmp).
- G aparente após os nomes nas buscas ASCII pertence ao campo seguinte.

Hash de tipo ASCII em 0x302ff0: acumulador 37, para cada byte
`h = (h * 37 + byte) & 0xffffffff`. Não há multiplicação final. Propriedades
usam outro caminho (0x302f60), com multiplicação adicional e XOR do descritor.

Registro UnitFontEx em 0xae8709/0xae875b: tamanho 0x3c0, descritor 0x153e670,
pai 0x136f620 (GxUnitFontBmp). Fábrica 0xae95d0 chama construtor 0xae9a60,
que chama bitmap 0x2e5230 e instala vtable 0x12e1b28. O construtor derivado
inspecionado não muda +0x1a8 diretamente, inicialmente zero na classe base.
Propriedades e atualizações posteriores ainda podem selecionar outro slot.

Propriedade bitmap +0x1a8 registrada em 0x2e4dce chama-se `_index`:
referência 0x2e4df8, string técnica 0x1017501. Próximo passo: rastrear
propriedades herdadas e overrides UnitFontEx ou observar índice em runtime
autorizado. Sonda relata candidatos estruturais; nenhum atlas alterado.
