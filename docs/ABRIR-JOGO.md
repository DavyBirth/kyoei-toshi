# Abrir por duplo clique

Na pasta `G:\kyoei-toshi`, abrir **City Shrouded in Shadow.exe**.
Ele usa a mesma preparação e os mesmos saves do port atual, sem precisar
de um comando nesta conversa. A janela do jogo aparece sem um terminal extra.
O jogo continua em japonês até aplicarmos um patch revisado.

O launcher permanece aberto em segundo plano enquanto o jogo roda e evita
duas aberturas pelo próprio launcher. Fechar a janela do jogo encerra ambos.
Não impede uma abertura manual por outro comando.

Se houver erro, o launcher indica `build/launcher-last.log`; o registro completo
do jogo continua em `G:\kyoei-toshi-work\native-05\logs`.

Para recompilar: `powershell -NoProfile -File tools/build_launcher.ps1`.
Para verificar os caminhos sem abrir o jogo: executar o launcher com `--check`.
O executável local não incorpora nenhum arquivo do jogo; ainda depende desta
instalação e do dump próprio. Não é um pacote portátil para distribuição.
