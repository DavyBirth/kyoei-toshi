# Processamento dos lotes gráficos

O escopo `command-batch` envolve o laço `while (num_submits || num_commands)`
de `Liverpool::Process`. A fila pode receber novos envios durante esse laço.
Portanto, 492 ms nesse escopo não demonstram que um envio ou desenho levou
492 ms, nem que a GPU ficou ocupada por esse tempo.

A integração do Kyoei acrescenta `CommandBatch` na pilha do worker, fora das
corrotinas. Com `PerformanceStats`, os primeiros oito lotes de pelo menos
50 ms por thread registram:

- `milliseconds`: duração do laço inteiro.
- `resume_ms`: soma dos tempos das chamadas externas de `task.resume()`.
- `max_resume_ms`: maior chamada externa de retomada.
- `resumes`, `completed`, `yielded`: chamadas e resultado `task.done()`.
- `empty`: visitas a filas sem tarefa disponível.
- `end_ms`, `diagnostic`: relógio monotônico e sobreposição com diagnóstico.

As retomadas externas são sequenciais. A soma inclui esperas e callbacks
executados dentro delas. Uma tarefa que cede a vez pode ter feito trabalho
útil antes; `yielded` não mede tempo ocioso. Retomadas internas de corrotinas
filhas entram no tempo da chamada externa, sem contagem separada.

Diferenças entre duração total e soma das retomadas incluem callbacks fora
delas, locks, visitas a filas, contabilidade e agendamento do sistema. Não
rotular essa diferença automaticamente como contenção ou tempo ocioso.
Os valores não são timestamps da GPU e não devem ser somados aos escopos
inclusivos já existentes. Tempos arredondados têm pequena diferença numérica.

Depois de oito registros, a coleta detalhada deixa de ler relógios e contar
retomadas. Os resumos antigos continuam funcionando. Sem PerformanceStats,
o wrapper executa a retomada sem consultas adicionais ao relógio. Enquanto
ativa, a coleta adiciona duas leituras de relógio por retomada externa; uma
fila que gira muitas vezes pode sofrer custo de instrumentação. Comparações
de FPS precisam considerar isso.

Essa mudança preserva a ordem das tarefas, as esperas e as condições de
sincronização. Não constitui otimização de FPS. O log de Davy de 16/10 às
20h05 antecede esta coleta e não pode fornecer esses campos retroativamente.
