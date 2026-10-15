# Galeria privada de texturas

Revisão importada em 15/10/2026 diretamente de Downloads, sem anexar JSON
no chat: 1 aceita (E06_fiction), 58 pedem ajustes. Ferramenta:
`tools/import_localized_texture_review.py`, com validação de hashes e inventário.
Cópia privada: `build/texture-review-backups/e967c3f206d7a4b4`, contendo as
59 TEX atuais, galeria original/inglês, JSON recebido e manifesto SHA-256.
Nenhuma sugestão gráfica foi aplicada nesta etapa; preservar essa referência
ao preparar ajustes futuros, que continuam sujeitos à revisão de Davy.

Pedidos registrados: centralizar Options, aproximar altura da fonte do original
sem esticar horizontalmente, sombra suave nas seleções e títulos, melhorar
fonte/alinhamento de E06_message. Cobertura: 59 texturas localizadas entre
1.412 candidatas UI/história (não necessariamente todas com texto).
O inventário ainda não identifica todo texto visível. Não certifica jogo
inteiramente em inglês; LOCALIZACAO-PENDENCIAS.md também registra lacunas
narrativas nos textos, inclusive notícias e material de leitura.

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
