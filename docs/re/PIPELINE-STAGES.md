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

A comparação de perfil continua exigindo tamanho e bytes idênticos. Em
PerformanceStats, uma rejeição agora mostra tamanhos e até oito diferenças
por offset, com valores antigo/atual. Isso permite investigar alteração real
de capacidades versus diferenças de representação, sem aceitar um perfil
incompatível nem apagar o cache. A causa da rejeição ainda não foi demonstrada.

O snapshot do Godzilla permanece intacto: o CMake gera uma unidade gráfica
adaptada no build local, que substitui a unidade do arquivo estático pré-compilado.
