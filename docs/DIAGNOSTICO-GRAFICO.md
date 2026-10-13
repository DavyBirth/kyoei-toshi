# Verificação gráfica pendente

O jogo não será aberto nesta etapa. As mudanças compiladas ainda precisam de
verificação visual e comparação de desempenho em uma futura sessão de teste.

## Piscada preta ao entrar nos menus

O driver tem dois caminhos explícitos para uma imagem preta: solicitação do
jogo com índice de buffer -1 e apresentação enquanto a saída de vídeo está
fechada. Uma imagem normal também pode conter um fade desenhado pelo jogo.
Ainda não sabemos qual caminho aparece na piscada relatada.

`KYOEI_DISPLAY_TRACE=1` registra início/fim de flips pretos solicitados pelo
jogo, apresentação com saída fechada e mudanças dos atributos da superfície.
`tools/analyze_native_performance.py` inclui essas linhas no relatório.
Sem essa variável, a instrumentação fica desligada. Não removemos frames
pretos, porque isso poderia esconder transições necessárias.

## Tutorial do orbe

A textura `texture/hud/operation_pic_treasure.tex` decodificada está limpa.
Isso descarta corrupção dessa imagem no dump verificado; não descarta erros
de composição, sincronização ou de outra textura usada nessa tela.

Foi corrigida a invalidação de cache do readback de imagens não coerente:
agora ocorre depois da conclusão da GPU e antes de publicar os bytes para o
jogo, nos caminhos síncrono, assíncrono e em lote. O código anterior invalidava
em Commit, antes de gravar a cópia. Teste dos métodos reais com GPU simulada
reproduz bytes antigos na referência e verifica a correção. Não há evidência
de que essa seja a causa do defeito do orbe na GPU de Davy.

Referências: [cache e mapeamento no VMA](https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/memory_mapping.html)
e [sincronização Vulkan](https://github.khronos.org/Vulkan-Site/spec/latest/chapters/synchronization.html).

## Desempenho

`KYOEI_BATCH_READBACK=1` agrupa downloads, publicando todos os bytes antes de
retornar ao jogo. Se o buffer circular for reutilizado, publica a região antiga
antes da nova cópia. O padrão permanece desligado até a comparação de execução.
O teste de integração reproduz três imagens em um buffer que comporta duas:
a referência usa três esperas, o lote usa duas, com bytes idênticos.

Isso prova a redução de esperas nesse caso sintético, não ganho de FPS no jogo.
Grupos com uma única imagem não têm esse benefício. Futuro teste deve usar a
mesma cena, sem capturas durante a medição, distinguindo cidade, menu e carga.
