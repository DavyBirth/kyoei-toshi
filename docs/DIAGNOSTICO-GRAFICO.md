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

Capturas reais de Davy: log 20261013_143716.log. Capture 1: 3.754 draws,
256 perfis e 1.045 ocorrências descartadas pelo limite. Capture 2: 3.241
draws, 255 perfis, nenhum descarte. Ambos os logs completos, mas somente a
segunda amostragem completa. Relatórios privados orb-raster-report-01/02.json.
Nenhum perfil da segunda captura tem offset de janela diferente de zero;
screen/generic/window/scissor nos alvos 1920×1080 não mostram corte horizontal
que explique a faixa de ~102 pixels na saída. Viewports com altura negativa
são inversão Y, não sinal de recorte lateral. Não alterar scissor com esses dados.

A primeira captura perdeu parte dos passes finais; 38 perfis presentes só no
registro 2 não comprovam que surgiram no tutorial. Passes finais de UI usam
VS 0x103924c800, PS 0x103924d000/400 e dois alvos 1920×1080 nessa execução;
há também passes pós-processados menores. Endereços são deste processo,
não patches/IDs estáveis. Causa ainda não provada: investigar geometria e
amostragem/composição dos passes, em vez de aumentar recortes globalmente.

Complemento compilado no mesmo executável separado: limite 2.048 perfis,
parâmetros VS/PS em draws de UI/pós-processamento e até quatro atributos com
as primeiras quatro posições XY em formatos float32 compatíveis. São registros
do buffer, sem resolver índices nem aplicar transformação VS; não chamar
esses valores de limites finais de tela sem seguir a transformação.
Buffers GPU-modificados, instanciados, formatos incompatíveis e ranges não
residentes são marcados/ignorados, sem download ou espera adicional. Falha de
cópia não produz posição zero válida. Dados somente numéricos no log privado.
Testes de limites/cópia/falha/GPU dirty PASS e parser oito testes PASS.
Próximo: repetir F9 antes e na tela do tutorial com esse complemento pronto;
somente então escolher uma correção. Executável normal continua preservado.

Diagnóstico separado pronto: Testar orbe EN.cmd usa build/kyoei-orb-trace.exe,
em EN 1280×720, FitHud e frame stats; agrupamento desligado para a observação.
Apertar F9 antes de pegar o orbe, esperar dois segundos, pegar e apertar F9
novamente com o tutorial/camada alaranjada visível. F9 só aciona registro;
não fecha o tutorial nem altera viewport/scissor/blend. Arquivo normal
build/kyoei.exe preservado por comparação de SHA256, configuração de build
normal restaurada depois. Nenhuma partida iniciada pelo agente.

Captura estados dos próximos dois presents após o primeiro draw: shaders
por endereço, target 0, blend/depth/primitive, viewport real, recorte final e
bounds screen/window/generic/viewport com flags independentes de offset.
Deduplica perfis, limita a 256 e registra descartes. Não contém textura,
shader original ou falas. Captura é opt-in via KYOEI_RASTER_TRACE; padrão
desligado. Hotkey/registro entre threads usa contador atômico e mutex.

test_kyoei_raster_trace PASS com registradores reais, coordinates/flags,
deduplicação, dois quadros, limite/rearm e desligado, sem GPU/janela.
Build separado e CheckOnly PASS. Analyze_raster_trace.py preserva completude
e descartes e lista recortes à esquerda como candidatos, sem atribuir causa.
Seis testes de leitura PASS. Não tirar conclusão de FPS durante a captura,
porque a escrita desses registros pode pausar a apresentação.

Atualização 13/10, teste de Davy: defeito esclarecido como camada alaranjada
sobre o cenário, que não cobre toda a tela e deixa uma faixa limpa à esquerda.
Não é corrupção da imagem dos três orbes. Capturas privadas sem foco/controles:
build/orb-user-20261013-01.png e -02.png. A borda está aproximadamente a 102
pixels do início da área do jogo, em saída 1280×720; medida visual aproximada,
sem afirmar qual viewport/draw produz a camada.

Comparação com a prévia original limpa ccca0f02f40c2c30.png confirmou o
desenho dos orbes e fundo da foto. Brilho abaixo da foto não foi identificado
por Davy como o bug principal. Partida aberta por ele, PID histórico 26124,
log native-05/logs/20261013_135113.log; FitHud e BatchReadback ativos.
Não reiniciar nem substituir o executável dessa partida automaticamente.

Próxima evidência necessária: retângulos de viewport/scissor, offsets,
render target e blending do draw da camada, comparados com o antes do tutorial.
Logs atuais não capturam esses estados. Não remover o efeito ou aumentar
recortes globalmente para esconder a faixa. Renderer combina recortes screen,
window e generic e aplica offset; flags e limites são ponto de investigação,
sem causa demonstrada. O mesmo caminho está no
[renderer oficial atual do shadPS4](https://github.com/shadps4-emu/shadPS4/blob/main/src/video_core/renderer_vulkan/vk_rasterizer.cpp).
Não alegar que uma atualização do emulador resolveu esse defeito.

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

Testes de Davy 20261013_134625 e 135113 disponíveis. O segundo inclui parada
no tutorial (~28,8–29,6 FPS nas últimas janelas), não comparável diretamente
à caminhada do primeiro (~19,6–26,4 nas últimas janelas). Registros periódicos
de readback mostram uma imagem/uma espera/256 bytes, sem redução de esperas
nesses lotes. Nenhum ganho de FPS por agrupamento comprovado.

`KYOEI_BATCH_READBACK=1` agrupa downloads, publicando todos os bytes antes de
retornar ao jogo. Se o buffer circular for reutilizado, publica a região antiga
antes da nova cópia. O padrão permanece desligado até a comparação de execução.
O teste de integração reproduz três imagens em um buffer que comporta duas:
a referência usa três esperas, o lote usa duas, com bytes idênticos.

Isso prova a redução de esperas nesse caso sintético, não ganho de FPS no jogo.
Grupos com uma única imagem não têm esse benefício. Futuro teste deve usar a
mesma cena, sem capturas durante a medição, distinguindo cidade, menu e carga.
