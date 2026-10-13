# Retomada — 13/10/2026

Mais recente 172251: 1.885/1.781 perfis completos/sem perdas, mas ZERO constantes.
Erro do diagnóstico: BufferResource::IsStorage sempre retorna true nesta base;
filtrar !is_storage excluía constantes somente leitura. Corrigido para Guest
e !is_written, mantendo guardas GPU dirty/leitura/limites. Teste com estrutura
real comprova armazenamento storage e elegibilidade read-only; escrito/especial
rejeitados. Build e testes PASS, parser nove PASS, CheckOnly PASS. Novo binário
build/orb-pending/kyoei-orb-trace-next.exe; mesmo atalho constantes usa esse.
PID 21636 continua com build/kyoei-orb-trace-next.exe antigo; não fechar ou
substituir. Usuário precisa reiniciar por conta própria para nova coleta F9.
Relatório 05 e captura orb-user-20261013-04.png privados; causa ainda desconhecida.
Parser agora declara constant_capture_present, sem confundir com log completo.

Log 20261013_162330 analisado: 1.957/1.707 perfis completos, zero descartes;
relatório build/orb-raster-report-04.json. Davy relata corte agora à direita,
confirmado em captura privada build/orb-user-20261013-03.png. PID 32984 aberto,
não fechar/reiniciar. Vértices selecionados separam caixas; recortes continuam
inteiros. Constantes lidas depois por memória podem ter sido reutilizadas:
não representam o instante F9. Novo kyoei-orb-trace-next.exe compilado separado,
captura até 64 bytes de oito uniform buffers no binding, ignora GPU dirty/storage.
Testes limites/falha/dirty/disabled PASS; parser nove PASS; CheckOnly PASS.
Atalho Testar orbe constantes EN.cmd, F9 antes e no tutorial após Davy encerrar
a partida atual por conta própria. Executável atual e normal preservados.
Não afirmar correção. Próximo analisar uniform_samples para transformação VS
e composição PS. Script recusa partida existente via nomes Win32_Process.

Nova captura 20261013_160452: IDs 1/2 completos, 839/1.133 perfis, zero perdas;
nenhum candidato de recorte na esquerda. Relatório build/orb-raster-report-03.json.
Os vértices anteriores eram começo de buffers UI compartilhados, não subdraws:
não inferir limites das caixas desses XY. Diagnóstico separado recompilado agora
resolve índices 16/32 bits, base vertex efetivo (inclusive fetch shader) e offsets
diretos; indiretos marcados indisponíveis. samples/selection/indices/selected_xy
no log. Testes negativos/limites/GPU dirty PASS, parser oito PASS, CheckOnly PASS.
Normal SHA preservado, configuração CMake restaurada para kyoei. Próximo: repetir
F9 antes e durante tutorial pelo mesmo Testar orbe EN.cmd. Sem correção visual
comprovada; não mudar recorte por palpite. Nenhum jogo iniciado nesta análise.

Capturas 20261013_143716 analisadas: tutorial sem perdas (255 perfis), recorte
não mostra faixa/offset; baseline perdeu 1.045 ocorrências acima de 256 perfis.
Não mudar scissor com esses dados. Complemento kyoei-orb-trace.exe compilado:
2.048 perfis, XY numérico de buffers float compatíveis e parâmetros VS/PS,
sem GPU waits. Testes de cópia/guardas/disabled e parser oito PASS. Próximo:
F9 antes + no tutorial com o complemento, comparar geometria/transformação/
amostragem. Dados XY não resolvem índices nem são pós-VS. Causa não provada.

Diagnóstico do orbe pronto: Testar orbe EN.cmd usa kyoei-orb-trace.exe separado,
normal kyoei.exe com hash preservado. F9 antes de pegar e F9 no tutorial
registra viewport/scissor/offset/target/blend/endereço dos shaders de dois
quadros. Compilado/CheckOnly e testes offline PASS; nenhum jogo iniciado.
Próximo: comparar captures begin/end no novo log com analyze_raster_trace.py.
Não atribuir causa só pela borda de imagem, não usar captura para medir FPS.
Build configurado de volta ao nome kyoei para próximos trabalhos normais.

