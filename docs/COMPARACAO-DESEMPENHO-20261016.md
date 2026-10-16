# Comparação histórica até 16/10/2026

Reanálise de 23 logs com séries de frame stats em native-05/logs, mais diário,
ANALISE-COMECO.md e DESEMPENHO.md. Inventário numérico privado em
`build/research/fps-history-20261016.json`. Não houve nova execução do jogo.

## Conclusão

O maior avanço técnico demonstrado ocorreu em 12/10: leitura direta de TLS,
eliminando o caminho de exceções em acessos frequentes. Há relato humano de
melhora e queda comprovada do custo da operação. Não há rota A/B que permita
atribuir um percentual de ganho de FPS a esse dia ou ranquear todos os dias
por ganho de FPS de gameplay. A meta de 60 FPS estáveis não foi comprovada.

| Dia | Evidência útil | Limite |
|---|---|---|
| 12/10 | Antes do TLS direto, diário registra trechos de 9,77–21,42 FPS. Depois, Davy relatou melhora; alguns relatórios chegaram a 33–41, ainda caindo a 19–25. Trechos de ação observados separadamente: 22,74 e 25,41 FPS. | Cenas diferentes; não calcular ganho percentual entre essas faixas. |
| 13/10 | Sessão 135113: 34,31 FPS agregados; outras sessões completas variaram bastante. | Inclui menus, transições, capturas e cenas diferentes; não é média da cidade. |
| 14/10 | Davy relatou desempenho bem melhor, com quedas. | Log 010525 sem frame stats; sem resultado numérico comparável. |
| 15/10 | Sessões noturnas: 39,84 / 29,25 / 32,91 / 31,42 / 36,16 FPS agregados. Blocos lentos próximos por ordem: antes 23,91/23,35 e depois 25,03/24,65 FPS. | Sessões/durações distintas; coleta de 231142 excessiva; blocos sem alinhamento exato de cena. Não é tendência causal nem ganho validado. |
| 16/10 | Updates cumulativos e testes dos aplicadores. | Sem nova sessão de gameplay no conjunto analisado. |

Os valores de 15/10 estão na ordem dos logs 225918, 231142, 233924, 234620 e
235349. O último, ainda sem análise detalhada anterior, contém 41 janelas,
4.920 intervalos, 136,07 segundos resumidos, média mista de 36,16 FPS e máximo
567,26 ms. Não é prova de ganho em relação ao teste 234620, mais longo.

## O que a melhoria TLS prova

Antes, aproximadamente 19–33 mil exceções por apresentação em vários trechos.
Após o caminho direto, o contador ficou zero nas partidas observadas.
Na fixture com duas threads e 10.000 leituras por thread: legado 32,37/32,74 ms
e 20.002 exceções; direto 0,02/0,10 ms e zero exceções. É forte evidência de
redução de trabalho, sem converter esse fator em aumento de FPS do jogo.

## Por que não usar a maior média como vencedor

Em 12/10, o log 205113 agrega 58,37 FPS em cerca de 851 segundos; isso não
significa gameplay quase fixo em 60. A própria sessão tem trechos de 19–25
FPS e muito tempo em apresentação leve. Em 15/10, o log 023013 agrega 59,13
FPS em cerca de 120 segundos. Esses números não tornam esses testes os mais
rápidos em ação. O mesmo problema impede afirmar 22→40 FPS comparando apenas
um trecho observado com uma sessão inteira.

As travadas continuam: nos testes recentes há intervalos máximos de cerca de
0,55–0,67 segundo. Os máximos dependem também de carga/cena e não são uma
comparação controlada de stutter entre versões.

Para medir o ganho real: mesma versão/sistema de coleta, mesmo save, percurso,
resolução e estado de cache; repetir os dois modos. O candidato mais recente
permite restaurar o polling anterior com KYOEI_POLL_EMPTY_OPS=1, sem trocar o
executável. Ainda falta essa comparação. Não foi solicitado novo teste agora.
