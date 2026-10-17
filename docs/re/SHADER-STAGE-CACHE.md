# Reuso da especializacao de shaders

Experimento de 17/10/2026. Meta de 60 FPS ainda nao atingida.

O perfil de CPU local encontrou custo recorrente em `StageSpecialization`
e `ParseFetchShader` no caminho de cada desenho. O adaptador de
`vk_pipeline_cache.cpp` pode reaproveitar o indice de uma permutacao ja
encontrada, sem reaproveitar os recursos vinculados ao desenho.

## Contrato

- `Info::RefreshFlatBuf` continua executado antes da consulta. Enderecos,
  buffers, texturas, samplers, uploads e barreiras continuam atualizados.
- O cache por thread tem oito entradas, identificadas por PipelineCache,
  Info e epoca. Confere runtime, bindings iniciais e numero de modulos.
- A chave limitada a 4096 bytes projeta os campos lidos pelo construtor
  fixo de `StageSpecialization`: nulidade; stride/storage/formato/swizzle
  de buffers; tipo/classe/conversao/storage/bindings de imagens; dimensoes
  de fmask; modos de coordenadas e degamma de samplers. Atributos fetch
  preservam formato e selecao de componentes, alem de sua descricao.
- O codigo fetch e comparado antes e depois da leitura de recursos.
  Snapshot limitado a 512 bytes dentro da mesma pagina de 4096 bytes;
  casos fora desse limite usam o caminho original.
- Uma primeira observacao fetch apenas aquece o snapshot. Observacoes
  iguais precisam envolver o construtor original para confirmar a entrada.
- Inicializacao, carregamento e substituicao de shaders invalidam a epoca.
  Tessellation usa sempre o caminho original por ler constantes adicionais.
- Excesso de capacidade, chave grande ou entrada alterada resultam em
  recalcular pelo caminho original. Nenhum efeito e desativado.

## Validacao e reproducao

`test_kyoei_stage_cache` usa o construtor real para 12.000 mutacoes de
recursos, incluindo campos relevantes e irrelevantes, nulidade, runtime,
bindings, proprietario, modulos, epoca, codigo alterado e expulsao LRU.
O parser fetch sintetico desse teste nao substitui a verificacao no jogo.

Variaveis de ambiente, lidas no inicio do processo:

| Variavel | Efeito |
| --- | --- |
| `KYOEI_STAGE_CACHE=1` | Habilita o caminho experimental; padrao desligado. |
| `KYOEI_VERIFY_STAGE_CACHE=1` | Executa tambem construtor/busca originais e confere toda previsao. |
| `KYOEI_STAGE_CACHE_AB=1` | Alterna caminhos antigo/novo a cada 20 segundos para coleta privada. |

O modo de verificacao acrescenta trabalho e nao mede o desempenho do
caminho rapido. No A/B, descartar o primeiro resumo de 120 intervalos apos
cada troca e blocos incompletos; conferir foco, cena e atividades concorrentes.
Capturas e amostragem com suspensao de threads nao devem ocorrer dentro
do trecho usado para comparar FPS.

O primeiro candidato normalizado mediu 29,90/31,89 FPS na mesma vista,
com quatro blocos completos; foco variou em um bloco. A variante com 64
entradas nao demonstrou beneficio e foi descartada. Evidencias privadas
ficam em `build/research`; resultados seguintes estao em `docs/DIARIO.md`.
