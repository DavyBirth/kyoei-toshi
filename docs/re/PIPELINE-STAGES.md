# Custos de novos pipelines gráficos

No teste privado `20261016_201734.log`, o escopo inclusivo
`graphics-pipeline` chegou a 429,21 ms. Ele não mede exclusivamente o driver.

A integração agora separa três escopos, mantendo o original:

| Escopo | Conteúdo |
| --- | --- |
| `graphics-construct` | Construtor completo: layouts, preparação, chamada ao driver e nomeação |
| `graphics-driver` | Somente `createGraphicsPipelineUnique`, dentro do construtor |
| `graphics-register` | `RegisterPipelineData`: serialização e envio ao escritor do cache |

`graphics-driver` é inclusivo em `graphics-construct`, que é inclusivo em
`graphics-pipeline`. Não somar esses tempos. A chamada ao driver mede tempo
na CPU, incluindo eventual bloqueio; não mede execução de desenhos na GPU.
O escopo de driver exclui pré-carregamento (`preloading=true`), para não
consumir os registros destinados à partida antes de o jogo começar.

Cada escopo resume 120 chamadas. Cada categoria registra também as primeiras
oito chamadas de pelo menos 16,67 ms por thread, com `end_ms` monotônico e
atribuição de diagnóstico. Os registros antigos sem `end_ms` continuam aceitos.
Ausência de registro após o teto não prova ausência de pausas.

## Cache rejeitado no teste

O log informa `Pipeline cache isn't compatible with current system` antes
da partida. `WarmUp` fecha a base ao rejeitar o perfil e retorna sem carregar
pipelines. `RegisterPipelineData` retorna imediatamente com a base fechada.
Logo, nessa condição a base também não registra novos pipelines na sessão.
O blob do driver Vulkan usa outro caminho e continuou sendo atualizado.

O registro da base já usa `PipelineCacheIO` para escrever arquivos. O custo
na thread gráfica inclui serialização, preparação da tarefa e locks da fila;
não atribuir automaticamente a ele todo o tempo de escrita no disco.

O teste `20261016_211419.log` identificou diferenças somente nos offsets
66–71. Os campos do `Shader::Profile` do snapshot ocupam os primeiros 66 bytes;
a estrutura tem 72 bytes por alinhamento. A comparação anterior incluía esses
seis bytes finais de preenchimento, cujo conteúdo não representa capacidades.

A integração agora exige os mesmos 72 bytes armazenados e compara todos os
66 bytes de campos, ignorando somente esse preenchimento final. Guardas de
compilação verificam tamanho e offsets da ABI congelada. Se o snapshot mudar,
revisar a lista de campos e essas guardas. O formato salvo não muda. Um teste
com o tipo real verifica rejeição ao alterar cada um dos 66 bytes significativos,
aceitação com padding diferente e rejeição de tamanhos truncados ou maiores.

Em PerformanceStats, uma rejeição mostra tamanhos e até oito diferenças de
campos, com valores antigo/atual. Nenhuma capacidade diferente é aceita. O cache
existente permanece preservado. O pré-carregamento e qualquer melhora de
travadas após essa correção ainda precisam de validação no jogo.

Nesse último teste, os máximos nas janelas completas foram: driver 3,44 ms,
construtor 3,69 ms, registro 0,03 ms. Não houve compile stall dessas três
categorias. Portanto, o custo alto de criação visto na sessão anterior não
se reproduziu; corrigir a rejeição de perfil não explica sozinho as pausas
recorrentes. O blob Vulkan do driver já vinha sendo reutilizado separadamente.

O snapshot do Godzilla permanece intacto: o CMake gera uma unidade gráfica
adaptada no build local, que substitui a unidade do arquivo estático pré-compilado.
