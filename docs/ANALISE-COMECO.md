# Primeira fase: observação sem outro jogo aberto

12/10/2026. Davy jogou o começo e pediu análise até morrer. Executável
dee729ed8fd2d1cf6be007ef63b2ee9ec2faa967c6d961380ee3a68266b93fb2,
PID 33432, registro 20261012_211713.log. Readback linear e TLS direto ativos.
Davy informou Abiotic Factor fechado; nenhuma configuração foi alterada
durante a coleta. Não foi feita comparação com a mesma rota em outro build.

## Resultados de desempenho

| Intervalo observado | Quadros resumidos | FPS agregado | Menor/maior janela de 120 intervalos | Maior intervalo |
| --- | ---: | ---: | --- | ---: |
| 21:24:01–21:24:31 | 600 | 22,74 | 13,69–28,69 FPS | 1.849,97 ms |
| 21:26:30–21:27:01 | 720 | 25,41 | 19,00–28,80 FPS | 784,11 ms |

Horários locais, America/Bahia. Valores vêm de apresentações concluídas do
jogo, em relatórios de 120 intervalos, sem redesenhos ociosos. Não são FPS
instantâneos nem captura bruta de todos os quadros. Cena, transições e
capturas podem influenciar pausas; não atribuir cada pausa a um gargalo sem
trace. Médias de 60 FPS no menu após a morte não representam gameplay.

CPU consumiu em média o equivalente a 1,80/1,83 núcleos lógicos ativos
nesses intervalos, em máquina com 16 processadores lógicos. Isso não indica
que nenhuma thread esteja saturada. Memória residente: aproximadamente
8,41/8,44 GiB; duas amostras não provam vazamento.

Três leituras pontuais do contador GPU 3D no segundo intervalo: 36,2%,
33,6% e 11,5%. São instantâneos, não média contínua da GPU ou da placa toda.
Não houve evidência de saturação constante nessas amostras; isso não basta
para fechar diagnóstico CPU vs GPU.

PrepareFrame ficou em média perto de 0,16–0,18 ms e Present 0,39–0,43 ms.
Um relatório de leitura linear no segundo intervalo teve média 11,65 ms,
máximo 71,11 ms. O caminho é um custo relevante a investigar, mas soma de
relatórios de threads diferentes não equivale ao tempo crítico do quadro.
Contador de exceções TLS ficou zero nos intervalos.

## Visual e estabilidade

Quatro capturas privadas em build/perf-clean-start.png e perf-clean-02/03/04.png:
rua com NPCs e minimapa; close de diálogo legendado; rua com fogo/partículas;
cena do gigante. Imagens inspecionadas: sem retorno evidente do desfoque
global, sem texturas ausentes ou geometria corrompida evidentes nessas
capturas. A névoa/tonalidade do fogo não foi comparada quadro a quadro ao PS4.
Capturas estáticas não certificam ausência de flicker durante movimento.

Não apareceram STOP/assert/falhas de mapeamento nos intervalos analisados;
o processo continuou aberto depois da morte. Antes desta coleta Davy já
confirmou Continue, título e botões funcionando. Missão inteira ainda não
foi certificada; morte é o limite desta observação.

## Textos desse começo

O índice local identificou 749 candidatos em common/lua/Text_Stage_201.luc,
principal recurso de textos da fase. Não confundir com stage_201.luc (3),
stage_201_demo.luc (1) ou stage_201_npc.luc (13): esses scripts têm poucos
textos embutidos e não substituem a tabela de diálogos.

Objetivos e marcadores observados correspondem a common/lua/map_goal_text.luc,
IDs p0/k15, k17, k19, k21, k34, k36, k38, k40 e k42 como candidatos para
conferir. Primeiro piloto: objetivo de chegar à rua principal; depois
objetivo de fuga e marcação do encontro, seguido por 20–30 diálogos/avisos
curtos desse trecho. Escolhas de relação/nome podem alterar variantes.

Nenhuma tradução foi aplicada nesta partida. Antes de patch: verificar
recurso/ID, substituir uma frase em saída de teste separada e conferir
fonte, largura, contexto e layout no jogo. Originais completos continuam
fora do repositório.

Dados privados: build/perf-clean-sample-01/02.json (processo),
perf-clean-metrics-01/02.json (apresentações) e perf-clean-session-summary.json.
O resumo da sessão inteira inclui abertura, reload e menus, portanto não
deve ser usado como média da cidade.
