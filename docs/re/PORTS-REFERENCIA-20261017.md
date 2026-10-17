# Comparacao dos ports fornecidos por Davy

17/10/2026. Inspecionados os seis arquivos de fontes em `third_party`.
Pacotes executaveis nao foram executados. Fontes/documentacao extraidas
para `build/research/reference-sources`, sem copiar dados de jogos.
Inventario e hashes: `ports-reference-inventory.json`.

## Diferencas que importam para o Kyoei

| Projeto | Evidencia nas fontes | Comparacao com Kyoei |
| --- | --- | --- |
| Bloodborne 0.51 | `gpu/shadps4/video_core/renderer_vulkan/vk_scheduler.cpp`: gravacao de comandos Vulkan em 2 ou 3 workers por padrao, segmentos de aproximadamente 64 KiB e submissao assincrona. | Nossa base grava os comandos na mesma thread que traduz o fluxo PS4. Esta e uma diferenca estrutural importante; requer integrar gravacao, ownership dos payloads e submissao, nao apenas ligar uma flag. Nao foi medido quanto dessa mudanca resolveria aqui. |
| Bloodborne 0.51 | `gpu/shadps4/shader_recompiler/frontend/fetch_shader.cpp`: cache por endereco, validacao dos bytes e limite de 4096 entradas. | O parser herdado decodifica novamente em cada desenho. Candidato local equivalente em principio foi verificado com camera/corrida, mas nao resolveu 60 FPS. |
| God of War III 2.4.1 | `gpu/shadps4/video_core/buffer_cache/buffer.cpp`: stream buffer em RAM cacheada por padrao; documenta custo de escritas pelo BAR em NVIDIA. | Kyoei prefere memoria de dispositivo. Teste confirmou flags reais 0x7 no caminho antigo e 0xe na RAM cacheada. RAM cacheada mediu 30,00 FPS contra referencia 29,90; nenhum ganho util comprovado neste PC. |
| Gravity Rush 2, 3159679 | `port/gpu/shadps4/video_core/buffer_cache/buffer.cpp` e `texture_cache/image.cpp`: mesma escolha de RAM e memo da ultima transicao parcial. | Nossa base nao tem essa memo de subrecursos: pode repetir percursos de mip/layer. Sua relevancia precisa ser medida; nao remover barreiras de escrita. |
| The Last Guardian, 3dd0230 | Mesmas melhorias de memoria/transicoes; `docs/60fps.md` identifica flip rate e ajuste especifico do filtro de delta do jogo. | Esses patches de executavel nao se transferem ao Kyoei. O Kyoei ja solicita flip rate 0. Isso sozinho nao prova que todas as esperas internas estejam corretas. |
| inFAMOUS Second Son, 1898d44 | `port/gpu/shadps4/video_core/renderer_vulkan/vk_pipeline_cache.cpp`: reaproveitamento por programa, tentativa da ultima variante e verificacao opcional. Readbacks diferidos e submissao parcial documentados. | Nosso cache global experimental e diferente: oito entradas e reconstrucao de chave de recursos. Foi caro e nao deu ganho relevante. O comparador de especializacao corrigido dessa familia ja coincide com o da nossa base: nao e uma correcao faltante a reaplicar. |
| P.T. 1.0.5 | `README.md`: logica reimplementada em C++ e renderer proprio, lendo os dados originais em runtime. | E a arquitetura de destino do M6 em diante. Seu desempenho nao demonstra que carregar o executavel PS4 no backend atual deva ter custo equivalente. |

## O erro de prioridade desta investigacao

Os ajustes recentes concentraram esforco em evitar pequenas chamadas e em
construir caches auxiliares, enquanto a gravacao Vulkan e a traducao continuam
concentradas numa thread. A comparacao com Bloodborne torna essa diferenca
explicita. Mais acertos de cache nao significaram mais FPS nas coletas.

O proximo trabalho deve medir e atacar o custo ativo desse caminho serial,
incluindo gravacao/submissao e ownership dos recursos. Copiar isoladamente o
scheduler novo nao funciona: suas chamadas `Record` e os dados que capturam
precisam estar integrados aos demais componentes. Alterar o layout das classes
sem recompilar os consumidores do backend prebuilt quebraria o ABI.

As outras melhorias sao candidatas especificas, com custo e resultado local
registrados, nao uma justificativa para outra rodada indiscriminada de flags.
Resolucao, efeitos, simulacao e resultados dos readbacks devem ser preservados.

## Resultados desta rodada

Referencia em primeiro plano: 29,90 FPS / 1080 intervalos. GPU global 36-38%
na coleta posterior; nao identifica sozinha a causa do limite. Perfil de
1000 pilhas da thread grafica: 343 em espera de label e 586 em Draw.

O cache direto de fetch passou 10.000 mutacoes sinteticas e comparacao real
de todos os campos dos atributos, incluindo giro/corrida, sem divergencias.
A/B posterior na mesma partida: antigo 26,29 FPS, novo 27,12, 840 intervalos
por caminho. Foco ausente em quase todos os checks e blocos variando de
24,44 a 27,87 FPS: ganho insuficiente e coleta variavel; nao prova 60 FPS
nem resolve stutter. A primeira tentativa de A/B encerrou ainda no menu e
nao produziu amostra utilizavel.

Nenhum dos ports fornecidos foi testado neste PC nesta analise. Seus relatos
de desempenho nao foram tratados como resultados do Kyoei.
