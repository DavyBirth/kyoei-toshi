# Galeria privada de texturas

Continuação após reinício em 15/10: nova proposta em
`build/texture-review-v2/index.html`, aberta no Edge, com original japonês,
inglês anterior e inglês revisado. 59 grupos/177 PNGs. Não aplicada ao portable.
Os controles de aprovação continuam disponíveis para a nova revisão.

Mudanças: centro vertical dos botões calculado pela altura da imagem,
altura das letras das seleções aproximada à área branca dos glifos originais,
preservando a largura da fonte; sombra difusa nas seleções/títulos; linha
Stage maior; aviso inicial com letras maiores e espaçamento ajustado.
Conferência visual de amostras feita em `build/texture-review-v2/contact.png`.

Validação privada `build/texture-review-v2/validation.json`: 47 TEX mudaram;
os 11 `M_option_select_*` já produziam o centro vertical solicitado e ficaram
byte-idênticos. E06_fiction, aceita, copiada sem regenerar e byte-idêntica.
As 59 preservam dimensões, mipmaps, descritores e tamanho descomprimido.
Teste de preservação de bordas/gradientes PASS. A aprovação visual de Davy
continua pendente; não declarar as 58 solicitações encerradas.

Geração privada desta proposta: tabela `build/texture-revision-labels.json`
derivada de `translations/textures-en.json`, excluindo E06_fiction; executar
`tools/localize_textures.py` com saída `build/textures-revised` e copiar a
ficção aceita da versão preservada. Galeria suporta `--previous` apontando
para `build/overnight/textures-en`. Referência anterior e backup preservados.

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
