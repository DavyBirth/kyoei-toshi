# Modo foto: primeiro piloto

Pedido de Davy em 19/10/2026: desenvolver em paralelo às melhorias de
desempenho. A primeira etapa é tirar uma foto limpa, usando a câmera normal.

- **H** alterna somente o guia de controles entre grande, pequeno e oculto.
  A sequência foi conferida na rua: HP, mapa, objetivo e interação continuam
  visíveis. H não é um comando para ocultar a interface inteira.
- **F12** salva uma imagem PNG em `photos/`, na pasta de execução do port.
  O nome inclui o instante e um número; fotos anteriores são preservadas.

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

Compilação completa passou. Em 19/10/2026, Davy ativou a janela e apertou F12:
o port salvou uma foto real de 1920x1080 durante a partida. A imagem foi
conferida: cena da rua com personagem, fogo, interface e mapa, sem desktop
ou interface do port. Isso valida F12 e a gravação na partida.

Na janela Controls do launcher, a opção **Photo preview** ativa
`KYOEI_PHOTO_HUD=1`. F10 alterna a
travessia de desenho de uma árvore nativa de HUD. O padrão continua desligado;
não declarar cobertura de todos os elementos ou de todas as cenas antes de
conferir. Restaurar ao perder foco, minimizar, fechar ou abrir confirmação,
diálogo/mapa/menus. A atualização do jogo continua ativa.

Em 19/10, duas vistas da rua foram conferidas antes, durante e depois:
HP, mapa, objetivo e interação desapareceram e voltaram; cenário e animações
continuaram. Uma sequência com foco real e teclas F10/F12 produziu dois PNGs
de 1920x1080, um sem HUD e outro com HUD restaurado. O teste de entrada SDL
passou para foco, repetição, janela errada e restauração por ações de diálogo,
controle, minimização e fechamento. As sondas privadas foram retiradas antes
da compilação usada na sequência real. O launcher compilou e sua tela de
controles foi conferida; o pacote portátil pessoal ainda não foi atualizado.

Próximos testes: conferir antes/durante/depois em outras cenas e transições,
comparar fotos com outra resolução de janela e testar pasta de saída sem
permissão. Câmera livre e pausa exigem
investigação independente de câmera, lógica e desenho.

Antes disso, uma solicitação privada no renderizador gerou PNG de 1920x1080
na tela inicial. Dois roteiros automáticos ficaram sem foco e não produziram
fotos; não constituem falha do atalho. Sonda automática removida do código final.
