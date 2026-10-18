# Registros de vida util dos uploads - 18/10/2026

## Problema e correcao

`StreamBuffer::Commit` mantem registros `(tick, upper_bound)` para impedir que
uma volta do buffer sobrescreva bytes ainda usados pela GPU. `current_watch_cursor`
aponta para a proxima posicao livre, mas o backend fixo consultava essa posicao
ao tentar ampliar um registro do mesmo tick. Normalmente, isso acrescentava
um registro por copia em vez de ampliar o ultimo registro existente.

Bloodborne, TLG e GOW3 usam `current_watch_cursor - 1` em `AdvanceAndWatch`.
A adaptacao local segue essa regra: `KyoeiStreamWatches::ExtendLast` amplia
somente o ultimo registro, somente quando o tick e igual. Um novo tick continua
recebendo um registro separado. O restante de `Commit`, `Map`, flush/invalidate,
rotacao das listas e esperas do scheduler permanece igual.

Isso preserva qual tick protege cada trecho, incluindo lacunas de alinhamento.
O limite do ultimo registro do mesmo tick passa a abranger todas as alocacoes
desse envio; esperar esse tick cobre todas elas. Nenhuma espera/fence e removida
antes da conclusao dos trabalhos que ela protege.

O CMake adapta uma copia de `buffer.cpp`, sem alterar a copia fixa do Godzilla,
o projeto irmao ou o layout das classes consumidas pela biblioteca prebuilt.
Integra somente com `KYOEI_EXPERIMENTAL_RENDERER=ON`, que continua OFF por
padrao no repositorio. Na previa ON, a compactacao fica ligada por padrao.
`KYOEI_MERGE_STREAM_WATCHES=0` usa a referencia conservadora de um registro por
alocacao; nao restaura a consulta incorreta a uma posicao ainda livre.
`KYOEI_STREAM_WATCH_CONTROL` permite comparar os caminhos no mesmo processo;
o controle privado e consultado amortizadamente, sem mudar os relogios do jogo.

## Provas

- `test_kyoei_stream_watches`: 100.000 alocacoes aleatorias, alinhamentos,
  mudancas de tick, crescimento das listas e 440 voltas do buffer. A referencia
  guarda todos os registros individuais. Os dois caminhos exigiram o mesmo
  tick para cada ponto consultado, incluindo varreduras durante as voltas.
  Registros: 100.000 para 5.644 no conjunto sintetico. Isso nao e uma medida
  de memoria economizada no jogo inteiro.
- Fixture GPU integrada: `KYOEI_STREAM_SELF_TEST=1` junto do teste existente
  `KYOEI_COOPERATIVE_SELF_TEST=1`, `KYOEI_COOPERATIVE_READBACK=1` e `--gpu-test`.
  Um stream pequeno, com capacidade para 16 alocacoes alinhadas ao atom size,
  abastece 2.048 copias para regioes distintas de um buffer de leitura.
  Sao 127 voltas e envios parciais a cada 37 copias. Depois da conclusao GPU,
  todos os 262.144 bytes sao comparados ao padrao esperado. PASS com e sem
  compactacao; fixture RGBA/padding/publicacao tambem PASS nos dois casos.

## Medicao no jogo

`stream-watch-ab-20261018`, RTX 2060, mesma partida e vista inicial, Fifo.
Gravacao Vulkan, cache fetch de duas entradas e timer ligados nos dois caminhos;
reaproveitamento de uploads desligado. Aquecimento de 80 segundos depois da
carga do stage201, seguido de quatro blocos de aproximadamente 25 segundos.
Controles sem compactacao nas duas pontas, para reduzir confusao com deriva
temporal. Primeiro resumo de cada bloco descartado.

| Bloco | Registros | FPS | Intervalos | Maximo |
|---|---|---:|---:|---:|
| 0 | individuais | 36,33 | 720 | 35,22 ms |
| 1 | compactados | 38,15 | 840 | 51,66 ms |
| 2 | compactados | 37,36 | 840 | 49,65 ms |
| 3 | individuais | 35,74 | 840 | 40,65 ms |

Media ponderada **36,01/37,75 FPS (+4,83%)**, 1.560/1.680 intervalos. Exit 0,
196/196 verificacoes sem foco. Sem compilacao, captura ou amostragem de pilhas
durante os blocos. Os maximos nao melhoraram: nao declarar stutter resolvido.
Resolucao interna, efeitos, shaders do jogo, simulacao e velocidade preservados.
Nao somar os percentuais das sessoes anteriores nem comparar seus FPS absolutos.
O resultado e local e nao comprova 60 FPS estaveis, outras GPUs ou o jogo inteiro.

Logs, intervalos, fontes de referencia e capturas ficam privados em `build/research`.

## Alternativa de reaproveitamento de dados, descartada

Tambem foi testado um cache de snapshots para uploads pequenos de leitura,
restrito ao mesmo tick e com comparacao integral dos bytes. Reaproveitou cerca
de 37% das consultas elegiveis, mas o ganho de quadros nao foi consistente.
Primeiro A/B, desligado/ligado/desligado/ligado: 30,40/30,18/33,31/38,55 FPS.
Repeticao com registros corrigidos e aquecimento maior, ligado/desligado/
desligado/ligado: 39,16/34,12/34,72/33,15 FPS. As medias agregadas positivas
nao bastam para atribuir ganho com essa variacao entre blocos.

Por isso, o cache, suas flags e seu alvo de teste foram removidos do codigo
mantido. Fontes e integracao ficaram no arquivo privado de pesquisa; o rascunho
final que reposiciona a captura depois da reserva de memoria nao foi compilado
nem medido. Os binarios/objetos obsoletos desse teste foram removidos depois
de conferir ausencia de processos, referencias de compilacao e existencia das
evidencias. A versao final mantem as copias originais dos dados.

## Validacao final e instalacao

`stream-watch-camera-20261018`: giro, caminhada e corrida, exit 0, 34/34
verificacoes sem foco. Capturas inspecionadas, incluindo explosao, dano,
particulas e desfoque preservados. 10.000 comparacoes completas do parser
fetch sem divergencias. Seus FPS nao medem ganho: verificacao e capturas
acrescentam trabalho.

Portatil pessoal atualizado. O executavel sanitizado instalado passou as
2.048 transferencias/127 voltas e a fixture RGBA/padding/publicacao. Os dois
launchers passaram `--check`. Tres arquivos persistentes, excluindo caches e
logs, identicos por hash antes/depois. Todos os testes mudos; audio dos atalhos
normais preservado. Nenhum ZIP para amigos alterado; portatil continua privado,
sem nova finalizacao de distribuicao. Testes encerrados.
