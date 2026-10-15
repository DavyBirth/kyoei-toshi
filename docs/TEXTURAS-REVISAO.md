# Galeria privada de texturas

Davy esclareceu em 15/10/2026: relato de que Options/títulos estavam bons
não é aprovação definitiva das imagens. As 59 texturas seguem em revisão.

Gerador: `tools/build_localized_texture_gallery.py`. Usa apenas o dump próprio
inventariado e as TEX inglesas atuais de `build/overnight/textures-en`.
Ambiente com Pillow já instalado: `build/image-tools/Scripts/python.exe`.

Galeria atual: `build/texture-review/index.html`, com 118 PNGs em resolução
nativa, original/inglês lado a lado. Filtro por caminho, zoom isolado em tamanho
original, ajuste à tela e decisão/notas por imagem. Todas começam pendentes.
O botão **Salvar minha revisão** exporta JSON; decisões também ficam no
navegador local, associadas ao hash da textura inglesa. Não altera o jogo.

Conferência em Edge separado/headless: 59 cards renderizados, comparação
e contagem visíveis, screenshot inspecionada em `build/texture-gallery.png`.
Aberta também no navegador de Davy. Conteúdo original e previews são privados,
ignorados pelo Git e separados do portable para mantê-lo enxuto.