Mais recente: Davy testou e pediu olhar orbe. Defeito é camada alaranjada
sobre cenário deixando faixa limpa na esquerda; foto do tutorial íntegra.
Capturas privadas build/orb-user-20261013-01/-02.png e comparação original.
Partida PID observado 26124, log 20261013_135113, não iniciada pelo agente.
Preservar processo/executable; próximo diagnóstico é scissor/viewport/offset/
blend/render target da camada. Ver DIAGNOSTICO-GRAFICO.md; causa não provada.
Testes reais confirmam 28 chars recebidos/exibidos e font size 38; ancoragem
visual ainda pendente. Readbacks periódicos 1 imagem/1 espera; sem ganho FPS
comprovado, ~29 FPS no tutorial parado não equivale à rua em movimento.

Prévia FitHud agora inclui medidas locais do minimapa (dois sites assinados),
sem mudar rotação/seta/clamps. ABI/instalação/fallback PASS, visual pendente.
Testar desempenho EN.cmd habilita agrupamento de readback para comparação
com Testar HUD ajustada EN.cmd. Contadores do caminho individual e fences/
reuso/publicação PASS. Sem ganho de FPS medido; manter padrão desligado.

Prévia de caixa EN implementada/compilada, testes offline PASS. Atalho
Testar HUD ajustada EN.cmd ativa FitHud; padrão continua original. Esperado:
Listen to their conversation inteiro, placa ajustada e espelhamento preservado.
Fonte/escala/bordas precisam de teste de Davy. Cache privado em build.
Minimapa/performance continuam pendentes. Retorno GPT via pasta
translation/retorno-gpt, não anexos no terminal. Ver HUD-INVESTIGACAO.md.

Outro GPT já trabalha na entrega inglesa. Davy pediu PT depois: pacote
separado external-pt-full-01/Kyoei-Portugues-handoff.zip pronto, 16.136 IDs
candidatos/173 arquivos. Original EN intacto. Importador suporta idioma en
ou pt-BR, auditados separadamente; nove testes e fixture PT real PASS.
Receber ZIPs/JSONs como anexos aqui, inclusive parciais; revisão ainda exigida.

Teste de Davy disponível: native-05/logs/20261013_130354.log, PID histórico
5432, depois deixou de aparecer. Rua ~21–29 FPS; readback ~10,5–11 ms; não
é comparação anterior/atual. Trace consumiu 24 updates iguais de Listen;
código agora deduplica conteúdo/nó, lê tamanho +0x19c e campos de fonte,
build PASS após fechamento pelo usuário. Davy não precisa de novo teste por
enquanto; continuar análise independente antes de pedir repetição.

Checkpoint posterior: diagnóstico cmnd_marker compilado, padrão desligado.
Testar HUD EN.cmd permite a Davy testar com frame stats/HUD trace; o agente
não o executou. Relay/assinatura/instalação PASS, build/CheckOnly PASS.
Próximo: analisar log dessa execução para fonte/escala/contagens e FPS;
então corrigir localmente medição/truncamento. Fonte principal/IM não
confirmada na caixa; não aplicar fator arbitrário.

Entrega externa pronta e enviada em links: docs/m4/TRADUCAO-EXTERNA.md.
ZIP privado external-en-full-01: 16.100 candidatos/173 JSONs, inglês primeiro,
138 recursos incluindo 20 Text_Stage; não modificar a entrega já enviada.
Davy está em casa e disponível para testes; pediu manter o trabalho da HUD
até terminar antes de mudar para ampliar tradução. Instrumentação/escala ainda
necessárias para corrigir cmnd_marker. Partida PID 28332 deixou de aparecer
na observação posterior; não foi encerrada pelo agente.

