# Gravacao Vulkan delegada: experimento local

17/10/2026. Inspiracao: separacao entre traducao PS4 e gravacao Vulkan no
Bloodborne 0.51 fornecido por Davy. Nao executamos esse port nem copiamos
seus ajustes de executavel para o Kyoei.

O Kyoei usa uma biblioteca grafica precompilada junto com unidades adaptadas.
Trocar o layout de `Scheduler` isoladamente quebraria esse contrato binario.
`src/kyoei_vulkan_recording.cpp` usa um estado externo por scheduler, preservando
o layout e as assinaturas da base fixa. Nenhum arquivo do Godzilla foi alterado.

## Contrato

- A compilacao normal exclui estes experimentos. Configurar com
  `-DKYOEI_EXPERIMENTAL_RENDERER=ON` inclui a gravacao delegada, o cache fetch e
  o temporizador preciso. Nessa previa, os tres ficam ativos por padrao;
  `KYOEI_VULKAN_RECORDING=0`, `KYOEI_FETCH_CACHE=0` e
  `KYOEI_HIGH_RES_PRESENT_TIMER=0` permitem voltar aos caminhos anteriores.
- Cada scheduler tem um unico produtor e um worker de gravacao. A ordem dos
  comandos de cada buffer continua estrita; nao ha gravacao simultanea do mesmo
  buffer nem mudanca na ordem de submissao das filas Vulkan.
- Comandos escalares copiam seus valores. Bindings, descritores, barreiras,
  constantes e attachments possuem snapshots dos arrays e estruturas usados.
  Somente os membros definidos pelo tipo de descritor sao dereferenciados.
- Blocos de 64 KiB guardam payloads e closures sem alocacao por comando.
  O produtor publica ao atingir 32 KiB ou 96 comandos; a fila aceita ate oito
  blocos pendentes. Blocos voltam ao produtor somente depois da gravacao.
- Ponteiros e extensoes `pNext` ainda sem snapshot explicito usam o caminho
  direto depois de drenar os comandos anteriores. O configure confere se todas
  as entradas `vkCmd*` do header Vulkan possuem adaptador.
- A fila e drenada antes de `vkEndCommandBuffer`, da submissao e da destruicao
  do scheduler. Esperas GPU, readbacks, fences e publicacao de resultados
  continuam com sua semantica original.
- ImGui possui uma tabela privada de funcoes Vulkan. Sua entrada direta deve
  drenar o buffer depois de `beginRendering`, antes de chamar o backend ImGui.
- Recursos referenciados continuam protegidos pelos ticks originais. Os
  snapshots possuem os argumentos de gravacao, nao duplicam texturas/buffers.

Isto e uma primeira etapa com um worker por scheduler. Nao implementa os
segmentos paralelos de dois ou tres workers por fluxo do Bloodborne, nem sua
submissao assincrona. Nao atribuir ao experimento a arquitetura completa dele.

## Verificacao e limites

`test_kyoei_vulkan_recording` confere 100.000 comandos em ordem, 100.000
snapshots com reuso de blocos, mutacao imediata dos dados originais, limites de
payload, descritores com campos irrelevantes invalidos, attachments, barreiras,
fallback `pNext`, drenagem e reutilizacao de handles/owners. Usa funcoes Vulkan
simuladas; isso nao substitui a verificacao no driver real.

Primeiro teste jogavel: 29,19 FPS/360 intervalos, maximo 60,86 ms, exit 0.
Controle em outra execucao: 28,55 FPS/480 intervalos, maximo 155,92 ms.
Versao com snapshots nos blocos: 29,40 FPS/2.760 intervalos, maximo 79,72 ms,
exit 0. Todas essas amostras ficaram sem foco. Sao execucoes separadas e nao
demonstram ganho atribuivel ao candidato. A captura da cena inicial nao mostrou
corrupcao aparente. Os numeros nao demonstram 60 FPS nem ausencia de stutter.

