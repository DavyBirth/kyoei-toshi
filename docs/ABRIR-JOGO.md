# Abrir por duplo clique

Na pasta `G:\kyoei-toshi`, abrir **City Shrouded in Shadow.exe**.
Ele abre o launcher com a arte original do jogo e opções de idioma, resolução
de saída e janela/tela cheia sem bordas. Clique em **Iniciar jogo**.
Usa a mesma preparação e os mesmos saves do port atual, sem terminal extra.

Inglês tem13.226 entradas no runtime:13.096 do consolidado e130 pilotos.
Português permanece em94. São59 texturas localizadas: Options,19 cartões
de fase e dois avisos de abertura. A versão inglesa ainda exige revisão
editorial/visual; créditos, outros textos em imagens e problemas de HUD
permanecem pendentes. Não interpretar o total16.100 como texto todo visível:
inclui placeholders, identificadores e notas de desenvolvimento.
Revisão humana e verificação visual das prévias ainda estão pendentes.

A resolução selecionada altera o tamanho da saída em janela; a renderização
interna permanece como está. Tela cheia sem bordas usa a resolução da tela.
As preferências ficam em `build/launcher-settings.json`.

O launcher permanece disponível enquanto o jogo roda e bloqueia uma segunda
abertura por ele. Fechar o jogo permite iniciar outra partida. Fechar o launcher
durante uma partida o minimiza; não encerra o jogo. Não impede uma abertura
manual por outro comando.

Se houver erro, o launcher indica `build/launcher-last.log`; o registro completo
do jogo continua em `G:\kyoei-toshi-work\native-05\logs`.

Para recompilar: preparar a arte local com `tools/prepare_launcher_assets.py`
usando o ambiente Python de imagens, depois
`powershell -NoProfile -File tools/build_launcher.ps1`.
Para verificar os caminhos sem abrir o jogo: executar o launcher com `--check`.
O launcher local inclui o ícone derivado da sua cópia e lê a arte em `build`.
Ainda depende desta instalação e do dump próprio. Não é um pacote portátil
para distribuição; a arte original e os pacotes locais não vão no Git.

O pacote inglês atual é indicado por `docs/m4/runtime-preview.json`;
o português continua em `runtime-preview-02` na mesma pasta de tradução.
O runtime monta somente o pacote traduzido; todos os outros arquivos vêm do
dump original, que continua intacto. Esses pacotes locais incluem dados do
jogo e não são arquivos distribuíveis. No Git ficam as traduções e ferramentas.

## Cópia portátil pessoal

Em `build/City-Shrouded-in-Shadow-Portable`, abrir **City Shrouded in Shadow.exe**.
Copiar a pasta inteira para NTFS ou exFAT. O launcher próprio oferece original
japonês/inglês, resolução e tela cheia. Tudo é relativo à pasta: runtime, dados
do disco próprio, fontes, DLLs e tradução. Não usa instalação do projeto.
Começa com perfil vazio; os novos saves/configuração/logs ficam em `data/`.
Teclado e mapeamento dos prompts PS4 estão no README.txt da própria cópia.
Manifesto inclui hashes, idiomas e verificação de privacidade. Não compartilhar
publicamente: contém os dados do disco, e não apenas o patch.
