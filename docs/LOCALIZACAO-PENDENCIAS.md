# Conferência das pendências em 15/10/2026

Leitura dos 173 JSONs privados, sem modificá-los: 13.096/16.100 preenchidos,
3.004 vazios. O estado salvo já tinha 12 entradas a mais que o encerramento
Claude descrito no diário. Os rascunhos de runtime somam 13.226, pois incluem
entradas locais adicionais; não misturar esses denominadores.

| Triagem reproduzível | Quantidade |
| --- | ---: |
| Cadastro provisório | 2.121 |
| Campos de modelo de notícia | 256 |
| Créditos, nomes a conferir | 320 |
| Testes numerados | 18 |
| Anotações explícitas de desenvolvimento | 7 |
| Rótulos técnicos candidatos | 3 |
| Conteúdo provisório/indisponível candidato | 44 |
| Exigem revisão individual | 235 |

Gerado com `tools/report_localization_pending.py`; índice privado em
`build/pending-audit-20261015.json`, sem prosa japonesa original no Git.

A maioria é provisória, mas não é correto dizer que o restante é só crédito
ou que está todo fora do alcance do jogador. Créditos também aparecem ao
jogador; preservar nomes originais pode ser uma decisão editorial válida,
mas não os transforma em arquivos internos.

A revisão das pendências encontrou artigos completos não traduzidos, por
exemplo `common/lua/stage_210_news_text.luc`, ID `p0/k177`, e
`common/lua/stage_207_news_text.luc`, IDs `p0/k130` e `p0/k190`.
Há também texto longo em `common/csv/reading_material.csh`, `r37/c4`.
Esses candidatos merecem prioridade sobre placeholders. Ainda falta provar
o acesso a cada um em gameplay; presença em recurso narrativo não prova que
todas as variantes são usadas.

Entre as 235 entradas, 104 estão em item_data.csh, 12 em Text_Mail.luc e 11
em reading_material.csh. Existem rótulos técnicos, testes de paginação,
instruções de desenvolvimento e conteúdo narrativo misturados. Não excluir
grupos inteiros por nome de arquivo, nem declarar 100% ajustando a contagem.

Nesta continuação, as escolhas de sexo do protagonista no rascunho local
foram revistas para “I'm a man.” e “I'm a woman.”. A prévia en-14 contém
essas mudanças e preserva byte a byte as 59 texturas anteriores. Nenhum
outro texto foi traduzido nesta continuação. Revisão humana continua pendente.