Comparacao posterior na mesma partida combina a gravacao com o cache direto de
instrucoes fetch previamente validado. `KYOEI_VULKAN_RECORDING_CONTROL` e
`KYOEI_FETCH_CACHE_CONTROL` aceitam, somente para pesquisa, um arquivo com `0`
ou `1`. Gravacao troca apenas na abertura de um novo buffer apos a drenagem;
fetch consulta o controle amortizadamente. Blocos de transicao precisam ser
descartados. `KYOEI_VULKAN_RECORDING_STATS=1` registra contadores no encerramento,
sem imprimir por desenho. Os controles A/B e contadores nao integram os atalhos.

## Correcoes guiadas pelo perfil e resultado

O primeiro A/B combinado mediu 28,55/28,34 FPS (1.200 intervalos por caminho).
Um perfil posterior, fora da medicao, encontrou esperas de drenagem em
`vkCmdCopyBuffer` e `vkCmdSetColorWriteMaskEXT`. Este ultimo requer
`firstAttachment` alem da contagem; o overload inicial nao correspondia a
assinatura e caia no fallback. Testes agora exigem que ambos executem no worker
e que seus arrays sobrevivam a mudancas imediatas no armazenamento original.

Com essas duas correcoes, segundo A/B: 28,75/29,78 FPS, 1.200 intervalos por
caminho. Em seguida foram incluidos snapshots das transferencias de imagens,
limpezas, blits, viewport e scissor. Essas operacoes encerravam a sobreposicao
com frequencia na versao anterior. O teste de pixel RGBA no driver real passou
tambem depois de delegar clear e copyImageToBuffer, incluindo padding e
publicacao da leitura antes do evento original.

Ultimo A/B, temporizador preciso ligado em ambos os caminhos:

| Bloco | Caminho | FPS | Intervalos | Maior intervalo |
| --- | --- | --- | --- | --- |
| 0 | original | 29,87 | 720 | 49,95 ms |
| 1 | gravacao + fetch | 40,06 | 840 | 37,31 ms |
| 2 | original | 29,39 | 600 | 104,66 ms |
| 3 | gravacao + fetch | 39,89 | 960 | 35,43 ms |

Media ponderada: **29,65/39,97 FPS (+34,8%)**, com 1.320/1.800 intervalos.
Primeiro resumo apos cada troca descartado. Mesma partida e vista inicial,
caches quentes; 197/197 verificacoes sem foco. Sem compilacao, captura ou
amostragem de pilhas durante os blocos. Exit 0. O temporizador comum nao faz
parte deste ultimo controle: nao atribuir todo o ganho ao timer ou comparar
diretamente com os numeros das primeiras variantes.

O resultado demonstra um ganho local nessa vista, sem provar 60 FPS, minimo
global, ausencia de stutter em movimento ou desempenho em outras GPUs.

Logs, saves, capturas e perfis ficam privados em `build/research`. Resolucao,
efeitos, shaders do jogo, simulacao e relogios do convidado nao sao alterados.

Teste seguinte com giro, caminhada e corrida: exit 0, 34/34 verificacoes sem
foco, capturas inspecionadas sem corrupcao aparente. O parser atingiu o marco
de 10.000 comparacoes completas sem divergencia. Verificacao duplica trabalho e
capturas perturbam o trecho: seus FPS nao medem o ganho do cache. O teste
atravessou efeitos de explosao/dano preservados, sem reduzir qualidade.

Builds com a opcao ligada e desligada passaram. O build desligado temporario
foi removido apos a verificacao; seu log permanece privado. A previa local
foi instalada no portatil pessoal, com os tres caminhos ligados por padrao,
sem incluir controles A/B. Runtime sanitizado tambem passou o teste GPU do
pixel/padding/publicacao; os dois launchers passaram `--check`. Tres arquivos
de dados/saves conferidos por hash antes/depois, identicos. Runtime anterior
continua disponivel no atualizador 20261017b para reversao.

Nenhum ZIP para amigos foi atualizado. Esta previa foi validada na RTX 2060
local; nao representa teste em AMD, Linux/Wine ou no restante completo do jogo.
