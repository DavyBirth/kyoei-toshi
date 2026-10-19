# Modo foto: primeiro piloto

Pedido de Davy em 19/10/2026: desenvolver em paralelo às melhorias de
desempenho. A primeira etapa é tirar uma foto limpa, usando a câmera normal.

- Durante a partida, **H** usa o comando original do jogo para mostrar ou
  esconder a interface. Confira a tela antes de tirar a foto; menus e avisos
  podem ter comportamento próprio.
- **F12** salva uma imagem PNG em `photos/`, na pasta de execução do port.
  O nome inclui o instante e um número; fotos anteriores são preservadas.
- Aperte **H** novamente para voltar à interface, se você a escondeu.

O PNG recebe os pixels da superfície final do PS4 antes do redimensionamento
da janela e da interface acrescentada pelo port. A resolução vem da imagem do
jogo: uma janela menor não reduz a foto. A resolução deve ser conferida nas
propriedades do PNG; não presumir 1080p só porque o monitor é 1080p.

A câmera e a partida continuam funcionando enquanto você prepara a foto.
Não há câmera livre, congelamento da simulação, ajuste de exposição ou
profundidade de campo neste piloto. A interface do jogo é controlada
manualmente, sem presumir seu estado nem mexer em saves.

F12 só solicita captura com a janela focada. Segurar a tecla não gera uma
sequência de fotos. Sair do foco cancela a solicitação ainda pendente. Uma
captura aguarda a GPU e comprime o PNG, podendo causar uma pausa curta naquele
instante; tirar fotos deve ficar fora das medições de desempenho.

O primeiro caminho aceita RGBA/BGRA de 8 bits. Outros formatos, incluindo
HDR de 10 bits, são recusados com mensagem no log para evitar fotos com
cores incorretas. Falhas de gravação também aparecem no log.

Validação inicial: cópias isoladas da janela e do apresentador passaram na
compilação de sintaxe. Compilação completa passou. Captura real durante a partida ainda precisa ser
confirmadas antes de declarar o recurso pronto para uso.

Próximo teste mudo: partida em 1080p, esconder HUD com H, F12, conferir PNG
sem desktop/interface do port, cores e tamanho; H de novo deve recuperar a
interface. Comparar com outra resolução de janela e testar tecla repetida,
perda de foco e pasta de saída sem permissão. Câmera livre e pausa exigem
investigação independente de câmera, lógica e desenho.

Prova de gravação: solicitação privada no renderizador gerou PNG real de1920x1080 na tela inicial, conferido visualmente. Dois roteiros automáticos de F12 ficaram sem foco e não produziram fotos. Atalho durante a partida e sequência H/foto/H ainda pendentes. Sonda automática removida do código final.
