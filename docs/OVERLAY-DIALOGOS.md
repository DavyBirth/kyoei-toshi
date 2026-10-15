# Diálogos na imagem do jogo — prévia offline

Implementados e compilados em 13/10/2026. Testes offline de modelo, ImGui e
ABI passaram. Desenho/composição Vulkan em uma partida ainda não verificados;
não abrir o jogo automaticamente para certificar esta prévia.

## Ativação atual

No portable atualizado em 15/10, English ativa a interface dentro do jogo
automaticamente. O launcher de desenvolvimento inicia com a opção marcada;
preferências já salvas podem manter a seleção anterior. Apresentação nativa
continua como fallback se o backend Vulkan do overlay estiver indisponível.
O script também aceita `-InGameDialogs` junto de `-Language en`.
`-CheckOnly` confere configuração sem iniciar jogo.

O que Davy deve ver no futuro: teclado sobre a imagem quando usar controle,
edição com teclado físico quando começar a digitar, lista de slots de save/load
e confirmações na mesma apresentação. Ainda exige teste de foco, CJK, teclado
real/controle, minimizar/restaurar e guardar/carregar em gameplay.

## Caminhos reais ligados

- sceImeDialogInit abre estado compartilhado; GetStatus/GetResult/Abort/Term
  mantêm contrato anterior. Filtros convidados continuam na thread de GetResult.
- Lista e confirmações/progresso usam o backend/slots/resultado existentes.
  Estado possui cópia própria dos labels; worker aguarda condition variable.
- SDL recebe texto/mouse/teclas; polling físico do pad aplica cada borda de
  botão uma vez. Entrada convidada fica neutra enquanto o modal existe.
- Presenter compõe ImGui após o blit do frame, usando attachment Load e
  transições ColorAttachment/Present. Driver redesenha último frame quando
  o convidado espera resposta e não produz novos flips.
- Mudança de formato/contagem do swapchain reconstrói recursos após GPU idle.
  Fonts usam Noto existentes, sem arte original nova distribuída.

## Contratos e testes

`test_kyoei_overlay`: ImGui real sem SDL/Vulkan/jogo. Testa desenho dentro da
área em 640×360 e 1280×720, seleção de teclado/controlador, UTF-16, bordas,
substituição de default com terminador, 1.025 slots, labels copiados, cancelamento
antes/durante abertura, progresso não confirmado por botão e fallback.

`test_kyoei_ime_overlay`: adapter verdadeiro chamado pela ABI System V, com
filtro convidado sintético. Prova filtro na thread convidada, publicação apenas
em GetResult, limites/surrogates, palavras de guarda, cancelar/abortar e lifecycle.
Não abre a janela Win32 nem escreve saves.

Edição física de nome aceita Delete (caractere seguinte), Home e End, além
das setas/Backspace/Ctrl+A. Delete preserva pares UTF-16 e não avança cursor;
Home/End desfazem seleção e vão às extremidades. Testes do modelo e do overlay
real cobrem edição no meio de nome Unicode e resultado com terminador.

Os testes detectaram e corrigiram terminador residual ao substituir seleção
inicial, máscaras trocadas de Square/Triangle e escala acumulativa que zerava
propriedades ImGui ao redimensionar repetidamente.

Limite original de nome personalizado continua quatro unidades por campo.
Os quatro defaults têm apresentação romaji reversível, documentada em
m4/NOMES-PADRAO.md, sem aumentar buffers/saves. HUD original,
minimapa e truncamento de falas são outro caminho e seguem em investigação.
