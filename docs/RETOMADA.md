# Retomada — 12/10/2026

Checkpoint adicional de 13/10: modelo headless de nomes e modalidade ativa
implementados; SDL/IME reais alimentam modalidade; build kyoei e dois testes
headless PASS, jogo não executado. IME protege UTF-16. Overlay e save/load
na imagem ainda pendentes. Mapa técnico e fontes em docs/FUNCIONAMENTO.md.

Atualização em 13/10/2026: plano offline salvo em `docs/PLANO-MANHA.md`.
Primeiro checkpoint de execução: construtor/validador aceitam --language en;
construtor --validate-only validou 94 entradas EN sem criar pacote nem alterar
manifesto. Dez regressões sintéticas PASS. Diagnóstico e caixas de interação
com corte em docs/m3/HUD-INVESTIGACAO.md. Sem correção visual certificada.
Na próxima sessão da escola, não abrir o jogo. Por pedido explícito de Davy,
fazer primeiro a tradução inglesa, salvar/validar essa etapa e somente depois
passar ao português. O plano não agenda execução. Os 4% abaixo são históricos.

Davy pediu parar perto de 3% da quota; informou 4%. Progresso salvo e trabalho
encerrado após o checkpoint. Ler também `docs/DIARIO.md` e AGENTS.md.

## Restrição e estado atual

Não abrir o jogo automaticamente. Davy abriu uma partida PT-BR por conta
própria no fim da sessão, PID observado 4772. Não fechar ou reiniciar essa
partida sem pedido. A proibição de abertura pelo agente permanece.

Log atual: native-05/logs/20261012_225841.log. A partida de Davy está usando
runtime-preview-01 (45 entradas), iniciada antes do pacote 02 ficar pronto.
O manifesto agora aponta para 02 (94 entradas), usado na próxima abertura.
O log confirma PT-BR e tela cheia 1920×1080; não trocar a partida automaticamente.

Não alterar `G:\godzilla-port-codex` nem o core fixo. CMake adapta cópias em
build. Dump próprio: `G:\GodzillaShadPS4PC\launcher\game_data\CUSA03562`, 01.03.
Original intacto; textos e arte completos ficam fora do Git.

## Próxima prioridade: defeitos relatados por Davy

1. Marcador "Parque: encontrar sua amiga" se move estranhamente com a câmera.
   Davy esclareceu: texto não acompanha bem o indicador. Corrigir e verificar
   tanto inglês quanto português, sem assumir que ambos foram testados.
   Fonte do texto: `common/lua/map_goal_text.luc`, variante p0/k17. Investigar
   medidas/âncora/transformação do rótulo com texto latino; causa não provada.
2. Objetivo "Encontre sua amiga. Primeiro, chegue à avenida." perde o "à".
   Variante p0/k36 no mesmo recurso. O texto reconstruído preserva o caractere
   UTF-8. Descobrir qual tabela/atlas a tela usa; principal tem o glifo,
   variantes não têm. Não converter para português sem acentos.
3. Ainda pendentes: piscada preta ao entrar em Load/New Game e textura estranha
   no tutorial do orbe. Fonte original da imagem do orbe está limpa.
4. Desempenho: última medição limpa anterior deu ~22,74/25,41 FPS em janelas
   mistas de cidade/eventos. Nenhum ganho novo de FPS comprovado nesta etapa.

## Novos requisitos de interface e nomes

- Davy confirmou que conseguiu entender a seleção de personagem e gostou
  da tradução. Isso é retorno positivo desse trecho, não aprovação completa.
- Implementar teclado de nome dentro da imagem do jogo, navegável com controle.
  Detectar modalidade ativa de entrada, permitindo alternar teclado/controle;
  não basta detectar se há um controle conectado. Entrada física de teclado
  continua disponível, com interface adequada à modalidade usada.
- Integrar também save/load na interface do jogo, conforme pedido de Davy.
  Hoje usam diálogos nativos. Preservar funcionalidade atual, slots, saves,
  círculo para confirmar e X para voltar. Não refazer backend sem necessidade.
