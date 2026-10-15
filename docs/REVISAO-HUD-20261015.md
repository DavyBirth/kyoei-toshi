# Revisão dos testes 1 a 4 — 15/10/2026

Fontes visuais: quatro capturas fornecidas por Davy na Área de Trabalho.
Os originais permanecem fora do repositório. Provas novas ficam em build/.

| Problema | Causa/ação | Prova e limite |
| --- | --- | --- |
| `I’ m` espaçado | Apóstrofo curvo ocupa avanço japonês; normalizar pontuação latina | 455 strings corrigidas, ida/volta do delta verificada; rascunhos preservados |
| Resposta longa fora da caixa e `t` / `his` | Quebra explícita entre palavras na resposta identificada | Delta verificado; repetir visualmente esse e-mail |
| Rótulos japoneses do correio/pausa | Textos em HUD e literais do executável fora do catálogo Lua/CSH | 25 campos HUD e cinco literais; painel PM/Sky/Location/Money/Shadow Coins conferido em jogo |
| Objetivo longo acima do mapa | Texto curto equivalente para alcançar a avenida | Conferido: `Reach the main street first.` cabe na faixa |
| Objetivo inferior cruza a bússola | Elementos separados ocupam a mesma faixa | Ainda pendente; não esconder a bússola nem declarar resolvido |
| Tecla E com bordas fortes | Contorno/shadow reduzidos no gerador existente | Novo E conferido no marcador Save; avaliação visual adicional pendente |
| L1/R1 = Q/Shift na pausa | Contexto agora vem dos pixels das setas/painel realmente usados pela GPU | Setas na pausa principal, A/D em páginas internas; setas continuam disponíveis na grade |
| Ícones fixos ao iniciar | Delta de arquivo não atualizava texturas já residentes | Pares de pixels selecionados no upload GPU por entrada recente; evento SDL simulado trocou setas ↔ L1/R1 sem reiniciar |

Os ícones pequenos desenhados pela fonte ainda não foram todos identificados
no cache GPU. Há prompts originais restantes; controle Xbox ainda usa símbolos
PlayStation. A infraestrutura de seleção por entrada existe, mas não há prova
com controle físico nem um conjunto de ícones Xbox entregue.

O reconhecimento compara pixels completos, incluindo layouts lineares e
microtiles Thin1D/Display1D, dimensões com padding e subconjuntos de mipmaps.
Não muda arquivos do dump nem pixels em memória do convidado. O estado da
pausa tem uma janela de 200 ms para tolerar apresentações repetidas; Esc,
Tab, Space e Backspace limpam esse estado imediatamente.

Uma tentativa de escrever o campo adicional de overflow do medidor nativo
travou uma sessão isolada no loading. Essa escrita foi retirada. O save
voltou a carregar, comprovado em build/ui-final-pause.png. O hook anterior
continua parcial: não declarar quebra universal corrigida por testes sintéticos.

Build Windows e testes de mouse/pausa, pares de textura, quebra e correções
de UI passaram. A campanha completa, todos os menus, save/load e controle
físico continuam exigindo teste de percurso. Não há prova de 100% jogável.
