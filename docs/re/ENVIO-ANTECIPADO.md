# Preparação de conversores e envios à GPU

## Conversores de textura

Os pipelines do jogo salvos no cache não incluem os shaders internos de
`TileManager`. O começo do jogo compilava nove detilers no primeiro uso.
A integração gera uma cópia local de `tile_manager.cpp` e do shader incorporado
da referência fixa, preparando essas variantes no construtor, antes de
`GPU: Vulkan backend ready` e antes de executar código do jogo.

Variantes de uma amostra observadas no log212733:

- Display2DThin:32 bits.
- Depth2DThin64:32 bits.
- Thin1DThin:8,32,64,128 bits.
- Thin2DThin:32,64,128 bits.

Os objetos ficam no mesmo array `detilers` já usado pelo backend. Não há
dispatch antecipado, alteração dos dados das texturas nem nova espera de GPU.
Outras variantes continuam sob demanda. `KYOEI_LAZY_DETILERS=1` restaura a
inicialização anterior para comparação local. O custo vai para a abertura;
isso não comprova ganho na taxa de quadros após carregar a cena.

Teste real em GPU com usuário isolado:as nove listas de defines coincidiram
exatamente com o log original,compilação47,10ms;execução normal isolada60,41ms,
sem nova compilação desses detilers após o backend ficar pronto. A cena foi
carregada e capturada. Os saves originais não foram usados para escrita.

A antecipação registra slots ainda não utilizados. Na primeira utilização
real, se número de amostras ou modo de array diferirem da variante preparada,
o pipeline antecipado é descartado enquanto ainda não foi submetido, e o
backend compila a especialização pedida. Depois da primeira utilização,
aplicam-se as regras originais de vida útil; esse mecanismo nunca descarta
um pipeline já usado pela GPU. A marcação é isolada por TileManager.

Teste real adicional:preparou nove variantes em49,11ms,substituiu a variante
Depth2DThin64/32 ao pedir quatro amostras e compilou com NUM_SAMPLES=4;exit0,
antes de executar o jogo. Teste de CPU cobre primeira utilização única,
isolamento de dois gerenciadores,limites de slot e limpeza de marcas pendentes.

## Experimento de envio antecipado descartado

Foi testado envio assíncrono em mudanças naturais de passe,
após pelo menos64 desenhos e4ms de preparação no mesmo tick. A leitura de
relógio acontece ao trocar tick ou quando uma mudança de passe pode ser elegível.

O ponto fica depois de calcular o estado do próximo passe e antes de emitir
bindings/estado dinâmico/desenho. Só divide quando o estado novo difere do
anterior. Interromper um passe inalterado poderia repetir loadOp=Clear;
por isso essa condição é proibida. Não são removidas esperas,barreiras ou
publicação de resultados. Usa `Scheduler::Flush`,na mesma fila e na mesma ordem.
O scheduler já termina o passe,aloca outro command buffer e invalida o estado
dinâmico para o próximo desenho.

O experimento foi retirado do código normal por ausência de ganho demonstrado.
Receita e fontes experimentais ficam somente no relatório privado de pesquisa.
O teste automático usa uma cópia privada de
cache e savedata,câmera por mouse desligada,mesmo slot,60s para estabilizar
após carregar StageModule e35s de medição. Registra offsets do log e amostras
de foco da janela;ângulos/cargas/foco diferentes invalidam uma comparação direta.
As primeiras rodadas foram invalidadas por ângulos diferentes. Rodadas seguintes
ignoram controle físico no processo isolado e usam R3 (tecla R) para recentralizar
a câmera. A medição de foco também variou,limitando comparações precisas.

## Apresentação

Com recentralização,Mailbox29,76FPS e Fifo29,77FPS nos intervalos resumidos da
vista parada. Isso não justifica mudar o modo de apresentação padrão. São
amostras curtas com diferenças de foco;nenhum percentual de ganho é atribuído.
A captura de pilhas ocorreu depois do recorte medido em Fifo;essa execução
teve saída anormal após a depuração e não valida encerramento normal do jogo.

## União de intervalos de vértices

A captura da thread gráfica mostrou `BindVertexBuffers` executando a ordenação
dos intervalos. Uma única pilha não estabelece a porcentagem de CPU desse trecho.
Foi implementada uma simplificação equivalente:quando cada intervalo intersecta
ou encosta na união acumulada,o conjunto conectado vira um único intervalo por
min/max. Se aparece um vão,o helper não toca na saída e o algoritmo original
de ordenação/união é executado.

A mudança ocorre antes de mapear os buffers,quando os campos de buffer Vulkan
e offset ainda estão zerados. Mantém os mesmos intervalos mapeados,barreiras,
bindings e ordem dos desenhos. Não reutiliza endereços ou handles entre quadros.
`KYOEI_LEGACY_VERTEX_SORT=1` restaura o algoritmo anterior para comparação no
mesmo executável. A referência fixa continua intacta.

O teste cobre100.000 uniões aleatórias mais adjacência,duplicatas,vãos,intervalo
que faz ponte em ordem diferente e endereços próximos de UINT64_MAX. Compara
o resultado ao algoritmo de referência. Fixture de CPU para200.000 conjuntos
de8 intervalos sobrepostos:51,51ms antes,36,94ms com caminho conectado. Essa
medição inclui contêineres da fixture e não é ganhoFPS ou porcentagem do quadro.
