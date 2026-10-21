# Referencias e possibilidades do modo foto

Pesquisa20/10/2026, a pedido de Davy. Fontes oficiais; recursos dos outros
jogos servem como referencia, nao como promessa de equivalencia no Kyoei.

| Referencia | Recursos observados | Situacao no Kyoei |
| --- | --- | --- |
| Spider-Man2 | Quatro abas, precisao, lente/DOF, filtros, tres luzes gerais/spot, poses | Abas/lente/DOF/filtros/tres luzes implementados; poses dependem de animacao nativa |
| Ghost of Tsushima | Vento, clima e horario ajustaveis | Dependem de mecanismos reais de cada fase; pesquisa documentada, sem controle falso |
| Ghost of Yotei | Enquadramento, forca do filtro, foco automatico, camera animada | Enquadramentos implementados; forca do filtro viavel no processamento host; autofocus exige leitura de profundidade; trajetorias exigem estado de camera |
| The Last of Us PartI/II | Luzes, lente/DOF, avancar quadro, direcao do olhar | Luzes/lente/DOF existentes; avancar quadro depende da simulacao e jobs; olhar exige esqueleto/animacao |
| Death Stranding2 | Poses e fonte de luz personalizavel | Fonte propria existente; poses precisam API de animacao e restauracao |

## Decisoes praticas

Nesta rodada foram adicionadas guias de composicao (tercos, aurea e centro)
e enquadramentos com barras. Funcionam no host, sem depender dos scripts da
fase e sem modificar jogo/saves. Guias ficam fora da captura; enquadramento
faz parte do PNG. Previa e captura usam a mesma proporcao/centros de pixel.

Proximos incrementos viaveis: forca do filtro; reset por ajuste/categoria;
presets fotograficos; guardar enquadramento temporario durante a mesma foto.
Implementar um por vez, com correspondencia entre previa e captura. Nao inserir
logos ou assets de outros jogos. Poses, expressoes, efeitos animados e avanco
de quadro exigem pesquisa nativa; nao destravar simulacao inteira como atalho.

Antes de ampliar novamente: corrigir dicas que ainda vazam para a captura,
validar outra cena/controle fisico e concluir entrada pelo menu de pausa.
Clima/hora continuam sem prova de integracao com iluminacao real da fase201.

## Fontes consultadas

- [Spider-Man2: funcionalidades e controles](https://blog.playstation.com/2023/10/19/marvels-spider-man-2-photo-mode-features-detailed-tips-to-get-started/).
- [Ghost of Tsushima: clima, vento e horario](https://blog.playstation.com/2020/07/16/ghost-of-tsushima-photo-mode-gallery-and-tips-guide/).
- [Ghost of Yotei: lista de recursos](https://blog.playstation.com/2025/09/24/ghost-of-yotei-photo-mode-features-detailed-tips-guide/).
- [The Last of Us PartI: luzes e avanco de quadro](https://blog.playstation.com/2022/09/07/the-last-of-us-part-i-photo-mode-detailed/).
- [The Last of Us PartII: guia da Naughty Dog](https://blog.br.playstation.com/2020/10/27/o-guia-da-naughty-dog-para-o-photo-mode-de-the-last-of-us-part-ii/).
- [Death Stranding2: guia oficial](https://www.playstation.com/en-ae/games/death-stranding-2-on-the-beach/death-stranding-2-on-the-beach-guide/).