Atualização 12:38 Bahia: Davy abriu o jogo, PID 28332, EN 1280×720 com diálogos
internos; preservar a partida. Não recompilar/substituir build/kyoei.exe em uso.
Capturas sem controles confirmam cortes de interação/objetivo; ver
docs/m3/HUD-INVESTIGACAO.md. Relato de lentidão ainda sem comparação de FPS.
Take a closer look corrige rascunho p0/k30, validado EN, pacote ativo inalterado.
Regressão de progresso não cancelável por controles e barreiras Vulkan PASS.
Descoberta seguinte: rotina 0xa6d9a0 corta >=13 para 11 caracteres + U+2026
e calcula placa por contagem. Prefixo bate com captura. Próximo passo verificar
chamadores 0xa1de11/0xa6d2cf e ligação ao recurso; não alterar utilitário global.

## Estado mais recente: sessão offline executada

Atualização posterior ao encerramento precoce: Davy pediu continuidade
autônoma. Overlay de nomes/lista/confirmar/progresso agora ligado ao runtime
real e compilado, opt-in no launcher inglês; fallback nativo preservado.
test_kyoei_overlay e test_kyoei_ime_overlay PASS sem jogo. Ver
docs/OVERLAY-DIALOGOS.md e docs/FONTES-CODIGO.md. A afirmação abaixo de modelo
ainda desconectado é histórica: integração implementada, gameplay pendente.

- Não abriu jogo. Não usar os 4% históricos como quota atual; percentual não
  acessível. Nenhum processo de jogo foi encontrado na preparação.
- Inglês primeiro: 130 entradas, prévia privada runtime-preview-en-03 ativa
  no manifesto para próxima abertura. PT continua 94 entradas no pacote 02.
  A ampliação EN está em translations/pilot-street-en.json; revisão pendente.
- Pacote EN relido/hash/reinserção PASS. CheckOnly EN/PT PASS, sem gameplay.
  Páginas inglesas scene-review-en-03 (962 candidatos) e relatório próprio;
  revisão anterior de 23 recursos preservada.
- Commits: b3b1b17 idioma isolado; 01a8cb7 modelos de nome/modalidade;
  ccb7551 lista de saves. Checkpoint seguinte reúne prévia EN e estudo estático.
- Build kyoei PASS. Três modelos headless PASS (nomes/modalidade/lista),
  test_save_list ABI PASS. Modelo de teclado ainda sem apresentação ligada ao
  renderer; save/load continuam na apresentação nativa. Não prometer novas
  telas no jogo como prontas.
- Novas ferramentas: inspect_elf_references.py e measure_translation_layout.py,
  três testes cada PASS. Manifesto independente tem cinco regressões PASS.
- HUD/minimapa/acentos ainda sem correção visual. Relatório e endereços
  consumidores em docs/m3/HUD-INVESTIGACAO.md. Não alterar floats por palpite.
- Defaults de nome localizados, mas campos limitados a quatro caracteres +
  terminador; romaji completo não cabe. Ver docs/m4/NOMES-PADRAO.md. Não
  truncar nem substituir nomes personalizados ou saves para contornar.
- Log existente não tem instrumentação suficiente para diagnosticar orbe,
  piscada ou FPS. KYOEI_BATCH_READBACK permanece opt-in; nenhum ganho declarado.

## Próximos passos exatos

1. Continuar EN; não fazer nova rodada PT até checkpoint inglês coerente.
   Validar/build com --language en; não incluir pilot-street-en em uma execução
   PT, pois ele deliberadamente não contém esse idioma.
2. Rastrear consumidores/propriedades dos nós de texto a partir das referências
   do minimapa (0xa59aa9) e registro das fontes (0x6dc030). LEAs confirmadas
   por llvm-objdump; scanner de bytes produz candidatos, não descompilação completa.
3. Integrar modelos de nome/lista ao overlay gráfico, com snapshots/fila de
   eventos e redraw quando o convidado espera resposta sem novo flip. A base
   ImGui atual é shim sem desenho. Preservar fallback até teste visual futuro.
4. Investigar representação dos nomes e filtros a partir dos campos 0x13bda8c
   e 0x13bda96, sem aumentar wcsncpy para além da capacidade dos destinos.
5. Não executar scripts de gameplay. CheckOnly pode exigir -ExecutionPolicy
   Bypass somente no processo; não mudar política global do Windows.

## Histórico dos checkpoints anteriores

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