- Nomes padrão de personagens em romaji nas versões EN/PT-BR: localizar os
  valores iniciais, confirmar leituras/grafias e proteger identidade. Não
  traduzir significado, inventar leituras ou mudar nomes personalizados.
  Conferir tamanho, exibição, entrada e persistência nos saves.
- Esses itens foram registrados perto do limite; ainda não implementados.

## O que já existe

- Launcher local `G:\kyoei-toshi\City Shrouded in Shadow.exe`: janela com arte
  PS4 e ícone, JA/EN/PT-BR, resolução de saída e janela/tela cheia sem bordas.
  Preferências em build/launcher-settings.json. Fontes do launcher versionadas;
  arte/ICO em build/launcher-assets, ignorados. Doc: ABRIR-JOGO.md.
- Tradução parcial: 94 entradas por idioma, três recursos. Arquivos
  translations/pilot-opening.json (30), pilot-options.json (15) e
  pilot-intro.json (49). Nomes de jogador, ícones e cores preservados.
  Rascunhos ainda exigem revisão humana; layout não certificado.
- Pacotes privados atuais runtime-preview-02 em
  `G:\kyoei-toshi-work\translation`, EN/PT-BR common.psarc. Manifesto:
  docs/m4/runtime-preview.json. Runtime monta só o pacote selecionado em
  /app0/DAT/common.psarc e /hostapp/DAT/common.psarc. Outros dados vêm do dump;
  saves em G:\kyoei-toshi-work\native-05\user. Nunca distribuir esses pacotes.
- Catálogo completo auditado: translation/catalog-01, 2.331 recursos,
  230.622 ocorrências UTF-8. Candidatos preliminares 15.687; cobertura de todos
  os textos visíveis ainda não certificada. Quatro vídeos permanecem na fila.
- Páginas privadas scene-review-02/index.html: 23 recursos/7.946 trechos,
  revisão lado a lado e exportação sem o japonês original.
- Galeria correta: catalog-01/images-v2/review-portable.html. 1.412 candidatas,
  1.411 prévias e uma legada pendente. Decoder antigo assumia BC3 em tudo;
  corrigido para BC3/RGBA8/R8. NÃO usar a galeria antiga para certificar pixels.
  Primeira folha revisada: 25 preservar, dez traduzir, uma pendente.
- Fontes: docs/m3/FONTES.md. Principal tem os 153 caracteres verificados,
  inclusive acentos; sete variantes common/font e uma de sistema faltam 52.
  Atlas principal é R8 linear. Folha privada inspecionada confirma glifos.
- Readback não coerente corrigido após fence, testes passam. Lote permanece
  opt-in KYOEI_BATCH_READBACK=1; padrão desligado, benefício real pendente.
  KYOEI_DISPLAY_TRACE=1 registra caminhos explícitos de frames pretos.

## Ferramentas e verificações

- Build: `cmake -S . -B build`, `cmake --build build --target kyoei -j 4`.
- Python de imagens: build/image-tools/Scripts/python.exe, Pillow/fonttools.
- Launcher: preparar arte com tools/prepare_launcher_assets.py nesse Python,
  depois powershell -NoProfile -File tools/build_launcher.ps1.
- Launcher --check e scripts/run_native.ps1 -CheckOnly verificam configuração
  sem jogo. --render-preview renderiza somente o launcher de forma oculta.
- tools/test_translation_mount.py: backend real, dados sintéticos, JA/EN/PT
  e somente leitura PASS. test_psarc_patch.py: quatro PASS; test_text.py:
  seis PASS; proteção de nomes: cinco PASS; controles: dois PASS; TEX:
  cinco PASS. Build passou. Readback e regressões de GPU passaram.
- Recriar pacotes em NOVO diretório com tools/build_runtime_translation.py
  OUTPUT translations/pilot-opening.json translations/pilot-options.json
  translations/pilot-intro.json --preview. Verifica source hashes e reinserção.
  Versões privadas 01/02 ocupam ~25,4 GB ao todo; não apagar o dump para limpar.
- Commits relevantes: 5de88ac catálogo; 0dd5210 launcher simples;
  a205a7a readback/diagnóstico; 062adc8 launcher visual e prévias de tradução.
  O checkpoint seguinte contém estes relatos e este arquivo de retomada.
