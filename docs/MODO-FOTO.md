# Modo foto: previa com pausa e camera livre

Abra **Testar modo foto.cmd** na raiz do projeto, com outras partidas fechadas.
Usa a compilacao atual, janela1920x1080, ingles e seus saves normais.
Na rua, com a janela ativa:

- **F10:** entrar/sair; ocultar interface e pausar pela API nativa do jogo.
- **WASD:** deslocar camera; **Espaco/Ctrl:** subir/descer.
- **Mouse ou J/I/K/L:** olhar; **roda do mouse:** zoom pelo campo de visao.
- **Shift:** deslocamento mais rapido.
- **F12:** salvar PNG em `photos/`, na pasta de execucao.

Sair restaura a camera original e libera a pausa adquirida pela previa.
Perder foco/minimizar restaura a interface; a proxima fase do jogo libera
sua pausa. Esc/Tab e acoes de dialogo tambem encerram a previa. Espaco e
movimento vertical enquanto a camera livre esta habilitada.

O launcher portatil tem **Photo preview** em Controls, desligado por padrao.
Codigo e tela atualizados; pacotes pessoais/dos amigos ainda precisam ser
reconstruidos para receber esta versao. O arquivo de teste da raiz usa o
executavel atual sem depender desses pacotes.

## Fotos e limites

O PNG vem da superficie final PS4 antes do redimensionamento e dos overlays
do port. As fotos conferidas tem1920x1080. RGBA/BGRA8bit suportados; outros
formatos, incluindo HDR10, recusados com mensagem no log. Nomes preservam
fotos anteriores. Capturar aguarda GPU/comprime PNG e pode causar pausa
curta; manter fotos fora dos benchmarks.

Deslocamento inicial limitado a10unidades do motor em torno da camera de
entrada; campo de visao15..110graus. Outras cenas, cutscenes, transicoes,
sombras/reflexos, transparencias e streaming ainda precisam de validacao.
Exposicao e profundidade de campo ainda nao tem controles.

H alterna so o guia original grande/pequeno/oculto. F10 usa estado proprio.
Filtros adicionais reconhecem a arvore do icone de mensagens e a classe
especifica do telefone3D do save; mensagens, maquina e APIs de save preservadas.
A notificacao ativa ainda precisa de reproducao para confirmar cobertura.

## Evidencia

Davy confirmou F12 em19/10. Em20/10 entrada real gerou fotos de camera girada,
avancada e elevada durante a pausa. F10 restaurou a visao normal e permitiu
reentrar. Roda do mouse alterou enquadramento; minimizar resultou em pausa0 e
FOV original40. A pausa precisou de recomposicao explicita da vista principal:
interceptar somente o compositor normal tambem congelava a camera livre.

Compilacao completa/testeSDL/walker de proveniencia passaram. Na rodada final,
o marcador azul3D do save desapareceu comF10 e voltou ao sair, preservando a
maquina e o cenario. Uma notificacao de mensagem separada ainda precisa de
reproducao; sua correcao nao foi declarada visualmente comprovada.
Detalhes em [FOTO-CAMERA-PAUSA.md](re/FOTO-CAMERA-PAUSA.md).
