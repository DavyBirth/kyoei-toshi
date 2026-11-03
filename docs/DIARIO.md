# Diário do projeto

### 15/10/2026 — salvamento final às 04:18

- Commits 2bf4850 (perfil de cache/identidade), c78a731 (avisos/en-12),
  35ee43d (portátil/monitor/triagem/relatório). Repositório sem alterações.
- Consolidado privado publicado, auditoria173 PASS, SHA256
  7b8d8477c55937cce7e7d6b44f5501552fbf15a052447c58c746c6ccb9e1155a.
- Última leitura da cota: 38% curta, 90% semanal. Não atingiu a reserva.
  Encerramento desta rodada após pacote verificável; validação em partida
  permanece bloqueada pela revisão automática até autorização específica.
- Monitor de uso será encerrado e desligamento autorizado solicitado somente
  após salvar. A solicitação efetiva fica em build/overnight/shutdown-request.json;
  não afirmar desligamento físico observado.


### 15/10/2026 — pacote portátil conferido

- Prévia final en-12: 13.226 EN incluindo pilotos, 59 texturas. Avisos iniciais
  traduzidos; ficção em linha única. SHA256 no relatório da sessão.
- Portátil privado: 125 arquivos, 24.221.637.327 bytes; 87 originais com hashes
  do disco, EN idêntico à prévia, varredura pessoal PASS. Sem saves/logs/contas.
- Launcher JA/EN e loader sem GPU/sem execução de código do jogo passaram em
  pasta com espaços e acentos. Conteúdo do pacote não foi modificado pelo teste.
- Cache truncado protegido; identificação Kyoei corrigida. Compilação PASS.
- en-08/09/10/11 obsoletas removidas com substituto conferido e sem consumidores.
  en-12/dump/saves/evidências preservados. Espaço livre após limpeza: ~59 GB.
- Tradução não declarada completa: 3.004 vazias triadas, nomes/créditos pendentes.
  HUD/60 FPS exigem partida; teste bloqueado por revisão automática, pedido de
  autorização específico pendente. Nenhuma partida iniciada/controlada.
- Detalhes, manifesto e testes: SESSAO-NOTURNA-2026-10-15.md.


### 15/10/2026 — continuação noturna e preparação portátil

- Davy autorizou tradução, HUD, engenharia reversa e desempenho durante a noite;
  checar cota a cada dez minutos, reservar10–15%, preparar cópia portátil
  pessoal sem identidade/logs/saves e desligar somente após entrega salva.
- Lido/preservado o avanço Claude:13.084 EN privadas, en-08 com13.214 incluindo
  pilotos/57 texturas. Mais12 EN em finish-20261015-news-lore:13.096/16.100
  privadas; en-10 com13.226/57 texturas. Inglês ainda requer revisão.
- Auditoria173 JSONs e reconstrução/releitura em117 recursos PASS.
  Adições20261015-02 contém12 EN. Consolidado privado republicado, SHA256
  8fdf31f92d92c4b2f81b63e53841f45a976b6f5cdbfba3d4ed1443773bf391b8.
- Options: rótulos fixos19/botões21, falha quando não cabem, textos curtos
  semanticamente completos. Botões recompostos com coluna limpa de cada linha,
  preservando bordas/gradientes. BC3 inspecionado e regressão sintética PASS.
  ST_15/ST_16 originais confirmados visualmente como Stage16/15: não inverter.
- scripts/check_usage.ps1 registra limite a cada600s, sem conversa/credenciais.
  Sessão ancorada por marcador atual. Indicador mudou de19% semanal para99%,
  novas datas de renovação, por volta03:20; causa desconhecida. Reserva considera
  ambas janelas. Não usar estimativa baseada só nos tokens do contexto.
- Pacote privado build/City-Shrouded-in-Shadow-Portable, launcher C# próprio,
  japonês/inglês, caminhos relativos, perfil vazio/fontes/dependências.
  Loader --list-stubs --no-gpu PASS sem partida; fonte/depuração removidas
  das seções de diagnóstico do PE. Varredura final de privacidade em curso.
- Varredura achou fallback Davy no serviço de usuário herdado. Adaptado para
  Player no CMake gerado; snapshot/Godzilla intactos. Launcher portátil fixa
  Player/idioma0 ou1. Coincidências em PSARC comprimido exigem contexto e
  conferência dos hashes do disco, sem alterar dados originais por palpite.
- Cache Vulkan por vendor/device/UUID, limite64MiB, rejeição de truncados,
  escrita atômica/fallback vazio. Inicialização antes de WarmUp. Saída normal
  pede Sync pela fila GPU, espera no máximo3s; erros não gravam. Build/cache/
  estatísticas PASS em testes de unidade; desempenho real ainda pendente.
- Abrir/fechar partida de teste foi bloqueado pela revisão automática por
  restrição anterior de não controlar jogo. Não contornado; autorização
  específica solicitada. Continuar trabalho sem partida enquanto pendente.
- Quebra por palavras ainda pendente; tipos/campos/rotinas encontrados,
  detalhes HUD-INVESTIGACAO.md. Não desabilitar wrap por palpite.
- 60 FPS não demonstrados. Log recente31,66 FPS agregados, leituras GPU10,86ms
  nas janelas amostradas. Métricas novas isolam compilação de shaders/pipelines,
  com registros limitados para pausas acima de16,67ms. Ver DESEMPENHO.md.

### 14/10/2026 — salvamento solicitado por Davy

- Davy pediu SALVA. Consolidado atual salvo e publicado: 11.887/16.100 EN,
  4.213 vazias; +948 desde 10.939 nesta continuação. Inglês ainda incompleto.
- Adições17/18: 147 e 63 EN, respectivamente, sem prosa japonesa original.
  Incluem notícias, dossiês, nomes documentados, epílogo, escolhas opcionais,
  rótulos de mapa, objetivos e as sete histórias do prédio Yukiyanagi.
- Yuki Kono usa cobertura inglesa de 2017 (Kouno, Gematsu, maio/2017),
  com vogal longa omitida conforme estilo; divergência Kano em matéria de
  2016 registrada nas notas. Não alegar pronúncia confirmada por áudio.
- Hinageshidai translitera hiragana explícito com sufixo topográfico dai.
  Yamabuki, Yukiyanagi e Kaido adotam leituras botânicas convencionais em
  topônimos florais; fontes botânicas primárias/dicionário nas notas privadas.
  West Azami traduz o componente direcional. Revisão humana ainda pendente.
- MP4 de Stage15 e Stage17 conferidos sem faixa de áudio. Identificadores
  de voz no dump são evidência de nomes, não reprodução ou escuta de falas.
  Extração de voz não concluída; sound extraído contém cabeçalhos .h e .acf.
- Auditoria dos 173 JSONs e reconstrução/releitura das 11.887 EN em memória
  PASS em 117 recursos. Campos protegidos preservados; nenhum inglês apagado.
- Consolidado privado published/Kyoei_English_Consolidated_Current.zip,
  SHA-256 bf78fc6ff5a3731d7bf48e004387d18db19f2cd4370880e4df908d04f9f9290c.
  CHECKPOINT.json e CONTINUE-HERE.md atualizados. Último lote yukiyanagi.
- en-05 permanece aplicada em 10.721 EN. Jogo não iniciado; dump/saves intactos.
  Próximo: lacunas player-facing, nomes ainda incertos, revisão transversal,
  HUD e textos em imagens/vídeos. Não confundir placeholders com tradução útil.

### 14/10/2026 — novas evidências de voz, notícias e trilha oficial

- Mais 168 EN em seis lotes, mais uma correção própria de grafia de título
  antes de exportar. Total 11.677/16.100, 4.423 vazias; +738 desde pedido
  de concluir. Continuação em curso, não declarar conclusão do inglês.
- Notícias integrais 0134/0013, rádio/música 0010/0005, créditos de nomes
  já comprovados, Abraham e 56 falas/rótulos/objetivos de NPCs.
- Evidências de nomes nos scripts próprios: Vo203_sakurako_010;
  Vo218_yanai_010, Vo218_kinosita_010, Vo218_kuribayasi; Vo212_iwabuti_010
  e Iwabuchi no demo212; Nakagawa em scripts204; Saeki em flags/animações.
  Conversão Kunrei si/ti para Hepburn shi/chi documentada. Prenomes incertos
  permanecem pendentes; evidência de sobrenome não resolve outros homônimos.
- Granzella Music oficial EN confirma Mai Iida, Daikoku Azuma, Hiroshi Ebihara,
  Kimi No Tonari De, Sky for Two of Us e See You. Fontes nas notas privadas:
  https://granzellamusic.jp/en/music/gzmm_10012/ e
  https://granzellamusic.jp/en/album/gzma_10007/ . Revisão transversal dos
  títulos em traduções anteriores ainda necessária; sem importar letras da web.
- Abraham: grafia corroborada por cobertura contemporânea do anúncio do jogo
  no Tokusatsu Network; entidade JP no site oficial Patlabor episódios33/34.
  Não confundir essa evidência secundária de grafia com localização oficial.
- k160 de 0013 retém glifos de pistas de bueiro com leitura EN. k164 interpreta
  aparente typo 地震 por 自身 conforme contexto explícito, marcado para revisão.
- Adições16:168 em16 recursos, sem original JP. Auditoria173 e reinserção das
  11.677 EN em117 recursos PASS. en-05 segue10.721; sem jogo iniciado.

### 14/10/2026 — continuação persistente: figurinos, loja e incêndio

- Mais 126 EN em quatro lotes: colunas completas de figurinos 0154, nomes
  confirmados, publicidade integral 0139 e notícias 0125 com variantes.
  Total 11.509/16.100, 4.591 vazias; +570 desde pedido de concluir. Em curso.
- Adições15: 126 em 15 recursos, sem original JP. Ficção/sátira dos anúncios,
  versos, avisos e quebras preservados; não apresentados como conselho real.
- Auditoria 173 JSONs, reconstrução/releitura de todas as 11.509 EN PASS em
  117 recursos. União com pilotos 11.639 validada em memória. en-05 permanece
  aplicada em 10.721. Sem iniciar jogo ou alterar dump/saves.
- Próximo: notícias longas restantes 0134 e demais lacunas; nomes próprios
  ainda exigem evidência, revisão editorial/HUD/imagens segue pendente.

### 14/10/2026 — conclusão solicitada: nomes destravados pelo próprio dump

- Davy pediu concluir, sem encerrar a cada checkpoint. Continuação em curso:
  +444 EN desde 10.939, total 11.383/16.100, 4.717 vazias. Não concluído.
- Falas/variantes de Yuki, epílogo, resgates, torre, navio, objetivos, cartas,
  notícias integrais da torre e de Mizuna. Adições12/13/14: 217/161/66 EN.
- Evidência interna importante: map_goal_text.luc k174 contém a chave
  MAP_GOAL_212_TO_MIZUNA_CITY e k175 seu destino JP. Usar Mizuna City, sem
  supor Mitsuna. place_text/place_data e scripts 212/214 corroboram MIZUNA.
- Leituras em recursos próprios: CHR_042_F_Fukazawa, CHR_041_M_Oda,
  CHR_034_M_Fujiwara, CHR_279_F_Hirai, CHR_273_F_Kumi (motion/NPC/head).
  Naoki em stage_210_npc.luc k614/k1238 e loading_image_and_text.csh r72
  no mesmo encontro com jovem ferido. Sobrenome não autoriza adivinhar prenome.
- Mukojima: libnpc.luc k408-410 e stage_214_npc.luc k88-90 usam Mukoujima;
  forma editorial omite vogal longa, como os demais nomes. Arquivos de voz
  usam também mukousima; preservar evidência para revisão humana.
- Auditoria dos 173 JSONs e reconstrução/releitura das 11.383 EN PASS em
  117 recursos. en-05 segue aplicada em 10.721; nenhum jogo iniciado.
  Próximo: artigos de figurinos 0154, nomes restantes, revisão transversal/HUD.

### 14/10/2026 — continuação das notícias da Sky Garden Bridge

- Após o checkpoint anterior, três lotes de artigos completos (+51 EN), mais
  cinco reutilizações revisadas de original exatamente idêntico/inglês unívoco.
  Nesta retomada +185; consolidado 10.939/16.100, 5.161 vazias. Incompleto.
- Telefonia de emergência, sobreviventes, restaurantes, transporte e lendas
  da ponte; especulação/humor/alegações ficcionais preservados como narrador.
  Artigos com leituras de nomes/bylines não comprovadas permanecem pendentes.
- Adições11: 56 em cinco recursos, sem original JP. Auditoria dos 173 JSONs
  e reconstrução/releitura das 10.939 EN em 117 recursos PASS. União com pilotos
  validada em memória: 11.069; nenhum arquivo de jogo gerado nesta etapa.
- en-05 permanece aplicada em 10.721 EN; revisão visual/humana pendente.
  Diário, índice de lacunas e consolidado privado atualizados para retomada.

### 14/10/2026 — continuação: porto, resgate e acessórios

- Davy autorizou continuar no modelo atual. Quatro lotes novos, +129 EN:
  10.883/16.100 preenchidas, 5.217 vazias. Inglês ainda incompleto.
- Notícias completas do porto e armazéns, diálogos de resgate da Yuki,
  nomes/descrições de acessórios. Humor e exageros ficcionais preservados;
  leituras desconhecidas, registros provisórios e conteúdo interno não inflados.
- Auditor barrou uma quebra de linha em 0079:k158 antes de qualquer gravação;
  corrigida e lote reaprovado. Auditoria dos 173 JSONs e reconstrução/releitura
  das 10.883 EN em 117 recursos PASS. Adições08/09/10 contêm 60/31/38 textos,
  somente tradução/metadados permitidos; união com pilotos validada em memória.
- en-05 aplicada permanece em 10.721 EN; novos lotes preparados no patch.
  Revisão humana/HUD e textos em imagens/vídeos permanecem pendentes.
  Nenhum jogo iniciado, dump ou saves alterados. Checkpoint privado atualizado.

### 14/10/2026 — retomada após aplicação: rádio, laboratório e torre

- Davy pediu continuar a tradução. Seis lotes novos (+163 EN) e uma correção
  própria: 10.754/16.100 preenchidas, 5.346 vazias. Inglês incompleto.
- Rádio/escritório/laboratório, fichas e itens de Asano/Sawada/Sasaki, documentos
  da Kunshiran Construction (kana explícita), notícias da torre e primeiras fases.
  Artigos longos completos; variantes, parágrafos e quebras preservados.
- Gyaos confirmado no site oficial https://gamera-rebirth.com/kaiju/gyaos.html ;
  Special Vehicles Section 2 e Division 2 na página oficial de Patlabor
  https://www.playstation.com/en-us/games/patlabor-the-case-files/ .
  Termos documentados nas notas; leituras humanas desconhecidas permanecem vazias.
- Correção editorial própria: light red substituído por lightweight red em
  0125:k249, pois o JP indica vermelho e peso leve, não vermelho-claro.
  Valor anterior/motivo registrados; traduções recebidas preservadas.
- Auditoria dos 173 JSONs e reconstrução/releitura das 10.754 EN em 117 recursos
  PASS. Adições07: 163 em 17 recursos, sem original japonês; união com pilotos
  validada em memória: 10.884. Sem reconstruir o pacote a cada lote.
- en-05 permanece aplicada em 10.721, PT-BR 94. Sem jogo iniciado ou saves
  alterados. Consolidado/retomada privados salvos para continuar as lacunas.

### 14/10/2026 — inglês atualizado aplicado: en-05

- Davy pediu aplicar o inglês atual. Gerado pacote privado runtime-preview-en-05,
  10.721 entradas (10.591 do consolidado + 130 pilotos), 117 recursos.
  Fonte do dump próprio conferida; reconstrução/releitura e prova do arquivo PASS.
  SHA-256: 1f30e32d5bc9ef518c989358a2cf3e5a809fa5f774a433b0f57f8e51edf7042f.
  Tamanho: 6.348.282.243 bytes, só 1.757.184 bytes a mais que o common original.
- Manifesto do launcher aponta en-05; PT-BR preservado em 94. CheckOnly de ambos
  passou, sem iniciar jogo ou alterar saves. Teste visual desta versão pendente.
- Davy esclareceu que os 4% eram do Codex, não do jogo ou do launcher. Não
  alterar a interface por esse motivo. Nenhuma mudança de launcher feita.
- Após interrupção, pediu identificar modelo anterior antes de continuar.
  Metadados da sessão atual confirmam gpt-6.1-sol nos turnos anteriores;
  esforço anterior ausente, turno atual registra low. Não presumir esforço antigo.
  Tradução completa, revisão de HUD e imagens/vídeos permanecem pendentes.

### 14/10/2026 — continuação persistente: objetivos, recaps, itens e notícias

- Davy pediu continuar. Quinze lotes de novas traduções (+1.092 EN) e um lote
  de correção própria: 10.591/16.100 preenchidas, 5.509 vazias. Não concluído.
- Objetivos de missão 0108 (+166), resumos de carregamento 0045/0046, mapas,
  diálogos de resgate/ônibus, documentos/chaves/fotos/alimentos/figurinos e
  notícias completas 0065/0016/0039/0041. Artigos longos traduzidos integralmente,
  mantendo tom, especulações ficcionais e quebras reais/escapadas por variante.
- Reutilização de 16 textos idênticos conferidos e com inglês prévio unívoco,
  indicando ID da fonte; identificadores internos excluídos. Fichas 0023 têm
  leituras explícitas de Shibata/Muto/Kubo/Osanai/Natsuno/Aoi/Namioka/Yanagibashi/Uchida.
- Otsuka Hideyasu confirmado por kana no anúncio de personagens reproduzido
  em https://game.watch.impress.co.jp/docs/news/1064842.html ; Hideharu em resumo
  de fã diverge. Isso resolveu 88 lacunas. Outros nomes sem evidência pendentes.
- Toro/Kuro/Doko Demo Issyo conferidos em fontes oficiais PlayStation;
  Go Go Curry no próprio site; Hikari Miyamoto/Summer Lesson na página oficial
  da PlayStation Store. URLs nas notas. Títulos ficcionais ainda provisórios.
- Inventário: 2.121 entradas vazias de itens são registros provisórios literais.
  Não equivalem a 2.121 itens reais sem tradução; não foram preenchidas só para
  aumentar a cobertura. Ainda há conteúdo visível pendente e revisão humana.
- Importador recusou nova notícia 0016:k139 por duas ocorrências de Alien Zarab
  contra três no JP; corrigido nome/reporte com expected_en e motivo no histórico.
  Auditorias dos 173 JSONs PASS; reconstrução/releitura das 10.591 EN em 117
  recursos PASS. Adições05 (+700/18 recursos) e adições06 (+392/21 recursos),
  sem original japonês. União com 130 pilotos validada em memória: 10.721.
- en-04 continua 8.389, PT-BR 94; sem pacote novo, jogo iniciado ou save alterado.
  Consolidado/índice/retomada privados atualizados. HUD e imagens/vídeos pendentes.

### 14/10/2026 — nova continuação autorizada para fechar o inglês

- Davy pediu seguir até terminar, sem novas confirmações por arquivo. Inglês
  continua incompleto: 9.499/16.100 preenchidas, 6.601 vazias; cobertura bruta
  inclui créditos, variantes e registros internos, não mede jogo visível revisado.
- Onze lotes: +802 EN em cenas 0057/0126/0128/0053/0085/0030/0044/0029/0131/0084
  e lacunas de nomes em cenas anteriores; itens reais 0117 incluídos no primeiro.
  Todos com backup prévio, histórico antes/depois e auditoria de 173 JSONs PASS.
- Leitura de Miho/Shibata/Muto no material de anúncio reproduzido pela Famitsu
  (201707/14137494); Sawada/Mizuki, Sasaki/Haruomi e Asano/Kosuke na introdução
  de personagens publicada em 4gamer (20170926052). Não confundir Mitsuki de wiki
  de fã com Mizuki publicado. Ichijo/Miyato/Hananira têm kana explícita no dump.
  Ebine/Ristorante Miyato vêm de kana; Giant Shadow Weekly é título provisório.
- Correção editorial própria 0128:k522 removeu pergunta extra ausente no JP,
  mantendo a quebra original. Inglês recebido não apagado. Nomes sem evidência,
  ações ambíguas e placeholders permanecem pendentes, sem preencher por hipótese.
- Todas as 9.499 EN reconstruídas/relidas em 117 recursos do dump próprio PASS.
  Adições03 (+692, 11 recursos) e adições04 (+110, 3 recursos) exportadas sem
  campo original. União com os pilotos validada em memória: 9.629 entradas.
- Sem pacote/manifesto novo: en-04 continua 8.389 e PT-BR 94. Nenhum jogo
  iniciado ou save modificado. Revisão humana, HUD e textos em imagens/vídeos
  permanecem pendentes. Índice/checkpoint privados atualizados para continuação.

### 14/10/2026 — retorno da en-04 e continuação do inglês

- Davy testou e relatou inglês entendível, compreensão das escolhas/humor,
  citando roubo de uniforme de policial. Não é aprovação editorial integral.
  Problemas: escolhas longas não cabem; palavras partidas nas legendas;
  ponto às vezes isolado na linha seguinte. Sem causa ou correção comprovada.
- Pedido explícito de escopo: logo, aviso antes dos logos e títulos de capítulos,
  além de texto em imagens/vídeos. Classificação de cada tela ainda pendente;
  catálogo M1 não prova qual recurso contém esses exemplos. Registro completo
  em docs/m4/REVISAO-VISUAL.md, sem encurtar texto para mascarar layout.
- Quatro lotes continuaram 0061/0111/0112/0114: +438 EN, agora 8.697/16.100
  preenchidas, 7.403 vazias. Todos os 173 JSONs auditados; todas as EN em
  117 recursos do dump reconstruídas/relidas em memória PASS. Revisão pendente.
- SSSP/Ultra Guard confirmados na enciclopédia oficial Tsuburaya; URLs nas
  notas. Registros provisórios e nomes incertos não preenchidos por hipótese.
- Importador --only-new exporta adições sem original; recusa divergências
  no inglês/hash já existente. Rascunho novo só +438, dois recursos; união
  com prévia anterior validada em memória: 8.827, sem pacote/manifesto novo.
- en-04 do launcher continua 8.389; PT-BR continua 94. Nenhuma partida
  iniciada/reiniciada pelo agente. Retomada/índice/consolidado privados atualizados.
  Inglês segue incompleto. Próximo: 0117 itens reais e lacunas narrativas.

### 14/10/2026 — tradução ampliada e prévia inglesa en-04

- Retomada autorizada: nove lotes, +1.337 EN; consolidado privado agora
  8.259/16.100 preenchidas, 7.841 vazias. Auditoria dos 173 JSONs PASS;
  todas as EN reconstruídas/relidas em 117 recursos do dump próprio PASS.
  Inglês recebido preservado; 38 reparos anteriores mantidos. Revisão pendente.
- Contexto Stage4: SRT degrada após ~90s; quadros 720p inspecionados em
  1200/1800/2200s. Em 1800s, fala corresponde a 0146:k1790, menina chama Yuki
  de irmã. Não é análise integral. Kashiwagi tem leitura explícita em 0018:k290.
- Davy autorizou aplicar incrementalmente se simples. Novo importador grava
  só campos permitidos, sem original japonês; valida fontes/tokens/nomes.
  Auto-review inicial rejeitou por receio de cópia do original; lista explícita
  de saída adicionada, execução corrigida aprovada. Nenhum original versionado.
- Prévia EN en-04: 8.389 entradas incluindo 130 anteriores, 117 recursos;
  PSARC gravado/relido/hash PASS. SHA e limitações no relatório da tarde.
  CheckOnly EN/PT PASS. Nenhum jogo aberto; gameplay/layout ainda pendentes.
- Pacote inglês 6.348.273.788 bytes: só +1.748.729 bytes sobre o original.
  Sem partida ativa, removido apenas pacote EN en-03 substituído/recriável,
  liberando 6.346.561.190 bytes. Provas/traduções antigas, dump/saves/PT intactos.
- Consolidado cumulativo atualizado; CONTINUE-HERE/CHECKPOINT/índice privados
  registram lacunas e próximos lotes. Inglês ainda incompleto; PT não ampliado.
  Não chamar 51,3% de entradas de metade do jogo visível. Continuação: 0061/0111.
  Detalhes em docs/LOCALIZACAO-TARDE-2026-10-14.md e docs/ABRIR-JOGO.md.

### 13/10/2026 — índice de fonte do nó bitmap rastreado offline

- Registro GxUnitFontBmp → fábrica 0x2e4cd0 → construtor 0x2e5230/base
  0x2e4570: string +0x110, tamanho +0x19c. Caminho de desenho 0x2e5aeb
  copia +0x1a8 a parâmetros+0x10; consumidor 0x2de9f6 seleciona slot <=3
  com stride 0x100. Desmontagem limitada verificada; não executada.
- Diagnóstico HUD adiciona font_index_1a8 com guarda de leitura, sem alterar
  campo ou atlas. Fonte concreta do objetivo ainda pendente; tamanho de fonte
  não basta para identificar variante. Investigação em docs/m3/FONTES.md.
- Build normal e teste do callback/instalação HUD PASS. Nenhum jogo iniciado;
  nenhuma fonte/glifo foi substituído por hipótese.

### 13/10/2026 — seleção de save rejeita retorno fora da lista e UTF-8 inválido

- Worker da lista valida selected contra count antes de indexar o array de
  nomes; retorno inválido vira cancelamento, mantendo saída vazia. Conversão
  dos nomes usa MB_ERR_INVALID_CHARS em vez de substituir bytes inválidos.
- Teste do serviço adaptado PASS: slots novos/existentes, cancelar, retorno
  além da lista, UTF-8 inválido, confirmação e ponteiros/palavras de guarda.
  Build normal PASS, CheckOnly EN PASS, sem janela/jogo/saves reais.

### 13/10/2026 — metadados de save e identificação de montagem

- Revisão offline encontrou write_param sem retorno: falhas de gravação eram
  reportadas como sucesso e abriam param.bin diretamente com truncamento.
  Agora usa temporário exclusivo, escrita completa/flush/substituição; falha
  retorna ERR_INTERNAL e preserva metadados anteriores. Não altera payload.
- slot_of usava atoi e aceitava /savedata0junk como 0. Parser agora exige nome
  canônico completo e terminador nos 16 bytes da ABI, slots 0–15.
- Teste do backend real com saves sintéticos PASS: persistência/reabertura,
  escrita parcial injetada, substituição bloqueada por handle Windows,
  preservação byte a byte, ausência de temporários e recuperação de criação.
  ABI Mount2/build normal PASS. Nenhum jogo ou save real acessado.
- Documentação em SAVE-DATA.md. Novos headers próprios/adaptação CMake;
  third_party/godzilla-core e projeto Godzilla intactos.

### 13/10/2026 — limpeza de pacotes antigos e regra permanente

- Davy autorizou apagar o que não precisa ser mantido, como regra permanente;
  registrada em AGENTS.md. Conferir dependências/substitutos antes da remoção.
- SHA256 e tamanho dos atuais EN en-03/PT-BR 02 conferidos contra manifesto;
  runtime lê esse manifesto e não havia partida aberta. Removidos somente
  common.psarc EN/PT da prévia 01 e EN da 02: 19.039.683.103 bytes liberados
  (19,04 GB/17,73 GiB). Metadados/provas antigos mantidos; lista privada em
  build/cleanup-old-previews-20261013.json. Dump/saves/pacotes atuais intactos.
- CheckOnly EN e PT-BR PASS após limpeza, sem abrir jogo. Versões antigas
  agora exigem reconstrução se forem necessárias; não referenciá-las como
  pacotes existentes. Traduções, ferramentas e pacote de retorno GPT mantidos.

### 13/10/2026 — consolidação dos seis avanços offline

- Retomada atualizada com commits, testes e pendências; build normal e
  diagnóstico pending compilados. CheckOnly HUD FitHud e orbe PASS. Hashes
  privados em build/offline-checkpoint-20261013.json. CMake voltou à saída
  normal build/kyoei.exe. Última consulta sem processo Kyoei em execução.
- Nenhuma partida aberta, dump/saves/core fixo/Godzilla preservados. Retorno
  de tradução externo ainda ausente; pacotes entregues ao GPT não alterados.
- Orbe, ancoragem visual, fonte do objetivo/à e FPS permanecem sem confirmação
  de correção. Não usar metadados de atlas ou teste sintético como gameplay.

### 13/10/2026 — trabalho offline: inventário completo de páginas de fonte

- Inspeção anterior via pasta parcial mostrava só TEXs 0. Auditoria direta do
  PSARC próprio encontrou 21 páginas: 15 utf8all e seis IM, todas R8 linear,
  2048×2048, um mip. Nenhum pixel extraído/alterado e nenhum pacote reconstruído.
- Principal/IM usam índices 0/1/2; seis páginas correspondentes presentes e
  todos os retângulos das tabelas dentro dos limites. Variantes usam até 24
  páginas, sem arquivos do mesmo prefixo: compartilhamento/uso ainda não provado.
- Oito hashes de tabela no PSARC coincidem com auditoria anterior. Metadados
  somente em docs/m3/font-pages-audit.json; utilitário audit_font_pages.py
  reproduz a leitura. Não versionar atlas/desmontagem/texto original completo.
- ASCII/pontuação liberada em FitHud confirmados na página 0. Ainda não corrige
  o à ausente, não identifica fonte do objetivo e não substitui atlas por palpite.

### 13/10/2026 — trabalho offline: guarda única contra partidas duplicadas

- Proteção movida para run_native.ps1, caminho comum do launcher e dos atalhos.
  Inclui executável normal e ambos os diagnósticos; antes atalho HUD desconhecia
  o diagnóstico next e launcher direto não tinha a mesma guarda.
- Usa nomes Win32_Process, como a guarda já corrigida do orbe. Não fecha nada,
  apenas recusa nova abertura. CheckOnly continua independente de partida ativa.
- CheckOnly HUD/orbe PASS. Bloco real da guarda avaliado isoladamente via AST
  com três processos sintéticos: todos bloqueados; estranho/CheckOnly permitidos.
  Nenhum trecho de abertura foi executado nesse teste.

### 13/10/2026 — trabalho offline: medições identificam custo do F9

- Janelas frame/work stats agora incluem diagnostic=1 quando cruzam trabalho
  da captura F9. Tempos brutos e frames lentos permanecem registrados; não
  excluir outliers nem apresentar pausa do diagnóstico como FPS normal.
- Estado/geração atômicos detectam captura inteira entre duas medições e
  captura dentro de uma chamada. Janela seguinte limpa volta a diagnostic=0.
- Primeira janela de TLS agora usa baseline do primeiro present, sem contar
  faults da inicialização como se pertencessem aos 120 intervalos medidos.
- Build normal PASS; teste real das métricas, modo desligado e regressões do
  raster PASS sem jogo. Não há novo ganho de FPS demonstrado.

### 13/10/2026 — trabalho offline: custo e evidência da captura gráfica

- Deduplicação do F9 indexa candidatos por chave e confirma igualdade completa:
  conserva ordem, repetições e limite de perfis. Colisões não fundem desenhos.
  Antes toda chamada varria todos os perfis; teste de mil shaders distintos
  faz zero comparações completas nas inserções e uma por repetição. Não é FPS.
- Regressão força colisão com geometria diferente, testa contagens/ordem e
  repetições; teste nativo/disabled PASS. Build normal e diagnóstico pending PASS.
- Parser examina também corte direito, ignora diferença de até um pixel e
  exibe contagem de amostras de constantes. Onze regressões PASS; relatório
  privado 06 reanalisa 172251. Candidatos direitos: somente viewport 16.384,
  modo de clipping desabilitado; não provar defeito com essa diferença esperada.
- CheckOnly diagnóstico PASS; configuração voltou a kyoei em build/. Nenhuma
  abertura de jogo. Coleta de constantes real continua pendente para o orbe.

### 13/10/2026 — trabalho offline: pontuação UTF-8 nas caixas e minimapa

- Ajuste FitHud agora mede seis glifos já existentes em ambas as fontes:
  aspas simples/duplas curvas, reticências e nota musical. Tabelas auditadas
  demonstram advance exatamente full-width; gerador de métricas verifica isso.
  Não remove reticências narrativas e não habilita acentos ausentes nas variantes.
- Medida do minimapa separa comprimento UTF-8 em bytes da contagem nativa de
  caracteres; antes um apóstrofo curvo impedia a medição e caía no original.
  Leitura limitada ao trecho de memória acessível e a 115 bytes. Códigos,
  caracteres não cobertos e UTF-8 truncado/inválido mantêm fallback original.
- Build PASS, testes de largura, callback real e instalação com assinaturas do
  eboot próprio PASS, sem chamar jogo. Cache privado 02 idêntico em formato ao
  01; ambos têm as mesmas medidas ASCII. CheckOnly FitHud PASS. Visual pendente.

### 13/10/2026 — trabalho offline: edição completa pelo teclado

- Davy pediu continuar durante a aula sem abrir o jogo. Sem novas capturas
  após 172251; não exigir testes dele neste período nem alterar diagnóstico
  gráfico por hipótese. Não iniciar nenhuma partida.
- Entrada de nomes no overlay agora trata Delete, Home e End. Delete mantém
  cursor e apaga o caractere seguinte inteiro, inclusive par UTF-16; seleção
  total é apagada inteira. Navegação existente por controle preservada.
- Build normal PASS; modelo de nome, overlay real em 640×360 e adapter IME
  via ABI PASS. Edição de sequência A/emoji/é/Z verificada sem janela, saves
  ou execução de código do jogo. Visual/gameplay permanece pendente.

### 13/10/2026 — filtro de constantes corrigido com recurso real do renderer

- Captura 20261013_172251: 1.885/1.781 perfis, sem perdas, mas zero uniform_samples.
  F9 de Davy funcionou; diagnóstico excluiu todos os buffers pelo filtro errado.
- Causa demonstrada no código fixo shader_recompiler/resource.h: IsStorage
  retorna true inclusive para constantes somente leitura. Corrigido filtro para
  recurso Guest e !is_written; GPU dirty/limites/falha continuam protegidos.
- Regressão usa Shader::BufferResource real: storage/read-only elegível;
  escrito e especial rejeitados. Teste nativo/disabled PASS, parser nove PASS,
  CheckOnly PASS. Parser registra ausência de constantes explicitamente.
- Build corrigido em build/orb-pending, sem substituir executável aberto do PID
  21636. Mesmo Testar orbe constantes EN.cmd aponta para nova versão. Nenhuma
  partida iniciada/encerrada. Próxima coleta requer abertura pelo usuário.
- Captura passiva orb-user-20261013-04.png: foto íntegra, contexto preservado.
  Relatório 05 não permite avaliar transformação: não declarar faixa corrigida.

### 13/10/2026 — corte do orbe mudou de lado; constantes no instante do desenho

- Davy registrou F9 e informou faixa agora na direita. Log 20261013_162330:
  1.957/1.707 perfis completos, sem descartes. Captura passiva da janela PID
  32984 confirma faixa direita e foto íntegra. Partida mantida aberta/intacta.
- Recortes seguem cobrindo tela; geometria selecionada distingue caixas/foto.
  Painel local 200×1.080 usa textura 16×16, presente antes e durante: não é
  prova de que seja camada defeituosa. Limites locais não são pós-shader.
- Leitura posterior limitada de constantes por ReadProcessMemory foi possível,
  mas buffers podem ter sido reciclados; não correlacionar esses valores ao F9.
  Preparada coleta no próprio BindBuffers: até oito buffers, 64 bytes cada,
  só uniforms VS/PS de draws com blend; sem waits nem storage/GPU dirty.
- Build kyoei-orb-trace-next.exe PASS sem substituir executável do PID ativo.
  Testes tamanho/overflow/falha/dirty/disabled PASS e parser nove PASS. CheckOnly
  do novo atalho PASS. Normal SHA preservado; CMake restaurado para kyoei.
- Testar orbe constantes EN.cmd disponível para próxima abertura pelo usuário;
  nenhuma partida iniciada ou fechada pelo agente. Causa ainda não provada.

### 13/10/2026 — segunda captura do orbe e seleção real de vértices

- Log de Davy 20261013_160452: duas capturas completas, 1.764/4.114 desenhos,
  839/1.133 perfis, nenhum descarte e nenhum recorte à esquerda. Relatório
  privado build/orb-raster-report-03.json. Causa da camada ainda não demonstrada.
- Comparação por target/shaders/primitive/blend/depth/viewport/scissor: 188
  estados comuns, 55 só antes, 74 só durante. Não equivalem a objetos visíveis;
  há dois frames e targets alternados. Não atribuir defeito só pela diferença.
- UI compartilha buffers grandes; quatro primeiras entradas repetidas em
  centenas de desenhos não descrevem suas caixas. Complemento agora seleciona
  índices 16/32 bits com deslocamento/base vertex efetivos, preservando renderer.
  Amostra até quatro entradas do desenho; não é geometria pós-shader nem total.
  Draw indireto fica explicitamente indisponível, sem reaproveitar seleção velha.
- Build separado PASS; testes de índice 16/32, base negativa válida, deslocamento
  direto, fora do buffer, GPU dirty e indisponível PASS. Parser oito PASS;
  CheckOnly PASS e SHA normal preservado. Nenhuma partida iniciada. Próximo
  registro F9 pelo mesmo atalho precisa de dados selected_xy antes/no tutorial.

### 13/10/2026 — leitura das capturas do orbe e complemento de geometria

- Davy informou capturei. Log 20261013_143716 analisado: registro 1 atingiu
  limite 256 e perdeu 1.045 ocorrências; registro 2 capturou 255 perfis sem
  perdas. Janela/recortes do tutorial não mostram a faixa deslocada; offsets
  zero. Não fazer mudança global de scissor. Causa permanece desconhecida.
- Parser distingue log completo de sampling_complete; 38 perfis exclusivos
  do registro 2 não são necessariamente novos no tutorial, pois baseline
  perdeu passes. Relatórios privados em build/orb-raster-report-01/02.json.
- Diagnóstico ampliado para 2.048 perfis, posições de buffers float compatíveis
  e parâmetros dos shaders. CPU reads limitados, sem espera GPU; dados GPU
  modificados/incompatíveis/indisponíveis marcados, falha não vira zero válido.
- Teste de registradores/coordenadas/cópia e modo desligado PASS; oito testes
  do parser PASS; build separado PASS. Normal preservado. Próximo teste F9
  para separar geometria de amostragem/composição, sem presumir correção.

### 13/10/2026 — diagnóstico do orbe pronto, acionado por F9

- Davy autorizou preparar o diagnóstico. Build separado kyoei-orb-trace.exe
  pronto, mantendo SHA256 do kyoei.exe normal. Código do core fixo intacto;
  rasterizer adaptado em cópia de build, sem alterar cálculos/efeitos.
- F9 captura dois quadros de viewport/scissor/offset/target/shaders por
  endereço/blend/depth. Perfis deduplicados, limite 256/descartes visíveis.
  Desligado por padrão. Teste com Regs real e modo desligado PASS, sem GPU.
- Testar orbe EN.cmd e scripts/test_orb.ps1 prontos; CheckOnly PASS. Davy
  fará F9 antes da coleta e outra vez na tela do tutorial, sem enviar logs.
- Leitor de capturas e seis regressões PASS; candidatos de recorte não são
  prova da causa. Próximo passo comparar os dois registros reais. Não
  executar jogo automaticamente; não medir FPS durante a captura diagnóstica.

### 13/10/2026 — bug do orbe esclarecido e retorno dos testes

- Davy pediu observar o defeito e esclareceu camada alaranjada que não cobre
  toda a tela/deixa faixa limpa na esquerda. Duas capturas do PID 26124 sem
  foco/controles; foto dos três orbes comparada com original e íntegra.
  Registro preciso e próximo diagnóstico em DIAGNOSTICO-GRAFICO.md.
- Partida do usuário às 13:51:13, log 20261013_135113, FitHud/BatchReadback;
  outra às 13:46:25. Preservar executável/processo em uso. Nenhuma partida
  iniciada pelo agente e nenhum shader/recorte alterado por palpite.
- HUD registra casos 28 recebidos/28 exibidos: restauração de texto completo
  comprovada no motor. Font size +0x19c confirmado 38 no runtime. Resultado
  de ancoragem/clipping nas bordas ainda não certificado visualmente.
- Lotes de readback registrados: uma imagem/uma espera/256 bytes, sem
  redução de waits nesses exemplos. ~29 FPS no tutorial parado não prova
  ganho comparado à rua em movimento. Não promover agrupamento como ganho.

### 13/10/2026 — minimapa na prévia e comparação de readback

- Mesma prévia FitHud adapta dois consumidores locais de largura do minimapa
  (extensão do rótulo e placa), por advances normalizados em vez de quantidade
  de letras. Rotação/seta/clamps originais preservados, assinaturas verificadas.
  Medição/fallback/ABI e instalação nos blocos reais PASS; gameplay pendente.
- Comparação opcional pronta em Testar desempenho EN.cmd: FitHud e
  BatchReadback, mantendo o padrão desligado. Agrupamento já existia; testes
  confirmam fence/publicação/reuso do ring. Sem ganho de FPS declarado.
- Contadores agora observam também caminho de waits individuais, sem inserir
  esperas; teste de observação/escopos aninhados PASS. CheckOnly da configuração
  PASS. Comparar mesma cena com Testar HUD ajustada EN.cmd (agrupamento off).

### 13/10/2026 — prévia concreta de ajuste da caixa de interação

- Prévia opt-in EN restaura texto completo pelo setter nativo assinado e
  ajusta largura/espelhamento com métricas conservadoras das duas fontes do
  dump. Cache privado em build; original/pacotes/saves intactos.
- Largura/limites e callback com String/nós sintéticos PASS; relay/instalação
  PASS, build kyoei e CheckOnly PASS. Nenhum gameplay iniciado. Atalho
  Testar HUD ajustada EN.cmd preparado para comparação de Davy.
- Fonte/escala/bordas requerem teste visual. Prévia limitada a ASCII sem
  códigos/largura máxima; minimapa/performance continuam pendentes.
- Davy usa terminal: baixar retornos GPT em
  G:\kyoei-toshi-work\translation\retorno-gpt e informar o caminho aqui;
  não depender de anexos no PowerShell.

### 13/10/2026 — português para o outro GPT após o inglês

- Davy informou que o lote inglês já está rodando no outro GPT. Preservado
  external-en-full-01; preparado external-pt-full-01 com 16.136 candidatos,
  94 rascunhos PT como contexto e 173 JSONs. Inglês primeiro, depois PT-BR.
- Exportador/importador suporta idioma separado, proteção de nomes/tokens
  e duplicidade por idioma. Nove testes PASS; ZIP/IDs e fixture real de
  importação PT com acentos/reinserção PASS, sem sobrepor EN existente.
- Retorno combinado ou em lotes pode ser anexado nesta conversa; o agente
  audita antes de publicar pacote. Nenhuma tradução nova ativada no jogo.

### 13/10/2026 — primeiro teste diagnóstico feito por Davy

- Davy iniciou Testar HUD EN.cmd por conta própria às 13:03:54, PID 5432;
  log native-05/logs/20261013_130354.log. Nenhum comando de gameplay enviado.
- Medição inicial tinha menus/transições (pausas até ~434 ms); depois há
  janelas na rua de ~21–29 FPS, com readbacks lineares ~10,5–11 ms médios.
  Prepare/present geralmente menores que 1 ms. Sem comparação da mesma cena
  com versão anterior/diálogos desligados, não atribuir regressão à tradução.
- Diagnóstico funcionou, mas 24 registros foram consumidos repetindo Listen:
  seis caracteres, placa scale_x=5,7222, x=24 e texto x=50. Ajustado para
  registrar mudanças de conteúdo/nó, usando comprimento em bytes verificado
  no setter nativo 0x31e4a0 e hash sem copiar fala para o log. Campo +0x19c
  e vizinhos adicionados para investigar tamanho/fonte.
- Davy perguntou se precisava fazer mais: informado que não por enquanto.
  Partida deixou de aparecer depois; build novo PASS só após conferir isso.
  Relay/instalação offline PASS. HUD continua sem correção visual aplicada.

### 13/10/2026 — diagnóstico concreto para teste da HUD

- Diagnóstico opt-in da rotina cmnd_marker 0xa6d9a0 preparado com assinatura
  completa, relay SysV e limite de 24 registros. Preserva comportamento
  original; observa contagens, geometria e campos de fonte, sem falas no log.
- Teste do relay/retorno e instalação sobre bloco real PASS, sem executar
  código do jogo. Build kyoei PASS, depois de confirmar ausência de partida;
  CheckOnly EN/1280×720/diálogos/diagnóstico PASS. Nenhum jogo iniciado.
- Testar HUD EN.cmd pronto para Davy, que ofereceu testes em casa. Não
  considerar corte/minimapa corrigidos; próximas evidências são fonte/escala
  efetivas e FPS comparável. Entrega externa permanece intacta.

### 13/10/2026 — entrega externa de tradução do catálogo completo

- Davy pediu arquivos para outro ChatGPT rascunhar inglês enquanto continua
  a investigação da HUD. Pacote privado external-en-full-01 preparado com
  16.100 candidatos, 138 recursos/20 Text_Stage, 173 JSONs; ZIP verificado.
  130 rascunhos existentes seguem como contexto. Texto em imagens/vídeos,
  HUD binário e encodings permanece pendente; não alegar cobertura total.
- tools/external_translation.py exporta somente para fora do Git/dump/M1 e
  importa retorno com hashes, IDs, tokens, nomes e reinserção verificados,
  sem trazer originais ao patch. Oito testes PASS e fixture real PASS.
  Entrega não será modificada enquanto Davy a envia; ver TRADUCAO-EXTERNA.md.
- HUD: comprovada carga cmnd_marker em 0xa6cf3b e ligação do nó texto ao
  truncador 0xa6d9a0. Restam métricas/escala reais e limites de tela antes
  de aplicar correção. Não trocar esse trabalho por tradução sem concluí-lo,
  conforme orientação posterior de Davy.

### 13/10/2026 — retorno ao vivo de Davy, 12:38 Bahia

- Investigação posterior encontrou truncamento anterior ao desenho na rotina
  0xa6d9a0: >=13 caracteres vira primeiros 11 + U+2026, produz exatamente
  Listen to t…; largura calculada por contagem. Sem patch especulativo no
  executável. Próximos chamadores/pontos de prova em HUD-INVESTIGACAO.md.

- Davy abriu uma partida EN pelo launcher (PID 28332, log
  native-05/logs/20261013_122032.log), 1280×720 com InGameDialogs. Não iniciada
  pelo agente; preservar processo, executable e pacote em uso.
- Capturas sem foco/controles confirmam corte da interação e do objetivo.
  Evidência e limites em docs/m3/HUD-INVESTIGACAO.md; ancoragem em movimento
  ainda não comprovada. Davy também relatou lentidão, sem FPS comparável.
- Corrigido rascunho EN de Peek inside para Take a closer look: ação usada
  no homem abrigado da chuva, verbo sem interior explícito. Validação EN PASS;
  nenhuma reconstrução/publicação do pacote durante a partida.
- Overlay: progresso sem escolhas agora ignora X/Esc, como a apresentação
  nativa; fechamento via backend permanece. Regressão no modelo real PASS.
- Teste dos descritores Vulkan reais PASS: transição direta/modal, Load/Store
  e retorno a Present. Ponteiro persistente do formato Vulkan e checkbox do
  launcher visível somente em EN preparados; sem substituir o binário ativo.

Memória do projeto entre sessões. Ler antes de começar cada sessão.

---

## 08/10/2026 — fechamento técnico do M0

- O diário estava vazio; havia inventário e importações ainda sem commit.
- Fonte local: `G:\GodzillaShadPS4PC\launcher\game_data\CUSA03562`.
- Identificação lida diretamente: CUSA03562, APP_VER 01.03, VERSION 01.00.
- Validados novamente os 87 arquivos (17.798.062.971 bytes), incluindo 25
  PSARC; todos os SHA-256 coincidiram com o inventário existente.
- SFO, SHA256SUMS e hashes das ferramentas também conferidos.
- Ferramenta oficial `G:\godzilla-port-codex\tools\imports.py` executada
  novamente: JSON idêntico, 704 importações, 61 módulos declarados,
  63 bibliotecas declaradas e 10 NIDs sem nome conhecido.
- Adicionados `tools/verify_m0.py`, `tools/report_m0.py` e resumo em
  `docs/m0/README.md`; relatório legível regenerado em UTF-8.
- Nenhum arquivo do dump alterado ou incluído no Git. A regra de usar
  exclusivamente o dump próprio do Davy permanece; hashes não certificam origem.
- M0 técnico validado. Próximo marco: M1, leitura e extração dos PSARC
  fora do repositório, com catálogo dos formatos encontrados.
- Não iniciado M6 e não copiado código compartilhado do Godzilla.

## 08/10/2026 — M1, extração dos PSARC

- Implementado leitor independente da variante PSARC encontrada nos 25
  arquivos: PSAR 1.4, zlib, blocos de 64 KiB, TOC em claro, flags 0x0D.
- Manifesto usa separadores nulos. A ordem do manifesto difere da TOC;
  ligação comprovada por MD5 dos caminhos em maiúsculas ASCII.
- Extraídos 72.961 arquivos (19.327.950.739 bytes) para
  `G:\kyoei-toshi-work\m1`, fora do repositório e do dump.
- Antes da extração, os SHA-256 dos 25 PSARC foram comparados com o M0.
- Releitura de todas as 72.961 cópias concluída com sucesso: tamanhos e
  SHA-256 idênticos aos calculados durante a decodificação dos PSARC.
  Os 25 pacotes terminaram com `VERIFICADO`; M1 concluído tecnicamente.
- Nove testes sintéticos passaram: blocos brutos e zlib, nomes fora de
  ordem, arquivos vazios, caminhos inválidos, colisões no Windows,
  truncamento, hashes de nomes, offsets de 40 bits, índices inválidos,
  corrupção zlib e rejeição de alterações no catálogo/extração de prova.
- Candidatos a textos: `lua/*.luc` e tabelas `csv/*.csh`, incluindo itens,
  dicas de carregamento, materiais de leitura e textos de objetivos/notícias.
  Os contêineres internos ainda precisam ser documentados no M2.
- Fontes candidatas em `font/utf8all*`, tabelas `.csh` e atlas `.tex`;
  cobertura de acentos ainda não comprovada (M3).
- Encontrados bancos de som com assinaturas @UTF/AFS2, três vídeos USM
  com CRID e um MP4; modelos, animações, colisões, interface e shaders.
- Ferramentas: `psarc.py`, `catalog_m1.py`, `report_m1.py`, `verify_m1.py`
  e `test_psarc.py`. Documentação em `docs/m1`; catálogo detalhado local
  em `work/m1/catalog.json`, ignorado pelo Git. Seu SHA-256 está registrado
  em `docs/m1/formats.json`, junto com contagens e assinaturas por extensão.
- Listagem sem extração e sintaxe das ferramentas finais também conferidas.
  A ferramenta de releitura agora usa buffer menor e mostra progresso;
  `verify_m1.py` permite repetir a prova sem uma nova extração.
- Nenhum texto completo ou dado do jogo adicionado ao repositório.
  Não feita reinserção, tradução, alteração de fonte ou trabalho no M6.
- Próximo marco: M2, documentar contêineres `.luc`/`.csh`, extrair e
  reinserir textos, começando com a prova sem alterações no shadPS4.

## 08/10/2026 — M2, ferramentas e prova de identidade

- Documentados envelopes BILZ, bytecode Lua 5.2 variante float32 e tabelas
  CSH. Corrigida a interpretação da tag CSH: três bits de tipo, 29 bits
  de comprimento. Dois valores opacos de oito bytes em item_data são
  preservados; o significado deles ainda é desconhecido.
- Implementadas extração e reinserção por IDs estáveis, com SHA-256 da base,
  verificação dos originais, preservação de dados não textuais e rejeição
  de IDs inválidos. Textos originais exportados ficam somente fora do Git.
- Auditoria completa: 2.032 CSH e 299 LUC, total 2.331 recursos; 235.069
  ocorrências de strings e 17.866 candidatas com caracteres japoneses.
  Zero falhas; todos idênticos após ida e volta estrutural e via JSON.
  Essas contagens incluem identificadores e listas; não são falas únicas.
- Seis testes sintéticos de texto e quatro de atualização PSARC passaram,
  incluindo acentos, crescimento, redução, aliases, dados opacos, bytecode,
  hashes, corrupção, blocos zlib/brutos e preservação de recursos vizinhos.
- Atualizador PSARC preserva o corpo original e anexa recursos alterados,
  ajustando a TOC. Sem alterações, mantém o pacote inteiro byte a byte.
- Construída cópia completa em
  `G:\kyoei-toshi-work\m2\prova-01\CUSA03562` após extrair e reinserir
  os 2.331 recursos. Os 87 arquivos (17.798.062.971 bytes) coincidem em
  tamanho e SHA-256 com o M0. Relatório em `docs/m2/dump-proof.json`.
- Tentativa isolada com shadPS4 instalado, usando diretório local separado
  `G:\kyoei-toshi-work\m2\shad-prova-01`; não foram copiados saves.
  O processo 37520 ficou num diálogo antes de iniciar o jogo, com log vazio.
  Backtrace confirmou espera em DialogBox; o processo depois encerrou sem
  uma janela de jogo confirmada nessa tentativa.
- Segunda tentativa com janela visível: shadPS4 0.16.0 reconheceu CUSA03562
  01.03 e executou a cópia até a cena inicial. Captura inspecionada mostra
  cenário renderizado e narração japonesa legível. Hashes do executável e
  da captura registrados em `docs/m2/runtime-proof.json`; imagem só local.
  Log contém erros sceKernelStat (arquivo não encontrado); não afirmar
  execução sem erros nem regressão completa. Não comparada execução da base.
- M2 NÃO concluído: execução inicial da cópia foi observada, mas falta
  confirmação da cobertura de todos os textos visíveis. Nada de tradução definitiva,
  alteração de fontes ou M6. Nenhum dado do jogo adicionado ao Git.
- Ferramentas e limites documentados em `docs/m2/README.md`.

## 08/10/2026 — reaproveitamento nativo do Godzilla

- Davy pediu priorizar a base do Godzilla seguindo Bloodborne/P.T.
  Conferido diário oficial: GPU funcionando, missões/ondas, controle e
  saves aprovados; condição M5 para iniciar M6 do Kyoei atendida.
- Primeiro snapshot recusou mudanças não salvas da sessão do Godzilla.
  Depois do commit oficial, fixado `e976b59a3f056c5ea5d0ddfe1c5ef566cce2f45e`
  em `third_party/godzilla-core`, ignorado pelo Git. Fonte oficial não editada.
- Referências fixas: Bloodborne `daa71654083b42ea0730b3f0fecee2fc20942d02`
  e shadPS4 `3d03375a5bbfa52772a2142e151d6b73af48dc8d`. Hashes em
  `docs/godzilla-core.json`; licença GPL-2.0 incluída na raiz.
- Reaproveitadas bibliotecas gráficas já compiladas, copiadas com hashes.
  Compilados aqui carregador/runtime e adaptador de identidade CUSA03562.
  Nenhum fonte da cópia fixa alterado.
- Preparador usa código fixo com política Kyoei: SFO 01.03, hashes M0 dos
  executáveis usados e hash da imagem. Não ignorada checagem do jogo.
- Primeira preparação parou ao imprimir título japonês com cp1252,
  deixando native-01/out parcial. Adaptador corrigido para UTF-8;
  preparação completa em `G:\kyoei-toshi-work\native-02\out`.
- Eboot: imagem 22.408.592 bytes, 704 importações e 82.840 relocações.
  Com módulos ligados, tabela tem 939 importações e 335 ligações nativas;
  isso não comprova que as demais estejam todas resolvidas pelo runtime.
- `build/kyoei.exe` compilado e executado sem iniciar shadPS4.exe.
  App0 explícito do Kyoei; usuário isolado em native-02/user.
  Saída 1 antes da janela: carregador rejeita TLS estático do eboot.
- TLS medido: vaddr 19.906.656, filesz 0, memsz 40, alinhamento 8.
  Bloodborne já possui suporte ao TLS principal; adaptação Windows falta.
  Não removida a rejeição para executar com TLS inválido.
- Conferidos 1.834 arquivos da base fixa, todos com hashes intactos.
  Provas em `docs/native-prepare.json` e `docs/native-proof.json`;
  integração e próxima correção em `docs/PORT-NATIVO.md`.
- M6 iniciado, ainda SEM janela/jogo nativo. A correção compartilhada deve
  entrar primeiro na sessão do Godzilla, depois renovar a cópia daqui.
  P.T. é referência futura, sem código incorporado nesta etapa.

## 08/10/2026 — resultados M2 preservados antes da mudança de foco

- Índice local text-index-01.jsonl: 17.866 ocorrências japonesas UTF-8,
  13.712 strings únicas em 140 recursos. Metadados em docs/m2/text-map.json;
  texto completo só fora do Git. Categorias por nomes ainda são indicativas.
- Há 4.447 strings não UTF-8, principalmente flags/configurações; amostras
  aparentam codificação japonesa antiga, ainda não certificada. Preservadas.
- sceKernelStat também apresentou erros com o dump original. Rastreio
  mostrou consultas masterPS4/finalizedPS4 e DAT/module.psarc. Não criados
  arquivos falsos nem alterado o dump.
- Cópia no emulador terminou com Out of order flip IRQ depois da abertura;
  FIFO também reproduziu a falha. Original foi fechado normalmente após
  cerca de seis minutos; não afirmar reprodução da falha gráfica nele.
  Log separado original-baseline.log preservado fora do repositório.
- A tentativa de versão alternativa foi interrompida pelo pedido de mudar
  o foco. Conferência posterior não encontrou shadPS4 aberto. M2 continua
  pendente; não foram aprovados M3/M4.

## 08/10/2026 — autorização de desenvolvimento independente e TLS local

- Davy corrigiu explicitamente a regra anterior: o Godzilla é a base de
  partida, NÃO uma dependência de aprovação ou de receber correções antes
  do Kyoei. As melhorias necessárias são feitas e testadas aqui. Esta
  orientação substitui a exigência anterior de mudar primeiro o Godzilla.
- Snapshot oficial continua intacto. CMake gera uma adaptação local do
  loader em build, com validação TLS, configuração do template e handler
  de FS; implementação própria em src/kyoei_tls.c/h. Nenhum fonte oficial
  ou da cópia fixa foi alterado.
- TLS variante II segue o desenho do Bloodborne, adaptado para FLS Windows:
  template, parte zerada, alinhamento e bloco independente por thread;
  destruição automática na saída. Módulo 1 atendido por __tls_get_addr.
  A instrução medida mov r64, fs:[0] é tratada somente em código guest;
  GS/TEB do Windows permanecem intactos. Outros acessos não suportados
  continuam falhando explicitamente, sem valores inventados.
- Teste Windows real PASS: template de 8 bytes, total 40 bytes, padding
  zero, alinhamento e duas threads simultâneas com valores independentes;
  execução de instrução FS real e retorno do TCB. Binário test_kyoei_tls.
- Primeiro boot após TLS revelou três nomes vazios no aerolib. Adaptador
  do preparador normaliza só o símbolo textual como ?, preservando NID,
  biblioteca e posição. Reparadas as posições 95, 105 e 107 da saída local.
- Implementado sceLibcHeapGetTraceInfo com layout de 32 bytes e tabelas
  persistentes de mask/64 estados segundo kernel.cpp da referência.
  Implementado sceKernelIsNeoMode como PS4 padrão (0).
- Boot real: janela SDL 1280x720, Vulkan na RTX 2060, módulos libc/Fios2,
  fontes e áudio inicializados; entrou no eboot+0x80. 357 importações pelo
  runtime, 335 nativas e 247 ainda sem implementação nesse estágio.
- Após consulta do modelo e carga dos módulos de sistema, próximo bloqueio
  sceKernelLoadStartModule para /app0/module/prx/libSceSecure.prx, presente
  no M0 (119.054 bytes, SHA256 50196e67ca822d77d5fa5c47651ef08f25af4bdd5a232d107085ac1a777d8251).
  Não simulado sucesso: ainda falta suporte à carga desse módulo.
- TLS inicial superado; cena do jogo ainda não renderizada e M6 não concluído.
  Logs externos native-02/logs/20261008_203518.log, 203728.log e 203811.log.
  Não aguardar mudanças do Godzilla para continuar este projeto.

## 09/10/2026 — primeira imagem do jogo no port nativo

- Pedido de Davy: abrir o jogo na tela dele. Implementações próprias aqui,
  preservando Godzilla e snapshot; nenhum executável shadPS4 foi lançado.
- Preparação native-04 inclui libc/Fios2/NpToolkit2 do dump com hashes M0,
  firmware externo já existente e Secure ligado com inicialização adiada.
  `deferred.tsv` guarda endereços relativos e NIDs externamente. LoadStartModule
  chamou o inicializador real do Secure (retorno 0); resolução Dlsym por
  SHA1/salt/base64 PS4, handles próprios e rejeição de módulos desconhecidos.
- Adaptadas consultas de PlayGo (dump totalmente instalado), DiscMap local,
  carga interna de módulos, modo padrão/sem ASAN, usuários e serviços offline.
  Corrigidas diferenças de nomes no VideoOut e na configuração de screenshot.
  NpToolkit2 iniciou suas threads; callbacks de usuário/rede não anunciam
  transições de serviços online inexistentes. Troféus sem banco registrado
  retornam erro NOT_REGISTERED; gravação de vídeo não está disponível.
- Consulta de armazenamento usa espaço real via GetDiskFreeSpaceExW.
  Corrigida a interpretação de gz_widen (0 significa sucesso).
- Primeira abertura: `native-04/logs/20261009_011201.log`. Captura externa
  `native-04/window-01.png`: logo de Kyoei sobre cidade, conferido visualmente.
  DualSense reconhecido, botões físicos entregues ao jogo. Depois houve parada
  explícita na consulta de DLCs: 815 leituras / 104.056.609 bytes até essa falha.
- Implementado GetAddcontInfoList como lista vazia para a sessão sem DLCs;
  reaberto em `20261009_011550.log`. Captura `native-04/window-02.png` mostra
  prédios e a silhueta de monstro na abertura, confirmando renderização real.
  Janela trazida para frente e renomeada Kyoei Toshi - Port para PC.
- Compilação concluída; teste TLS real PASS novamente; diff --check sem erro.
  `native-current.json` registra hashes do executável e captura externa,
  e o hash do log no momento da consulta (o processo ainda escreve nele).
- Pedido de abertura alcançado. Não declarar port completo, missão jogável,
  tradução pronta ou salvamentos validados. M6 continua em desenvolvimento;
  deixar o jogo aberto para Davy testar.

## 09/10/2026 — varredura e limpeza de armazenamento

- Davy pediu limpeza antes de avançar no port. Medidas iniciais: projeto
  454.478.418 bytes; pasta de trabalho 37.980.574.605 bytes (total 38,44 GB).
  Os maiores responsáveis eram o dump duplicado da prova M2 (17,80 GB) e
  a extração completa M1 (19,32 GB); não o código nem a compilação.
- Antes de remover derivados, verify_m0.py passou: 87 arquivos do dump
  original, todos os hashes iguais ao M0, CUSA03562 APP_VER 01.03.
- Plano detalhado externo em cleanup-20261009/plan.json; remoção em
  PowerShell com validação de caminho absoluto, raiz, reparse points,
  lista fixa de diretórios e extensões de mídia. Uma validação inicial
  de DirectoryInfo interrompeu antes de apagar arquivos; corrigida e
  operação completa registrada em completed.json.
- Removidos 57.060 recursos extraídos de mídia (texturas/modelos/pacotes,
  sons/vídeos), a cópia m2/prova-01/CUSA03562 e native-01/02/03/out.
  Liberação: 37.290.822.447 bytes. Nenhum dado original foi apagado.
- Mantidos 15.901 recursos pequenos da extração, incluindo todos os 2.331
  CSH/LUC; catálogos completos, índices, provas, logs, capturas e saves.
  Base fixa, build e native-04/out/user preservados; executável permanece
  com SHA256 d0aa33bba7c3cf12c87cee3816c7263af120e2efd5733130784b7aa7604b83e4.
- restore_m1.py recupera um recurso ou pacote ausente a partir do dump,
  valida catálogo/PSARC e hash do recurso, sem sobrescrever. Prova real:
  system.psarc:GxCore/texture/null-black.tex recuperado e conferido (165 bytes).
- audit_m2.py novamente PASS: 2.331 recursos idênticos, 17.866 strings
  japonesas, zero falhas. Extração M1 agora parcial; verificação completa
  de M1 requer reextrair recursos ausentes. Não alterar a prova histórica.
- Medição final: projeto 454.489.617 bytes; trabalho 707.036.026 bytes;
  total 1,16 GB (redução de aproximadamente 97%). Inventário adiciona alguns
  MB de metadados e o log pode crescer; números representam o momento medido.
- Jogo continuou aberto durante a limpeza e depois foi fechado pelo usuário:
  log window closed by user, saída 0; 3.134 leituras / 169.224.749 bytes.
  Não avançado em novos recursos de gameplay nesta etapa.

## 09/10/2026 — pausa solicitada por Davy; retomada pelo menu

- Davy autorizou avançar até a primeira missão e validar salvamentos.
  O port foi reaberto; somente o dump original e native-04 foram usados.
- Corrigido capture_native.ps1 para coordenadas físicas com consciência de
  DPI por thread. A escala do Windows cortava parte da captura anterior;
  play-04.png/play-05.png externos agora mostram a janela inteira.
- Criado input_native.ps1: valida PID, caminho do executável e janela;
  envia pressionar/soltar somente ao Kyoei via PostMessage, sem teclas globais.
  Testes reais de Enter, Z e Space chegaram ao scePadReadState como
  0x8, 0x8000 e 0x4000, seguidos de liberação 0. DualSense também responde.
- A abertura animada continua repetindo (cidade/monstro), porém não foi
  comprovado menu de jogo novo nem controle do personagem. Não declarar
  primeira missão, salvamentos ou jogabilidade validados.
- Próxima investigação: localizar o que segura a transição da abertura
  para o menu (threads/serviços ou desenho da interface). Comando LLDB de
  inspeção foi interrompido pelo pedido de encerrar; nenhuma pilha foi
  obtida. Conferência após interrupção não encontrou LLDB ativo.
- Davy pediu encerrar por hoje e retomar amanhã. Jogo fechado, ferramentas
  e diário salvos. Não alterar o runtime nem recompilar nesta pausa.
  Retomar de build/native-latest.console.log e das capturas externas
  G:/kyoei-toshi-work/native-04/play-01.png a play-05.png; manter extração
  M1 parcial e não recriar grandes cópias de dados.

## 09/10/2026 — primeira fase e personagem controlável; pausa de Davy

- Pedido: superar a abertura que parecia uma sequência de wallpapers e
  chegar ao gameplay. Usado somente o dump original CUSA03562 01.03.
- Corrigido o diálogo de mensagem NONE (tipo 2, sem botões): permanece
  aberto até o jogo fechar, sem fabricar uma escolha. Adicionada a função
  sceMsgDialogGetStatus, compartilhando o estado de UpdateStatus.
  Adaptações de services/dialog_ui são geradas em build pelo CMake;
  snapshot e projeto do Godzilla permanecem intactos.
- Preparação externa nova native-05 inclui o módulo real adiado
  module/MasterORBIS/stage201/StageModule.prx. Hash M0 conferido:
  134260577eb0b9ac10f484d25fa33f05772c3a59917806fbee24afc84e4ddb8a.
  Inicializador real retornou 0, handle 0x4001. native-04 preservado.
  docs/native-prepare.json agora aponta para native-05/out. O preparador
  atual inclui somente a primeira fase, não todas as fases do jogo.
- Marcadores RazorCpu PushMarkerStatic/PopMarker atendidos sem coletor,
  retorno 0 como na referência; nenhuma lógica de gameplay substituída.
- Implementado src/kyoei_ime.c: teclado de nomes nativo do Windows,
  UTF16, limites, confirmação/cancelamento/aborto, estados e resultado.
  Filtro real do jogo chamado na thread guest por GetResult. Teste real:
  sobrenome e nome com limite de quatro caracteres, filtro e opções 0;
  ambos confirmados, resultado 0, e a introdução continuou. Parâmetros
  estendidos, outros tipos e opções ainda não suportados são rejeitados.
- Prova real em native-05/logs/20261009_164159.log: carregamento chegou a
  100%, scripts registraram stage start e StageTitle_end 201. Capturas
  gameplay-15/17/18 mostram carga e primeira rua; gameplay-21 mostra
  personagem, HP 100/100 e minimapa. Controle físico gerou deslocamento
  no analógico e ação de esquiva. Primeira chegada ao gameplay comprovada;
  missão inteira e salvamentos/recarregamento ainda não validados.
- Mundo 3D fica desfocado durante gameplay; a ajuda de controle e o HUD
  aparecem. Não determinada a causa do desfoque. Menu inicial também
  apresentou opções invisíveis sobre o fundo, embora fosse navegável.
- Falha intermitente fatal da GPU: Out of order flip IRQ (saída 23),
  reproduzida na abertura e depois do início do gameplay. Experimento
  de um quadro pendente por vez, esperando count e liberação da superfície,
  passou no teste isolado mas NÃO resolveu o jogo: falhou novamente em
  20261009_164802.log. Experimento e teste removidos dos fontes finais.
- Diagnóstico local de VideoOut compila a unidade da cópia fixa, mantendo
  o assert. Depende apenas de headers do cache externo configurável
  KYOEI_GPU_HEADERS (padrão G:/godzilla-port-codex/build/_deps), lidos sem
  alterar o Godzilla. Demais unidades GPU continuam na biblioteca fixa.
  ABI STL dessa unidade configurada como na biblioteca (iterator level 0).
- Última prova 20261009_165400.log: buffer=1, label=0, previous=0,
  count=2115, pending=0, gc=0, arg=0 antes do assert. Próxima investigação:
  comparar registro dos callbacks de flip com a ordem dos pacotes NOP
  PatchedFlip e das escritas de etiquetas; verificar reuso/submissão dos
  command buffers. Isso é hipótese, não causa confirmada. Não remover o
  assert nem inventar valor 1 para a etiqueta.
- Build final compilou; teste TLS Windows PASS e prepare_native.py passou
  na compilação Python. Executável final com diagnóstico:
  SHA256 74dd4d4c2225cb27256ffa7317db078fda1543aa4c3e6e4fdcaafe505ad751e1.
  Capturas e provas externas preservadas em native-05/session-20261009;
  docs/native-gameplay.json registra os hashes, sem arquivos do jogo.
- Davy pediu salvar e interromper. Nenhum processo Kyoei, LLDB, Ninja
  ou Clang ativo na conferência final. Não executar novos testes ou
  recompilar após essa pausa; retomar dos logs/capturas acima quando
  solicitado. M6 continua em desenvolvimento: gameplay inicial funciona,
  mas o port ainda não é estável nem completo.

## 12/10/2026 — retomada e histórico de callbacks de apresentação

- Davy pediu continuar. Retomada pela falha intermitente Out of order flip
  IRQ, ainda sem causa confirmada. Snapshot e projeto Godzilla preservados.
- Adicionado src/kyoei_flip_trace.h, integrado somente à unidade VideoOut
  gerada em build. Guarda os últimos 64 registros/execuções de callbacks,
  com sequência, identificação do pedido, thread, tempo, buffer e etiqueta.
  Histórico impresso se a etiqueta falhar; assert e agendamento preservados.
  Registro ocorre antes da inserção na fila IRQ: em chamadas concorrentes,
  a sequência observada não prova sozinha a ordem de inserção da fila.
- Inspeção confirmou: GNM registra callback e modifica o command buffer
  para escrever etiqueta 1 antes do NOP PatchedFlip; Liverpool sinaliza
  GfxFlip nesse NOP e consome um callback da fila global. Próximo diagnóstico
  deve distinguir reordenação de pedidos, reuso de command buffers e
  alteração da etiqueta pela apresentação. Nenhuma dessas causas comprovada.
- Compilação passou, test_kyoei_tls.exe PASS, git diff --check sem erros.
  Executável SHA256:
  20f36412562d717abc6044c4ab4e9f1e7009f0cec7ade35fd35d3a61b8ed7364.
- Port aberto com scripts/run_native.ps1, PID 21776, registro externo
  native-05/logs/20261012_150241.log e console build/native-latest.console.log.
  Captura local ignorada build/resume-20261012-01.png mostra personagem na
  rua, HP 100/100 e menu de pausa com opções visíveis. Controle físico
  entregou botões e movimentos do analógico ao jogo durante a observação.
- A execução terminou com saída 30: após movimento pelo controle e eventos
  da fase, o jogo registrou Stall during rendering at flush, syncLabel=61096,
  expectedLabel=61104. Interrupção guest `cd 41` em eboot+0x4331ba, thread
  11140, reportada pelo Windows como access violation. Não houve mensagem
  Out of order flip IRQ nem descarga do novo histórico nesta execução.
  Não tratar essa interrupção deliberada após o stall como simples leitura
  de ponteiro inválido; causa da espera gráfica ainda desconhecida.
- Console registra 8.312 leituras / 1.236.744.684 bytes, zero escritas de
  arquivos. Desfoque, missão completa e salvar/recarregar continuam pendentes.
  Alteração desta sessão é diagnóstico, não correção comprovada.
- Terminal isolado falhou ao iniciar (setup refresh); comandos precisaram
  de execução aprovada fora do isolamento. Git usou safe.directory apenas
  por comando; nenhuma configuração global alterada. Política de scripts
  contornada somente no processo de teste com ExecutionPolicy Bypass.
- Davy pediu salvar para iniciar outro comando e resolver as permissões.
  Não abrir outra execução nesta pausa. Retomar do log acima: investigar
  stall de flush/syncLabel e chamada GNM imediatamente anterior ao `cd 41`,
  além da falha IRQ anterior. Última busca de fontes foi interrompida;
  nenhuma nova correção aplicada depois da parada do jogo.

## 12/10/2026 — corrida de liberação de superfície e novo teste

- Davy relatou que o fechamento no gameplay anterior ocorreu ao andar
  perto da luta de Ultraman e de explosões na cidade; mundo ainda desfocado.
  Comparou também o áudio caótico com vídeo do PS4 e confirmou comportamento
  semelhante na cena. Não alterar áudio só por sobreposição de vozes.
- Desmontagem local do dump confirmou loop em eboot+0x4330b0: após
  10.000.000 leituras sem atingir o valor esperado, imprime stall, chama
  sceGnmDebugHardwareStatus e executa int 0x41. Limite de iterações,
  não timeout em segundos. Compilação de efeitos pode atrasar a thread GPU,
  mas essa causa ainda não foi comprovada. Não pular a interrupção.
- Tentativa LLDB: primeiro faltou python3.dll; PATH do processo com Python
  3.14 existente permitiu conectar. Porém lldb-server falhou com C0000005
  durante o teste, perdeu a conexão; jogo saiu com -1073740972 (C0000374).
  Nenhuma pilha válida capturada. Prova build/render-stall-threads.log e
  log externo 20261012_151249.log. Ferramenta LLDB experimental removida.
- Adicionados kyoei_crash.c/h: MiniDumpWriteDump na chamada de diagnóstico
  de stall e no encerramento por fault. Encaminha diagnóstico GNM ao
  original; não substitui sua resposta. Integração no loader gerado e
  resolver só no executável principal (KYOEI_CRASH_CAPTURE), teste TLS
  separado. Dumps na pasta do executável, atualmente build, nunca distribuir;
  .dmp/.mdmp ignorados globalmente por poderem conter memória do jogo.
  Compilação passou; captura de dump em falha real ainda não comprovada.
- Execução 20261012_152005.log fechou com saída 23, na abertura/transição:
  Out of order flip IRQ. Histórico de 64 eventos mostrou registros e IRQs
  na mesma ordem, todos pela mesma thread registradora; último pedido 4531,
  buffer=0, etiqueta 1 no registro e 0 no IRQ, previous=1, count=4530,
  pending=0. Não houve fault/stall: caminho do minidump não foi chamado.
- Identificada janela de corrida no driver de referência: Flip publica
  status e dispara eventos antes de zerar etiqueta da superfície anterior.
  Consumidor acordado pode reusar essa superfície, escrever 1 para seu
  próximo quadro e ter essa escrita apagada pelo reset atrasado para 0.
- Adaptação local do driver.cpp gerada em build pelo CMake: liberação da
  superfície anterior e atualização de prev_index passam a ocorrer dentro
  do port_mutex, antes da publicação do status e dos eventos. Nenhuma escrita
  artificial de etiqueta 1; assert de ordem continua. Snapshot e Godzilla
  intactos; nova unidade usa os mesmos includes/ABI do VideoOut local.
- tools/test_videoout_order.py compila o método Flip real de cada fonte
  contra consumidor imediato do evento: referência perde a nova etiqueta
  (falha esperada), adaptação preserva (PASS). Não usa dados do jogo.
  Compilação final passou, teste TLS PASS, diff --check sem erros.
- Execução com apenas a ordem corrigida: PID 23040, 20261012_152415.log,
  console build/native-release-order.console.log. Falhou novamente na
  transição, pedido 4101, buffer 0 etiqueta 0, previous=1, count=4100,
  saída 23. A correção da notificação sozinha NÃO resolve o jogo.
  SHA256 desse executável intermediário:
  71f82a3055e4033398110b58c7091b545e430d6199b75cb81e9b9e245029cbf1.
- Adaptação adicional do Liverpool gerada em build: WRITE_DATA de 4 bytes
  com valor 1 para endereço de etiqueta VideoOut espera o valor real 0 antes
  de escrever. Usa YIELD_GFX da própria coroutine para permitir a liberação
  pela apresentação. Outros WRITE_DATA não alterados; nenhum valor forjado
  nem assert removido. Cabeçalho xxHash lido do cache externo existente.
- Teste ampliado compila o trecho real antes do WRITE_DATA contra coroutine
  e liberação tardia da superfície: referência perde trava antecipada,
  adaptação espera a liberação e preserva a nova trava. Ambos os cenários
  de teste (evento e escrita antecipada) PASS; TLS PASS novamente.
- Executável com as duas adaptações:
  39d27a785202a59b9cffa0f564619a749b6fe7810656b0fb9150b4865237024f.
  Execução PID 29280, log 20261012_152834.log, console
  build/native-surface-wait.console.log. Captura build/surface-wait-01.png
  confirma abertura renderizada e botões enviados chegam ao jogo. Ainda
  falta comprovar estabilidade durante gameplay e reproduzir o stall de
  flush; não declarar o port ou a primeira missão completos.
- Captura posterior build/surface-wait-02.png mostra o menu inicial inteiro
  com New Game, Load Game, Option e ???? legíveis; progresso visual observado
  com a espera de superfície. Processo segue aberto para teste, sem LLDB.
- Confirmação de New Game avançou ao carregamento e ao stage start da fase
  201. Captura build/surface-wait-03.png mostra a primeira rua na introdução,
  com prédios/placas definidos; ainda não é prova do desfoque resolvido no
  gameplay controlável. Última conferência: PID 29280 ativo, sem mensagem
  de falha IRQ ou stall nesta execução; deixar aberto para Davy testar.
- Continuam pendentes: validar essa corrida em jogo, diagnosticar stall,
  corrigir desfoque/menu inicial invisível e validar missão/salvar/recarregar.

## 12/10/2026 — retorno de Davy: câmeras e nova falha no gameplay

- Davy confirmou gameplay nesta execução. L1 alterna três modos de câmera:
  dois ficam nítidos e o modo normal permanece desfocado. Esse resultado
  restringe a investigação ao comportamento específico da câmera/efeitos;
  não comprova ainda erro de profundidade de campo ou causa no shader.
- A execução 20261012_152834.log terminou novamente com Out of order flip
  IRQ, saída 23. Último pedido 20623, buffer 0, etiqueta 1 ao registrar e 0
  no callback, previous=1, count=20622, pending=0. Uma única thread registradora
  nos últimos 64 eventos, callbacks na mesma ordem. 8.723 leituras,
  1.344.730.149 bytes, zero escritas. Não houve minidump: caminho atual de
  captura cobre fault/stall, enquanto esta saída é assert GPU.
- As duas adaptações de superfície passaram nos testes isolados, porém
  NÃO eliminaram a falha real; não declarar correção concluída. Investigar
  também as escritas reais de etiquetas e a condição de entrada da espera
  no Liverpool, além da sequência de liberação pelo driver. Histórico atual
  não registra essas operações e não permite atribuir a causa final.
- Preservados console build/native-surface-wait.console.log e log externo.
  Nenhum novo executável aberto nesta conferência; processo anterior saiu.

## 12/10/2026 — marcas de flip privadas e espera cooperativa da GPU

- Davy pediu corrigir os fechamentos e o desfoque. Histórico ampliado para
  256 eventos: escritas de etiquetas, correspondência do port, liberações,
  submissões e NOPs PatchedFlip. KYOEI_FLIP_STRESS=1 acrescenta 2 ms após
  escritas de etiqueta somente para reprodução; padrão desativado.
- Teste direto 20261012_154718.log: pedido 2344 falhou sem WRITE_DATA
  correspondente. Port e endereço de etiqueta conferidos pelo rastreamento.
  Cópia compartilhada original (GZ_COPY_GPU_BUFFERS=1) também falhou em
  20261012_154856.log, pedido 9521, durante a transição para a fase.
- Liverpool agora conserva cópias próprias de DCB/CCB como parâmetros de
  uma coroutine por envio, até processamento completo. Não usa a arena
  compartilhada que SubmitDone reinicia. Teste isolado comprova conservação
  após reutilização da origem, suspensão de dois envios e liberação dos
  frames de coroutine. Essa mudança sozinha ainda falhou em
  20261012_155436.log, pedido 8696.
- Esse último histórico identificou NOP PatchedFlip com header c09e1000,
  fora do NOP específico produzido pelo patch, sem trava precedente. Causa
  investigada: PatchFlipRequest modificava a memória do jogo e deixava sua
  marca interna ali; reutilização do NOP podia conservar o payload antigo.
- GNM local gerado em build passa a aplicar PatchFlipRequest numa cópia
  privada do último DCB e encaminhar essa cópia ao Liverpool, que a conserva.
  Arrays e comandos originais intactos. Preservados retornos de erro e
  callbacks reais, sem fabricar etiquetas ou retirar o assert de flip.
  Guards sem janela mantidos; reserva da antiga arena removida na unidade
  local. Godzilla e snapshot intactos; unidades adaptadas: VideoOut, driver,
  Liverpool e GNM. Não depende de GZ_COPY_GPU_BUFFERS para copiar envios.
- tools/test_flip_patch.py usa o wrapper real com patch mutante controlado:
  referência deixa marca na origem (falha esperada); adaptação mantém origem,
  encaminha comandos alterados e propaga erro sem submeter (PASS).
  tools/test_gpu_snapshot.py, test_videoout_order.py e test_kyoei_tls PASS.
- Execução 20261012_160128.log com patch privado e stress atingiu gameplay
  e menus. Não falhou por flip nessa tentativa, mas saiu 30 por stall ao
  aproximar-se de Ultraman; Davy relatou fechamento quando foi pisado.
  syncLabel=45535, expectedLabel=45543, trap em eboot+0x4331ba.
- Captura interna comprovada: render-stall e fault .dmp em build, cerca de
  0,9 MB cada. Análise LLDB offline funcionou sem conectar a processo vivo.
  No dump de fault, etiqueta real em 0x1038255300 já tinha 0xb1e7 (45543),
  exatamente o esperado. Thread Liverpool ociosa no dump de stall. O trabalho
  terminou depois do limite de tentativas, antes do encerramento; isso
  fundamenta adaptar a espera, sem falsificar conclusão gráfica.
- src/kyoei_gpu_wait.c/h: antes da rotina original de espera, lê a etiqueta
  real e cede CPU com Sleep(1), por até 3000 ms. Ao terminar/expirar entra
  na rotina original: comparação, 10 milhões de tentativas e int 0x41
  preservados. Caso de etiqueta nula segue a seleção frame-local original.
  Relay de 29 bytes próximo à imagem, argumentos SysV preservados, código
  RX após montagem e proteção da página original restaurada.
- Adaptação só aceita a assinatura dos 316 bytes de eboot+0x4330b0:
  FNV64 fa40be72aa1369e9, SHA256
  f70d547733e932e9a82473aeed96abb2658b1c393b02ce235f5e0ab699de872d.
  Não muda os arquivos preparados nem o dump; instala desvio na memória.
  test_kyoei_gpu_wait PASS: fence atrasada, timeout sem escrever etiquetas,
  relay preservando três argumentos mesmo com callback que os altera.
- Executável final desta etapa SHA256
  1382c5f4995a13303866919d87761d41cdad2b43e39dd5025dcc5868088ddee0.
  Diagnóstico --list-stubs sem janela passou após GNM local (451 runtime,
  418 módulos do jogo, 297 stubs); esses números não validam gameplay.
- Execução com espera e sem stress abriu, assinatura aceita; fechada
  normalmente pelo agente (CloseMainWindow) para comparar opção gráfica,
  saída 0. Nenhuma falha observada nesse teste limitado de abertura.
- Davy confirmou em vídeo do PS4 que câmera normal fica nítida; aqui dois
  modos alternativos são nítidos e o normal ainda desfocado. Q adicionado
  ao input_native.ps1 para L1. Capturas camera-private-01/02 em build; a
  segunda coincidiu com menu aberto por Davy, não é comparação controlada.
- Pista externa, não comprovação: issue shadPS4 #5057 relata névoa incorreta
  em CUSA03562 01.00 sem Readback Linear Graphics. Fonte:
  https://github.com/shadps4-emu/shadps4/issues/5057 . Nosso dump é 01.03.
  Opção existente GZ_READBACK_LINEAR=1 testada, sem desativar efeito do jogo.
- Nova execução de comparação: PID 28832, 20261012_161617.log,
  build/native-linear-wait.console.log, sem stress, com leitura linear e
  espera cooperativa. Ainda falta conferir câmera normal e passar pelos
  efeitos/Ultraman nessa execução. Não declarar estabilidade ou missão
  completa; salvamento/recarregamento seguem pendentes.

### 12/10/2026 — câmera nítida, PNG e primeira medição de desempenho

- Conclusão da execução 20261012_161617.log: captura
  build/linear-wait-06.png mostrou gameplay com câmera normal nítida,
  antes de alternar L1. Um modo alternativo também foi capturado nítido;
  a captura do terceiro estava minimizada e não serve de prova.
  GZ_READBACK_LINEAR=1 agora é padrão no lançador e na integração nativa;
  valor explícito 0 continua disponível para diagnóstico. Não removemos
  o efeito de foco do jogo. Capturador agora rejeita janela minimizada.
- Nessa execução a espera real de GPU terminou após 78 ms,
  expected=actual=192888, reached=1, sem forjar etiquetas. O jogo avançou
  até pedir scePngEncQueryMemorySize e encerrou com stub ausente (saída 20).
  Houve escrita de imagens em download0 e referência a
  SV_ID_201_AFTER_POLICE, mas isso não comprova save/load completo.
- Implementado src/kyoei_png.c/h: QueryMemorySize, Create, Encode e Delete,
  ABI SysV, codificação PNG real com miniz já existente, RGB/RGBA a partir
  de RGBA/BGRA, pitch e filtros PNG, limites e erros sem sobrescrever a
  saída quando o buffer é insuficiente. Sem nova biblioteca baixada.
- Testes: 64 PNGs decodificados independentemente com Python/zlib, CRC,
  pixels e filtros conferidos; formato, alpha, padding, buffer pequeno,
  entrada truncada e ciclo de handles PASS. Testes flip privado, snapshots,
  ordenação VideoOut, espera GPU e TLS PASS na compilação otimizada.
- Encontrado build local em Debug. Reconfigurado para RelWithDebInfo
  (otimização com símbolos), alinhado ao CRT release da biblioteca GPU.
  Build final passou; SHA256 do executável:
  3869508734a87cc6e06175978ca772ce7695226ae7967f0895f22b64e45f8aeb.
  --list-stubs passou: 455 runtime, 418 módulos, 293 stubs restantes.
- KYOEI_FRAME_STATS=1 mede intervalos entre apresentações concluídas do
  jogo, sem contar redesenhos ociosos. Relatório a cada 120 intervalos:
  FPS médio, média, p50/p95 e máximo. Não mede CPU/GPU separadamente.
  Nova execução em build/native-png-release.console.log, PID inicial
  22556, sem stress. Primeiras janelas após abertura perto de 60 FPS,
  mas tela/cena não certificada; não usar como ganho de gameplay.
  Primeira captura release-png-open.png preta, não comprova renderização.
- Pedido de Davy: começar inglês e melhorar desempenho. Plano salvo em
  docs/PLANO.md: estabilidade/save-load e medição em paralelo a um piloto
  de uma frase, depois 20–30 textos, cobertura M2/fontes M3 e revisão por
  cena; só depois tradução ampla/patch. Nenhuma tradução visível validada
  ainda. PNG precisa ser exercitado na partida; missão completa e ganho
  de FPS em percurso controlado continuam pendentes.
- Registro da nova execução: 20261012_201311.log. Captura posterior
  build/release-png-stage.png mostra tela de carregamento da primeira fase,
  58%, cenário da estação e narração visíveis. Durante esse carregamento
  relatório caiu a 13,14 FPS (p95 234,82 ms, máximo 665,95 ms); isso reforça
  separar carga/transição de gameplay. Os ~60 FPS anteriores não são
  evidência de desempenho na cidade. Partida deixada aberta para Davy.

### 12/10/2026 — erro real de salvamento, parada de módulo e profiling

- Davy relatou visual correto, desempenho abaixo de 30 FPS e fechamento
  após problema ao salvar. Log 20261012_201311 confirma vários relatórios
  de 9,77–21,42 FPS durante a fase, além de trechos perto de 30. Não houve
  melhora de fluidez comprovada pela compilação otimizada.
- PNG realmente exercitado: 228x128 RGB, 51.699 bytes. Depois o jogo pediu
  SaveDataDialog LIST (mode=1), não suportado no runtime fixo. Open retornou
  0x80b8000f, erro mostrado ao usuário; nenhuma escrita de save nessa partida.
  Mais tarde limpou os recursos da fase e fechou por stub de
  libkernel::sceKernelStopUnloadModule, eboot+0x6c31ac, handle 0x4001.
  O fechamento não foi uma prova de corrupção de save nem falha do PNG.
- Adaptação local de services/dialog_ui: lista de slots com navegação pelo
  controle/setas, OK/cancelar e novo save quando permitido. Resultado preserva
  ponteiros do chamador, usa diretório vazio para novo slot e buttonId INVALID
  conforme referência. Confirmações SYSTEM_MSG CONFIRM/OVERWRITE têm escolha
  real Sim/Não; demais tipos ainda são recusados explicitamente. A lista
  mostra nomes de diretórios, sem miniaturas/metadados de título; parâmetros
  do save selecionado ainda não são copiados para result.param. Carregamento
  e salvamento completos dependem de validação real, não declarar M6 concluído.
- Teste tools/test_save_list.py usa funções reais do services adaptado com
  UI controlada: novo/existente, cancelar, limites, ponteiros preservados,
  retorno Não e mensagem não suportada PASS.
- kyoei_modules suporta StopUnloadModule chamando DT_FINI real do módulo.
  Finalizadores verificados contra hashes M0 por tools/update_module_stops.py:
  libSceSecure offset 0x157a0; StageModule offset 0x140. Metadados F registrados
  só na saída preparada, preparador atualizado para futuras saídas. Após
  parada bem-sucedida marca módulo parado, impede dlsym e admite novo start;
  imagem fixa continua mapeada, não fingir que memória foi desmapeada.
  test_kyoei_modules PASS: argumentos/resultado, falha preservando estado,
  handle inválido e parada duplicada. Finalizador real na partida pendente.
- Profiling opcional: custo de PrepareFrame, Present e ProcessDownloadImages
  em médias/p95/máximo de 120 chamadas. TextureCache copiado/adaptado localmente;
  nenhuma mudança no algoritmo de leitura. Caminho existente espera GPU por
  imagem; hipótese de gargalo, ainda sem medição de gameplay que a confirme.
  Preparação/apresentação iniciais abaixo de 1 ms em média, carga posterior
  apresentou 17–26 FPS, ainda não atribuir causa. Readback continua habilitado.
- Execução nova: build/native-save-profile.console.log, PID inicial 32544,
  KYOEI_FRAME_STATS=1, stress=0. Executável em uso SHA256
  ae57e0f4d0e71f70a872faf936ed2e4ad537fbcacb5ce68f0a36e894ea0ae4ff.
  Ajuste posterior de conversão de handle evita overflow de inteiro no fonte;
  test_kyoei_modules já o compila, executável da partida ainda é o anterior.
  Não recompilar/substituir o executável em uso para não interromper Davy.

### 12/10/2026 — Mount2 após escolha de slot e medição de TLS

- Davy confirmou novo fechamento ao clicar para salvar e nenhuma melhora
  de desempenho percebida. Execução 20261012_202521.log mostrou cancelamento
  da lista funcionando e, na tentativa seguinte, seleção de novo slot
  funcionando: selected=0, directory vazio, result=0. Gerou PNG 228x128
  de 51.699 bytes e entrou no progresso de gravação. Fechou por stub
  sceSaveDataMount2 em eboot+0x6d2826, não por falha da lista. Writes=0.
- src/kyoei_savedata.c implementa Mount2 (64 bytes, mode offset 24) adaptando
  ao Mount real já existente (80 bytes, mode offset 40). Preserva usuário,
  diretório, blocos, modo, ponteiro de resultado e erros do backend. Usando
  título configurado a partir do SFO, não o título do Godzilla.
- Conferidas todas as 13 importações libSceSaveData do eboot. As duas outras
  ausentes eram GetProgress/ClearProgress, relativas ao backup assíncrono.
  Cópia adaptada rt_savedata em build adiciona consultas com validação de
  inicialização e ponteiro; zero representa ausência de trabalho de backup.
  Não reporta sucesso de gravação nem implementa backups inexistentes.
- Build passou. test_kyoei_savedata PASS (ABI/encaminhamento e erros),
  test_kyoei_tls PASS. tools/test_savedata_storage.py usa o backend real
  e pasta isolada build/test-save-storage/user, sem tocar nos saves de Davy:
  payload e metadados escritos, desmontados, terminados, reabertos e lidos
  idênticos; traversal inválido recusado, progresso e inicialização conferidos.
  Mapeamento guest no teste é controlado; isso não comprova save/load do jogo.
- Profiling da partida anterior: grupos de leitura linear com média 23,39 ms
  no primeiro relatório e 12,35 ms no seguinte, p95 18 ms no segundo.
  Prepare/Present perto de 0,15/0,5 ms. Readback tem custo relevante, mas
  ainda não explica sozinho a lentidão. Não removido, preservando visual.
- Adicionado contador de exceções FS:[0] tratadas por TLS e relatório por
  120 apresentações, para investigar custo CPU específico do Kyoei. Ainda
  não otimizado esse caminho nem feita afirmação de ganho de FPS.
- Nova partida aberta: build/native-mount2.console.log, stress=0, métricas=1.
  Retestar save e load no jogo; estabilidade completa e performance pendentes.

### 12/10/2026 — gravação confirmada; consulta de módulo ao dar load

- Davy relatou fechamento ao dar load e leve melhora de desempenho.
  Execução 20261012_203450.log confirma gravação real: SAVEDATA.DAT
  254.576 bytes, SYSTEM.DAT 26.344 bytes, dois param.bin de 1.328 bytes,
  ícones PNG. Ambos os jobs de gravação retornaram 0. Runtime registrou
  138 escritas. Arquivos ficaram no user separado, não no dump/repositório.
- Load selecionou SAVEDATA-000, montou existente read-only, terminou
  o diálogo e começou desmontagem da fase. Fechou depois em
  libc.prx+0x71178 por sceKernelGetModuleInfoFromAddr, addr=0x801c2c020,
  flags=2. Consulta foi feita dentro do finalizador real de StageModule.
  Não há evidência de save corrompido; gameplay restaurado ainda não provado.
- Implementada consulta por endereço com ABI de 424 bytes, segmentos
  offset 352, limites exatos, flags 0..2 e erros reais para parâmetros
  inválidos/endereço desconhecido. Responde só módulos diferidos cujos
  metadados foram certificados; outros endereços retornam ESRCH.
  Não pega lock de ciclo de vida, pois finalizador já o possui.
- tools/native_module_metadata.py deriva segmentos PT_LOAD/SCE_RELRO,
  finalizador e EH frame de ELF próprio verificado. Aceita EH versão 1 com
  ponteiro pcrel sdata4 observado; rejeita outros formatos. Segmentos usam
  bytes efetivamente preparados, sem anunciar páginas extras do emulador.
  Preparador e atualizador de metadados usam o mesmo gerador. Nenhum código
  ou texto de jogo foi versionado; tabela fica na saída privada preparada.
- test_kyoei_modules PASS: fronteiras, flags, nulo, chamada de info dentro
  de finalizador sem deadlock, estado/erros de stop. Build e --list-stubs
  passaram: 460 runtime, 418 módulos do jogo, 288 stubs restantes.
- Executável SHA256
  037a4cbfd892604d1fbb618d8c6429d1004570a1c8d9d9738a711844ce994878.
  Nova partida aberta em build/native-load-info.console.log, stress=0,
  métricas=1; saves anteriores preservados. Load completo continua pendente.
- Performance: medição anterior mostrou aproximadamente 21.000 exceções
  TLS tratadas por apresentação em alguns trechos; linear-downloads chegou
  a média 9,90 ms/p95 15,93 ms. TLS é forte candidato de custo CPU a otimizar;
  ainda não foi alterado o mecanismo nem comprovado ganho controlado.
  Leve melhora percebida por Davy é relato de teste, não benchmark.

### 12/10/2026 — load confirmado; Continue e TLS direto

- Davy confirmou que load restaurou gameplay, jogou, morreu e o Continue
  fechou. Save/load inicial agora tem confirmação humana; retomada após
  morte e missão completa permanecem pendentes. Partida anterior
  20261012_204140.log fechou por __elf_phdr_match_addr dentro de
  libc+0x711e4, durante StopUnloadModule da fase, não por leitura do save.
- Implementada comparação real de endereço contra segmentos executáveis
  de ModuleInfoEx. Fonte primária de comportamento FreeBSD:
  https://lists.freebsd.org/pipermail/svn-src-head/2010-August/019849.html
  Disassembly do libc próprio em build/libc-inspect.elf confirmou que o
  PS4 passa o mesmo ModuleInfoEx devolvido por GetModuleInfoFromAddr,
  e não o dl_phdr_info do FreeBSD. Não retornar sucesso fixo para ignorar
  destructors. Testes de segmento executável/dados, limites e nulo PASS.
- Performance anterior: 19–33 mil exceções TLS por apresentação em
  vários trechos; linear-downloads média 15,12 ms/p95 24,28 ms em relatório.
  Implementado caminho direto para as 580 leituras RAX FS:[0] já catalogadas.
  Slot próprio TlsAlloc abaixo de 64, layout x64 TEB inline conferido contra
  TlsGetValue com sentinela antes de patch. Instrução GS mantém registrador,
  tamanho e flags; nenhuma chamada/trampoline/red-zone. Proteções restauradas
  e instruction cache limpo. Alteração em memória antes das threads guest;
  dump e arquivo preparado intactos. Entradas main/pthread preinicializam
  TCB. Outros caminhos host->guest novos precisam do mesmo cuidado.
- KYOEI_TLS_FAST=0 permite baseline. Sem metadados ou slot/layout compatível
  mantém exceções. Gerador/update_tls_sites.py verifica eboot M0 e exporta
  só offsets privados para tls-sites.tsv; runtime verifica assinatura exata
  de cada instrução, recusa incompatibilidade. Cópia de rt_thread gerada
  no build; core fixo e Godzilla preservados.
- Teste TLS com duas threads e 10.000 leituras por thread: legado
  32,37/32,74 ms e 20.002 exceções; direto 0,02/0,10 ms e zero exceções.
  Template, zeragem, isolamento, alinhamento e cleanup PASS nos dois modos.
  Ganho é do teste de TLS; ainda não é comparação controlada de FPS na cidade.
- Build, test_kyoei_modules, test_kyoei_tls nos dois modos e --list-stubs
  PASS. 461 runtime, 418 módulos, 287 stubs. As 580 assinaturas reais
  foram aceitas; slot 8. Executável SHA256
  82e1714070e7c6e420a365320cac389de907f73d0f9cc418aeee7faea6b548c8.
- Nova execução build/native-continue-fast.console.log, PID inicial 32680,
  fast=1, métricas=1, stress=0. Abertura com handled_total=0. Relatórios
  iniciais de 60 FPS ainda são abertura, não prova de desempenho na cidade.
  Partida deixada aberta para confirmar Continue e fluidez após load.

### 12/10/2026 — limpeza de destructors de threads ao continuar

- Davy relatou rodar melhor, mas Continue após morte ainda fechou.
  20261012_205113.log confirma que o finalizador avançou além de
  __elf_phdr_match_addr e fechou na próxima API ausente:
  libkernel::__pthread_cxa_finalize, libc+0x71309, chamada no stop da fase.
  TLS handled_total permaneceu zero ao longo da partida. Relatórios finais
  incluíram 33–41 FPS e quedas a 19–25; melhoria percebida não significa
  performance concluída nem comparação de rota controlada.
- Referência de semântica: limpeza de handlers atfork e destructors TSD ao
  descarregar biblioteca. Runtime nativo não aceita registros atfork/sinais
  e não tem filas desses handlers; tem tabela real de chaves TSD.
  Fonte primária para essa tabela:
  https://raw.githubusercontent.com/freebsd/freebsd-src/main/lib/libthr/thread/thr_spec.c
- src/kyoei_thread_finalize.h, incluído na cópia local rt_thread gerada,
  implementa __pthread_cxa_finalize retirando só destructors de chaves
  usados cujo endereço pertence aos segmentos executáveis da fase.
  Mantém chave alocada e valor, como _thr_tsd_unload de referência;
  mantém destructors de outros módulos. Não executa destructors nesse
  ponto, não termina threads, não retorna sucesso fixo para ignorar limpeza.
- tools/test_thread_finalize.py usa as operações reais de chaves do core
  e o adaptador real: callback da fase removido, callback estrangeiro e de
  região não executável preservados, chave/valor mantidos, repetição idempotente,
  cleanup posterior chama callback restante. PASS. Build, test_kyoei_modules
  e --list-stubs PASS. 462 runtime, 418 módulos, 286 stubs.
- Nova execução build/native-pthread-finalize.console.log, fast=1,
  métricas=1, stress=0. Saves preservados. Continue precisa de nova validação
  no jogo; não declarar estabilidade completa. Godzilla/core fixo intactos.

### 12/10/2026 — título, botões japoneses e reuso de memória no Continue

- Davy pediu nome do Kyoei na janela e círculo confirmar/X voltar no save/load,
  como no jogo. Também relatou Continue passando à tela de carga e fechando;
  informou depois que velocidade estava bem melhor.
- 20261012_210849.log avançou além do finalizador de threads; fechamento
  agora é MapViewOfFile3, Windows 487, ao mapear 0x113d220000+0x10000 na
  recarga. Não é stub ausente nessa tentativa. TLS direto preservado.
- Reproduzido em teste sem jogo: map/unmap fragmentado com a implementação
  fixa falha no mesmo Windows 487, tabela com 512 regiões. Diagnóstico mostrou
  placeholder real de 0x8000 contra requisição de 0x4000. Expressão original
  &regions[placeholder(...)] permite ler o ponteiro da tabela antes de
  placeholder realocar essa tabela. Ponteiro pode ficar obsoleto no crescimento.
- Cópia local rt_vmem: primeiro calcula índice com placeholder, depois lê
  a base atual da tabela. Teste tools/test_vmem.py reproduz falha esperada no
  core original e passa 20.000 operações no adaptado, conferindo conteúdo de
  regiões ainda mapeadas. Teste usa processo/pool próprios, não saves/dump.
  Diagnóstico de VirtualQuery/offset físico adicionado só em falha.
  Contrato Windows consultado: placeholder deve casar exatamente com a view:
  https://learn.microsoft.com/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffile3
- Janela criada pelo adaptador local window.cpp com título
  "City Shrouded in Shadow - Kyoei Toshi | PC Port", incluindo base_title.
  Capturador não renomeia mais a janela. Core de referência continua intacto.
- Diálogos nativos: círculo (0x2000) confirma, X (0x4000) cancela/volta;
  Enter/Space confirmam, Esc volta. Indicação na tela atualizada. X em aviso
  com só um botão retorna cancelamento, não confirmação. IME também usa
  círculo confirmar e X cancelar, sem aceitar ambos como confirmação.
  Mapeamento dos botões do próprio jogo não foi invertido.
- Build PASS. tools/test_dialog_buttons.py valida navigate real em listas
  e avisos de um/dois botões e indicação UTF-8; test_save_list PASS.
  Novo executável SHA256
  dee729ed8fd2d1cf6be007ef63b2ee9ec2faa967c6d961380ee3a68266b93fb2.
- Execução aberta: build/native-title-buttons-vmem.console.log, PID inicial
  33432, fast=1, métricas=1, stress=0. Get-Process confirmou título novo
  na criação real da janela. Continuar após morrer ainda exige validação
  de gameplay; ganho relatado por Davy registrado sem prometer FPS fixo.

### 12/10/2026 — Continue confirmado e observação do começo

- Davy confirmou Continue após morrer funcionando, título corrigido e
  círculo/X corretos no save/load. Relatou velocidade bem melhor e pediu
  acompanhamento de FPS/visual/falhas enquanto jogava sem Abiotic Factor.
  Pediu encerrar a análise ao morrer; não interrompemos/reiniciamos o jogo.
- Análise salva em docs/ANALISE-COMECO.md: duas amostras de ~30 s, 600/720
  intervalos, FPS agregados 22,74/25,41. Incluem passagem por cena/eventos;
  não comparar com abertura/menu de 60 FPS. TLS zero. Leitura linear
  média 11,65 ms em relatório da segunda amostra. CPU ~1,8 núcleos ativos,
  RAM residente ~8,4 GiB. Três amostras pontuais GPU 3D 11,5–36,2%; não
  diagnóstico definitivo de gargalo ou vazamento.
- Quatro capturas de cidade, diálogo, fogo e gigante inspecionadas sem
  defeito gráfico evidente nesses frames. Nenhum STOP/assert/map failure
  nos intervalos; processo aberto após morte. Flicker/movimento e comparação
  exata com PS4 não certificados. Não prometer primeira fase completa.
- tools/analyze_native_performance.py resume intervalos delimitados por
  offset em bytes, aceita logs UTF-8 e UTF-16 do PowerShell. Mantém métricas
  privadas em build; não inclui textos originais nos relatórios versionados.
- Piloto do começo localizado: Text_Stage_201.luc tem 749 candidatos;
  map_goal_text.luc contém objetivos/marcadores observados. IDs e escopo
  documentados sem versionar originais. Nenhuma tradução aplicada ainda.

### 12/10/2026 — preparação integral e proteção de nomes

- Davy autorizou preparação de todos os textos e imagens de interface,
  preservando anúncios/placas ambientais. Relatou flash preto nos menus e
  textura estranha no tutorial do orbe; ambos seguem sem correção comprovada.
- Uma execução foi aberta para verificar readback apesar de Davy dizer que
  não testaria. Erro reconhecido. Orientação atual: não abrir o jogo; continuar
  apenas ferramentas e testes offline. Nenhuma tradução aplicada ao dump.
- Catálogo privado translation/catalog-01 em G:/kyoei-toshi-work:
  2.331 recursos auditados, 230.622 ocorrências UTF-8 e 4.447 de outra codificação;
  classificação preliminar encontra 15.687 candidatos. Não certifica toda a
  cobertura do jogo. Varredura complementar encontrou 120 trechos em HUD/CSD,
  cuja estrutura de reinserção ainda precisa ser estudada.
- Inventário de 51.266 texturas: 1.412 candidatas de interface para revisão,
  1.411 prévias BC3 decodificadas; uma textura legada pendente. Ambiente fica
  excluído por padrão. Galeria privada em catalog-01/images/review.html.
  Imagem original do tutorial do orbe inspecionada limpa; isso não prova que
  a renderização em execução esteja correta.
- Piloto: 30 entradas de objetivos/ações, inglês e pt-BR, 60 traduções com
  reinserção e releitura verificadas, outros textos preservados. Rascunho
  exige revisão humana e teste de layout; sem aplicação ou aprovação final.
- Davy pediu excelência e cuidado com nomes. Glossário inicial protege grafias
  oficiais; nomes humanos ainda pendentes. Validador verifica ocorrências,
  nomes personalizados explicitamente indicados e referências do motor.
  Cinco testes de proteção PASS; piloto estrutural PASS. Ferramenta não
  reconhece automaticamente todos os nomes japoneses; revisão obrigatória.
- Readback em lote preparado na integração local e teste auxiliar passou;
  ganho de FPS não comprovado. Investigação gráfica/performance permanece
  em andamento. Core fixo e projeto Godzilla preservados.

### 12/10/2026 — continuidade offline e checkpoint de tradução

- Davy autorizou avançar sem abrir o jogo até perto do limite de uso e salvar
  o progresso. Percentual de quota do Codex não está exposto ao agente;
  manter checkpoints persistentes, sem prometer leitura desse percentual.
- Fontes fallback NotoSans e NotoSansCJK face 0 têm todos os 152 caracteres
  necessários ao piloto/ASCII/acentos e pontuação. Relatório com hashes em
  docs/m3/fallback-coverage.json. Fonte própria do jogo/layout ainda pendentes.
- docs/m4/coverage-queue.json contabiliza os 72.961 arquivos sem perdas:
  2.331 recursos estruturados; 51.266 texturas; 23 recursos com hits extras;
  quatro vídeos pendentes; 202 recursos de áudio preservados; 19.135 arquivos
  de famílias ainda não certificados quanto a texto visível. Estes últimos
  incluem dados de modelos/animações; não são 19.135 textos para traduzir.
- Varredura complementar agora limita a descompressão e rejeita streams
  incompletos/sobras, como o extrator principal. Testes sintéticos de texto:
  seis PASS; proteção de nomes: cinco PASS; compilação Python PASS.
- Readback em lote permanece opt-in KYOEI_BATCH_READBACK=1 até validação
  comparativa de execução. Nenhuma nova execução do jogo nesta continuidade.

### 12/10/2026 — launcher local por duplo clique

- Davy pediu poder abrir o jogo sem voltar à conversa. Criado o launcher
  City Shrouded in Shadow.exe na raiz, ignorado pelo Git; fonte C# e compilador
  em tools/build_launcher.ps1. Usa scripts/run_native.ps1, preparação native-05
  e os saves atuais. Sem terminal adicional; logs capturados e erro indicado.
- scripts/run_native.ps1 valida arquivos essenciais e oferece -CheckOnly.
  Launcher --check retornou 0 e gravou confirmação em build/launcher-last.log.
  Nenhum processo kyoei aberto. Abertura real não testada por orientação atual.
  Instruções em docs/ABRIR-JOGO.md. Executável não inclui dados do jogo.

### 12/10/2026 — readback e evidência para defeitos gráficos

- Cópia local de TextureCache invalida memória não coerente depois da GPU
  concluir e antes da leitura/publicação, tanto síncrona quanto assíncrona.
  Antes, Commit invalidava antes de a cópia GPU ser gravada. Referências
  técnicas e limites em docs/DIAGNOSTICO-GRAFICO.md; não associar ao orbe sem
  reprodução na GPU real. Memória coerente mantém o caminho sem invalidação.
- tools/test_readback_integration.py compila os métodos reais de referência
  e adaptados contra GPU/ring simulados. Reproduz bytes antigos da referência
  não coerente, corrigidos nos dois caminhos. Lote com três imagens e buffer
  para duas usa duas esperas, versus três, preservando bytes e publicação.
  Teste auxiliar do lote PASS. Padrão continua sem lote; ganho real pendente.
- Ativação do lote usa escopo RAII e restaura o estado anterior. CMake agora
  inclui alvo permanente do teste. Build kyoei + teste PASS, sem executar jogo.
- KYOEI_DISPLAY_TRACE=1 registra flips pretos solicitados pelo jogo, saída
  fechada e atributos de superfícies. Não suprime frames ou altera ordem.
  Analisador de desempenho coleta o diagnóstico e amostras dos lotes.
- Regressões de ordem de VideoOut, gate de labels e patch privado de comandos
  PASS, reproduzindo as falhas da referência nos testes sintéticos.

### 12/10/2026 — revisão por cena, prévias corrigidas e fontes nativas

- Páginas privadas de revisão com importação/exportação de traduções e hashes.
  Atualização scene-review-02: 23 recursos e 7.946 candidatos, excluindo alguns
  rótulos internos de debug/protagonista. Ordem de constantes não certifica
  cronologia. Japonês fica só nas páginas privadas, não no rascunho exportado.
- Galeria portátil com classificação, observações e importação/exportação.
  Primeira folha: 36 imagens inspecionadas, 25 preservadas, dez com texto para
  edição e uma pendente. Instruções de controle examinadas em prévia separada.
  Não houve edição IA nem aplicação de imagens traduzidas.
- Investigação do atlas mostrou limite do primeiro decodificador: ele tratava
  toda textura como BC3. Algumas texturas usam RGBA8 ou máscaras R8. Corrigido
  o despacho pela família/canais medidos; cinco testes sintéticos PASS. Todas
  as 1.412 candidatas reprocessadas em catalog-01/images-v2; 1.411 prévias,
  uma textura legada pendente. Galeria nova: images-v2/review-portable.html.
  Prévia BC3 de fonte era inválida, não evidência de corrupção do jogo.
- Fonte principal: 11.303 chaves, bytes UTF-8 com padding em inteiros CSH.
  Seus 153 caracteres necessários estão presentes; atlas R8 inspecionado e
  acentos confirmados visualmente. Sete tabelas alternativas common/font e
  uma de sistema faltam 52 caracteres do conjunto. M3 ainda não completo:
  aplicação das variantes e correção dos atlas/fontes exigem investigação.
  Docs/m3/FONTES.md registra limites; não converter nomes ou remover acentos.

### 12/10/2026 — launcher visual e idiomas parciais

- Davy informou 17% de quota e pediu idioma/resolução/tela e identidade visual.
  Launcher recompilado com arte original sce_sys/icon0.png, ícone multirresolução
  Windows, cores noturnas/ciano/coral, opções JA/EN/PT-BR e botão Iniciar jogo.
  Renderização de prévia oculta inspecionada; nenhum kyoei iniciado.
- Resoluções de saída 720p, 900p, 1080p, 1440p e 2160p em janela. Tela cheia
  sem bordas usa a resolução do monitor. Renderização interna não alterada;
  não prometer ganho de FPS pela resolução de saída. Preferências em build.
- Rascunhos: 30 objetivos/ações, 15 descrições de opções e 49 entradas da
  narração/criação do personagem: 94 entradas em cada idioma, 188 traduções.
  Variáveis de nome, cores [#...]/[/#] e ícones [i10]/[i11] preservados e
  testados. Rótulos internos e nomes personalizados não traduzidos.
- Gerados e verificados dois common.psarc privados em runtime-preview-02.
  Provas de hash/leitura/reinserção e manifesto local em docs/m4/runtime-preview.json.
  São arquivos derivados do jogo, não distribuíveis; Git contém só rascunhos,
  ferramentas e metadados. Dump original intacto. Versão anterior privada 01
  também permanece como checkpoint, ocupando ~12,7 GB adicionais.
- Runtime local monta apenas /app0/DAT/common.psarc e /hostapp/DAT/common.psarc
  da língua selecionada. Outros arquivos vêm do dump e saves ficam no mesmo
  lugar. Teste usando backend real e arquivos sintéticos: JA/EN/PT-BR PASS,
  abertura/leitura, somente leitura, outros arquivos e rejeição de travessia.
- Build do port PASS. CheckOnly EN/PT-BR com configurações passou sem jogo.
  Prévia ainda exige revisão humana, teste visual de layout/acentos e gameplay;
  não declarar menus totalmente traduzidos, imagens traduzidas ou abertura
  real do novo launcher certificada. instruções em docs/ABRIR-JOGO.md.

### 13/10/2026 — retomada persistente e integração de diálogos

- Davy corrigiu o encerramento precoce: continuar o plano autonomamente,
  sem pedir testes ou abrir jogo. Também indicou GoW3 PC e Bloodborne PC.
- Referências públicas copiadas só para leitura em build, sem submódulos de
  jogos nem execução. Commits/licenças/padrões em docs/FONTES-CODIGO.md.
- Integração ImGui/Vulkan adaptada do GoW3; versão fixa ImGui 1.92.9b/MIT
  adicionada separadamente do core Godzilla. Core/Godzilla permanecem intactos.
- Nome, lista de saves, confirmações e progresso agora ligados às APIs reais
  de overlay. Eventos SDL/pad encaminhados, entrada convidada neutra e redraw
  do último frame em espera. Desligado por padrão; fallback nativo preservado.
- Build kyoei PASS sem execução. test_kyoei_overlay PASS: desenho ImGui real,
  bounds, edição, ownership de 1.025 slots, cancelamento/progresso/fallback.
- test_kyoei_ime_overlay PASS: ABI real, filtros na thread de GetResult,
  publicação, guards, Unicode, cancel/abort e lifecycle. Não escreve saves.
- Corrigidos terminador residual na troca do nome inicial, Square apagar/
  Triangle shift e escala cumulativa ao redimensionar. Regressão name_model
  PASS. Testes reais anteriores de SaveList e DialogButtons PASS.
- Launcher recompilado e prévia oculta inspecionada sem jogo. Nova opção
  inglesa de diálogos e CheckOnly en -InGameDialogs PASS. Gameplay e composição
  Vulkan não certificados offline; instruções em docs/OVERLAY-DIALOGOS.md.
- Observação de hora durante fechamento deste checkpoint: 15:10 UTC / 12:10
  Bahia. Primeiro checkpoint da manhã foi 06:53 Bahia; a correção de encerramento
  ocorreu depois de 07:26. Quota percentual continua desconhecida; não alegar 3%.

### 13/10/2026 — nova prévia inglesa e análise estática do executável

- Acrescentadas 36 entradas EN de conversas iniciais na rua e descritores de
  personagens; total 130 entradas. PT não ampliado. Revisão humana pendente.
- Validada reinserção dos três recursos; pacote privado inglês en-03 gerado,
  relido e hash conferido contra escrita. SHA256:
  ef792065cde7fbf2f3452c04b8ea616e2a9da8b1810f442fbf76960c75b2dbe6.
  Original M0 intacto. Manifesto atualizado só para EN; PT anterior 02 preservado
  com 94 entradas. Contagem agora também por idioma, evitando alegar igualdade.
- Publicação de manifesto usa troca atômica e recusa mistura de fontes/versões.
  Cinco regressões de idioma/publicação PASS. CheckOnly EN e PT PASS sem jogo.
- Páginas privadas inglesas: scene-review-en-03, três recursos/962 candidatos;
  exportam somente o idioma escolhido. Relatório separado scene-review-en.json;
  páginas e catálogo de revisão anterior preservados. 962 não significa cobertura
  narrativa certificada; inclui objetivos de outras etapas no recurso compartilhado.
- Medições de advances dos 130 textos nas duas fontes e três regressões PASS.
  Alguns avisos ingleses crescem até 1,625x nos exemplos examinados; outros
  encolhem. Medições não comprovam escala/fonte/layout na tela.
- Por autorização de Davy, desmontagem estática aprofundada no ELF próprio.
  Confirmadas LEAs de carga das duas fontes e da versão cockpit/map do HUD.
  Ferramenta reutilizável de xrefs com três testes sintéticos PASS; relatório só
  contém hashes/endereços/identificadores, nunca executável ou código completo.
- Defaults localizados em UTF-16. Cópias wcsncpy com cinco unidades e destinos
  separados por dez bytes; log real mostra IME max_length=4. Romaji completo
  exige tratar representação/limites e persistência; nenhuma truncagem aplicada.
  Prova e próximos consumidores em docs/m4/NOMES-PADRAO.md.
- Log 225841 analisado offline: sem séries de FPS, display trace ou readback;
  sem falhas detectadas pelos padrões do analisador. Não demonstra ausência
  dos defeitos visuais. Logs 230727/231419 encerram por fechamento do usuário.
  Nenhum ganho novo de FPS, correção do orbe ou piscada comprovados.

### 13/10/2026 — backend da lista de saves

- Navegação e cálculo de viewport separados em src/kyoei_list_model.h e
  usados pela apresentação atual. Preserva wrap, seleção e armazenamento.
- Corrigido underflow do número de linhas em janelas com altura insuficiente.
  Nenhum save real escrito; não declarar nova interface na imagem concluída.
- Build kyoei PASS sem execução. Teste headless list_model PASS e teste
  da ABI real com UI controlada test_save_list.py PASS.

### 13/10/2026 — entrada ativa, editor de nomes e estudo do motor

- Davy autorizou estudar o funcionamento completo e procurar fontes públicas.
  Fonte oficial identifica OROCHI 3/YEBIS 3; busca inicial não encontrou código
  público oficial do jogo/GxCore. Referências em docs/FUNCIONAMENTO.md.
- Modalidade ativa ligada aos eventos reais SDL e à digitação do IME nativo.
  Eixos com limiar/histerese; conexão, drift e eixo mantido não roubam modalidade.
  Acesso entre threads protegido, sem alterações no core fixo.
- Editor headless de nomes: grade/navegação, seleção inicial, caixa, espaço,
  apagar, cursor, confirmar/cancelar, limites e pares UTF-16. Modelo ainda não
  tem apresentação gráfica ligada ao jogo; não declarar overlay concluído.
- IME real verifica UTF-16 antes de escrever no buffer convidado, inclusive
  retorno de filtros. Preserva backend e nomes/saves existentes.
- Configuração/build kyoei PASS (não executado); testes headless name_model
  e input_mode PASS. Próximo: composição de overlay e roteamento da edição.

### 13/10/2026 — execução offline: inglês isolado e diagnóstico HUD

- Nenhum processo kyoei/shadPS4 encontrado na preparação; não abrir jogo.
  Espaço observado em G: ~47 GB. Dump/saves/core/Godzilla preservados.
- Construtor de prévias e validador agora aceitam --language en (ou pt-BR),
  sem exigir tradução da outra língua. Construtor aceita --validate-only:
  verifica hashes/tokens/nomes/reinserção sem criar PSARC ou publicar manifesto.
- As 94 entradas EN atuais passaram a reinserção dos três recursos. Dez
  testes sintéticos passaram: três de idioma isolado, dois tokens e cinco nomes.
  Nenhum novo pacote publicado; manifesto e prévias 02 permanecem vigentes.
- Davy acrescentou corte com reticências nas caixas de interação inglesas.
  Registrar ajuste de placa/medição/quebra como requisito; não encurtar por atalho.
- Identificados nós de texto/placa do minimapa em uma segunda versão HUD;
  estrutura BILZ confirmada, seletor de fonte e semântica dos campos pendentes.
  Investigação e próximo passo em docs/m3/HUD-INVESTIGACAO.md.

### 13/10/2026 — plano offline e ordem dos idiomas

- Recriado docs/PLANO-MANHA.md após confirmar que a gravação anterior não
  havia ocorrido. Referência adicionada à retomada.
- Davy pediu uma língua por vez: primeiro inglês, depois português. Revisar,
  ampliar e validar EN antes da nova rodada PT-BR; pacotes em sequência.
- Plano prioriza fontes/minimapa, entrada de nome e save/load, tradução e
  análise gráfica com testes offline. Não abrir jogo nem depender de Davy.
- Percentual da quota não acessível: checkpoints frequentes e reserva para
  encerramento, sem promessa de parada automática exata em 3%.

### 12/10/2026 — limite de quota e primeiro retorno da prévia PT-BR

- Davy respondeu que restam 4% de uso. Encerrar após salvar este checkpoint,
  conforme pedido de parar perto de 3%; não iniciar melhorias novas agora.
- Davy abriu a prévia PT-BR por conta própria. Relatou que "Parque: encontrar
  sua amiga" no minimapa se move estranhamente ao mover a câmera. A frase
  "Encontre sua amiga. Primeiro, chegue à avenida." aparece sem o "à".
  Acento ausente é compatível com a falta de glifos nas variantes, mas a fonte
  concreta dessa tela e a causa do movimento ainda não foram confirmadas.
- Prioridade da próxima sessão: rastrear a fonte do objetivo e corrigir os
  glifos necessários; investigar âncora/medidas/transformação do marcador
  do minimapa. Não remover acentos nem encurtar traduções sem diagnóstico.
- Processo do jogo observado PID 4772, iniciado por Davy; não fechar, reiniciar
  ou abrir outra partida. Código estava limpo no commit 062adc8 antes deste
  registro. Retomada detalhada em docs/RETOMADA.md.
- Log da partida de Davy: native-05/logs/20261012_225841.log. Confirma
  PT-BR montado de runtime-preview-01 (45 entradas), fullscreen=1 e saída
  1920x1080. A partida começou antes da publicação do pacote 02 (94 entradas);
  a próxima abertura usará 02. Não reiniciar a partida atual para trocar.
- Davy esclareceu o defeito do minimapa: o texto não acompanha corretamente
  o indicador ao mover a câmera. Corrigir/verificar tanto EN quanto PT-BR;
  o relato não certifica que ambas tenham sido testadas. Não tratar apenas
  como problema de tradução ou de comprimento sem diagnosticar a ancoragem.
- Davy gostou do resultado e confirmou conseguir selecionar o personagem
  entendendo tudo. Falta entrada de nome dentro do jogo, controlável pelo
  controle: detectar modalidade de entrada ativa (teclado ou controle) e
  apresentar a interface adequada. Pediu também save/load dentro do jogo,
  em vez das janelas nativas atuais. Preservar círculo confirmar/X voltar,
  navegação e funcionamento de save/load já conquistados.
- Nomes padrão precisam de romaji nas versões EN/PT-BR. Confirmar leitura e
  grafia antes de romanizar; nunca traduzir o significado ou substituir nomes
  personalizados do jogador. Localizar defaults fora dos diálogos e verificar
  entrada, exibição e persistência nos saves. Nenhuma implementação nova neste
  checkpoint; registrar para a próxima etapa, sem abrir/reiniciar jogo.


### 14/10/2026 — ícones de save e ciclo de montagem, offline

- SaveIcon limita leitura ao menor entre buffer_size/data_size, escreve em
  temporário exclusivo e substitui somente após escrita e flush completos.
  Falha preserva ícone anterior. Montagem somente leitura rejeita SaveIcon.
- Terminate retorna BUSY quando existem slots montados, preservando contexto;
  montagem revalida inicialização dentro do lock para evitar corrida.
- Backend real com fixtures temporárias PASS: buffer menor que data_size,
  escrita parcial/erro, preservação do ícone, readonly, remontagem e BUSY.
  CMake configure/build kyoei/test_kyoei_savedata e teste ABI PASS.
- Não é transação do payload inteiro nem proteção readonly de todos os arquivos.
  Nenhum jogo, janela ou save real utilizado. Tradução inglesa externa intacta.
- HUD: sonda de leitura identifica tipos UnitFontEx/GxUnitFontBmp nos nós de
  objetivo/navegação. Hash, offsets e construtor registrados em FONTES.md.
  Fonte final ainda desconhecida; nenhum atlas alterado ou diálogo exportado.

### 14/10/2026 — meta de desempenho e auditoria dos relatórios

- Davy definiu objetivo de pelo menos 60 FPS; plano técnico em DESEMPENHO.md.
- Analisador ignorava diagnostic=0/1: agora separa captura, não marcada e
  legado desconhecido, sem descartar pausas ou alterar total agregado.
  Teste sintético do CLI PASS, incluindo readback e janela lenta preservada.
- Log antigo 135113: 100 janelas, cenas misturadas, leituras lineares médias
  10,46 ms; amostras de uma imagem/espera/256 bytes apontam sincronização
  como hipótese a investigar. Não atribuir custo à largura de banda sem medir.
- Nenhum ganho de FPS declarado; nenhuma partida iniciada. Próximo trabalho:
  dependências/consumidor de readback, tempos de threads/fila GPU e pacing.

### 14/10/2026 — primeira correção de cache na investigação de 60 FPS

- Confirmado caminho de Finish síncrono antes de eventos EOS/EOP/ReleaseMem.
  Não removida sincronização obrigatória nem habilitado lote por palpite.
- Comparação com referência Bloodborne revelou mesmo erro na base: hash de
  imagem inteira comparado a prefixo 8x8. RefreshImage agora mede guest_size.
  Teste de métodos reais + XXH3 reproduz falha no original, passa no corrigido
  (intacta, escrita no último byte, CpuDirty). Core fixo preservado.
- Novos scopes de tempo de parede: readback-finish/map/publish, opt-in,
  agregados por thread. Testes de categorias, isolamento de threads e modo
  desligado PASS. Não são timestamps GPU nem ganho demonstrado.
- Port recompilado, CheckOnly EN com diagnóstico PASS, nenhuma partida aberta.
  Normal atualizado; orb-pending continua checkpoint anterior separado.
  Detalhes e próximo ponto sobre pacing/simulação em DESEMPENHO.md.

### 14/10/2026 — ambição de port nativo como etapa final opcional

- Davy esclareceu que recompilar significava avançar futuramente para renderer
  próprio, sem tradução gráfica de PS4; etapa final opcional, preservando o
  híbrido atual. Direção registrada na etapa 6 de PLANO.md.
- Separados renderer próprio e substituição integral da lógica do executável;
  descompilação não recupera automaticamente fonte compilável original.
  Nenhuma promessa de 60 FPS decorrente apenas dessa mudança de arquitetura.
- Davy relata Godzilla praticamente perfeito. Não tratado como benchmark do
  Kyoei, nem como autorização para alterar Godzilla/core fixo.

### 14/10/2026 — retorno de melhora percebida, quedas ainda presentes

- Davy abriu rapidamente e relatou desempenho bem melhor, ainda com quedas.
  Isso é percepção dele, sem comparação controlada ou FPS quantificado.
- Log 20261014_010525: janela 1280x720, RTX 2060, encerramento pelo usuário,
  Exit code 0. Sem frame/work stats; não permite atribuir as quedas. Nenhum
  processo Kyoei encontrado na conferência após relato. Agente não abriu jogo.
- Atalho Medir desempenho atual EN.cmd usa novo -PerformanceStats: métricas
  agregadas, HUD trace desligado, batch/readback experimental não habilitado.
  Não usar Testar desempenho EN como baseline normal: ele habilita BatchReadback
  e FitHud. CheckOnly valida a preparação sem executar partida.

### 14/10/2026 — retorno do GPT auditado e localização continuada

- Lidas regras retorno-gpt/AGENTS.md e fontes/relatórios. Extraídos original,
  baseline e working separados em retorno-gpt/codex-localization, fora do Git.
- Confirmados 173 JSONs/16100 entradas, inicialmente 4124 preenchidas, campos
  protegidos íntegros. Reparados 38 delimitadores 【…】 convertidos/perdidos no
  inglês recebido; wording preservado, justificativas e before/after privados.
- Inseridas 671 novas traduções: Stage8 219; Stage9 34; Stage3 6; Stage6 1;
  Stage10a 251; Stage11 63; Stage10b 97. Total 4795 preenchidas, 11305 vazias.
  Somente en/notes alterados; não confundir rascunho com aprovação editorial.
- Auditor automático e regressões PASS. Última auditoria full-set zero erros;
  28 recursos conferidos contra dump próprio e reinseridos em memória com
  strings restantes preservadas, antes do último lote 10b. Nada aplicado ao jogo.
- Vídeos atuais: 19 em 720p; índices 2fps concluídos 8/1/2/3/4/5/6, 26162
  amostras. Quadros pontuais lidos em resolução nativa e ligados a IDs quando
  demonstrado. Não é análise semântica integral nem cobertura de todos os ramos.
- Davy esclareceu explicitamente INTERROMPER AGORA e salvar. Varredura Stage7
  terminada; nenhum lote iniciado após isso. Consolidado cumulativo privado:
  published/Kyoei_English_Consolidated_Current.zip, SHA-256
  e66fa047159e70fa37f253b0ad83113bcf59148c4186e574b7739dda3678d09e.
  CONTINUE-HERE.md e CHECKPOINT.json registram plano, limitações e próximo lote.

### 14/10/2026 — retomada noturna até 03:00 autorizada

- Davy retomou a tradução e autorizou desligamento total às 03:00, Bahia,
  e checkpoints de cinco minutos. Horário inicial conferido 02:07 -03:00.
- Testado checkpoint auditado; registrado no Agendador Windows o checkpoint
  periódico (primeiro 02:14:18) e final/publicação/desligamento às 03:00.
  Sem /f. Scripts e log local em scripts/localization_shutdown.ps1 e
  build/localization-shutdown.log. Só os JSONs e metadados privados entram no ZIP.
- Relatório final por e-mail foi autorizado, com fallback no chat. Nenhuma
  ferramenta de envio de e-mail conectada; não prometer envio inexistente.
- Parada anterior revogada explicitamente. Continuar lotes até a reserva de
  fechamento; checkpoint atualizado em OVERNIGHT.md/AUTO-CHECKPOINT.json.


### 14/10/2026 - consolidado noturno salvo antes das 03:00

- Retomada autorizada conclu?da em reserva de fechamento: 6922/16100 entradas
  preenchidas, 9178 vazias. +2127 nesta retomada, +2798 desde o recebido (4124).
- Lotes ampliaram 10c/11/12/13/14/15/16/final e retomaram 1/2/4/7 e acompanhante.
  Ingl?s recebido preservado: zero apagamentos; 38 reparos de delimitadores.
  Corre??es editoriais pr?prias com before/after; nomes incertos n?o inventados.
- Auditoria dos 173 JSONs PASS; teste do auditor PASS. Todas as 6922 EN em
  117 recursos reais reconstru?das/relidas em mem?ria, strings restantes intactas.
  Nenhum jogo lan?ado, pacote runtime alterado, dump/saves tocados nesta etapa.
- 19 v?deos 720p conferidos; Stage7 retomado e ?ndice completo. Stages1-8
  somam 28495 amostras; quadros pontuais posteriores lidos. N?o declarar an?lise
  sem?ntica integral. Evid?ncias e gloss?rio provis?rio na pasta privada.
- Relat?rio docs/LOCALIZACAO-MADRUGADA-2026-10-14.md; retomada privada em
  CONTINUE-HERE.md/CHECKPOINT.json. Pr?ximo lote 0144 ap?s p0/k234, depois0145.
- Consolidado cumulativo publicado ap?s auditoria em published/; proveni?ncia
  no relat?rio privado published-checkpoint.json. As c?pias recebidas intactas.
- Checkpoints Windows a cada cinco minutos; desligamento autorizado agendado
  ?s03:00 Bahia, sem /f. N?o alegar execu??o j? observada. Sem sender de email;
  resumo local/chat como alternativa autorizada. Revis?o humana/layout pendentes.

- Conferencia final: tarefa periodica retornou0; desligamento03:00 habilitado,
  WakeToRun ativado so nessa tarefa. Triagem adicional deixou duas entradas
  recebidas com japones remanescente para revisao, sem alterar o ingles.

### 15/10/2026 — lote de 500 traduções (Claude)

- Davy pediu continuar a partir do diário com limite de 500 novas traduções.
- Lote claude-20261015-lote500 aplicado via apply_localization_batch.py:
  500 novas EN em 64 JSONs; total 12387/16100 preenchidas, 3713 vazias.
  Auditoria estrutural PASS; backup em checkpoints/before-claude-20261015-lote500.zip.
- Conteúdo: 284 rótulos de falante (nomes já confirmados; seis com leitura
  padrão inferida marcada nas notas), ~150 falas e ~70 itens/descrições.
  Pulados: notas de desenvolvimento, textos de teste, créditos e notícias longas.
- Consolidado não republicado em published/, pacote do jogo não alterado,
  jogo não iniciado. Revisão humana pendente.

### 15/10/2026 — segundo lote (pedido de 1000; 697 feitas)

- Davy autorizou liberar o Git (safe.directory global; autor local Davy) e pediu
  mais 1000. Aplicados sete sublotes claude-20261015-lote1000-a..g: 697 novas EN.
  Total 13084/16100 preenchidas, 3016 vazias. Auditoria estrutural PASS em cada.
- Conteúdo: notícias restantes, textos de leitura (revista Giant Shadow Weekly,
  cartas, contratos, cheques, bilhetes), os volumes de The Tale of the Last Sword,
  itens e cargos/empresas dos créditos. Leituras inferidas marcadas nas notas.
- 浅瀬尾 traduzido como "Asaseo" sem confirmação oficial; revisar.
- Não chegou a 1000 de propósito: o resto são nomes reais nos créditos (sem
  leitura confirmada), notas de desenvolvedor e placeholders 仮登録.
- Sublote "a" vazio criado por engano foi removido antes do reaplique.

### 15/10/2026 — prévia en-06, log de flip rate e opinião sobre 60 FPS

- Exportadas 1197 EN novas para translations/localization-en-additions-20261015-01.json
  (só inglês e hashes). Prévia privada runtime-preview-en-06: 13.214 entradas,
  roundtrip PASS, manifesto aponta en-06; PT-BR segue em runtime-preview-02.
  CheckOnly PASS. en-05 apagada (substituída e recriável). Gameplay não testado.
- Runtime agora registra no log cada sceVideoOutSetFlipRate (handle/rate).
  Objetivo: saber se o jogo limita a 30 Hz pelo flip rate. Davy precisa rodar
  "Medir desempenho atual EN" e enviar o log.
- Bloodborne-Windows: 60 FPS vem de patches no eboot 1.09 (comunidade,
  patches/Bloodborne.xml), não só do runtime. Para o Kyoei, engenharia reversa
  do eboot (limitador de quadros, passo de tempo da simulação) é o caminho
  equivalente; sem patch o jogo tende a rodar na taxa que ele mesmo pede.
- Revisão visual: quebras de palavra e ponto isolado indicam quebra de linha
  por caractere (estilo japonês). Correção robusta: localizar a rotina de
  quebra no eboot e quebrar em espaços (também engenharia reversa).

### 15/10/2026 — retorno de Davy sobre en-06 e correções

- Prints (Área de Trabalho 1-5.png, 01:57-02:01) e logs 20261015_015552/020354.
- Desempenho pior: causado pelo log novo de SetFlipRate, chamado a cada quadro
  (10.585 linhas numa sessão). Agora registra só quando a taxa muda. Rebuild PASS.
- Achado: jogo pede rate=0 (sem pular vblank, alvo 60 Hz). Limite atual de ~30
  FPS não vem do flip rate; vem do custo da emulação (CPU/GPU/sincronização).
- Minimapa desalinhado, "..." e escolhas cortadas: normal não ligava FitHud
  (só o atalho de teste). run_native.ps1 agora liga FitHud por padrão em EN;
  -OriginalHud desliga. CheckOnly EN PASS (ajuste HUD: True).
- Textura estranha do orbe: nunca foi corrigida (diagnóstico pendente desde 13/10),
  não é regressão desta sessão.

### 15/10/2026 — menu de opções em inglês (texturas)

- Davy: mais rápido, ainda com stutters/quedas; menu Options em japonês.
- Rótulos do menu são texturas texture/menu_option/*.tex (BC3 + mips, BILZ/zlib).
  Nova ferramenta tools/localize_textures.py apaga o texto e desenha o inglês
  (NotoSans) sobre o fundo original; tabela translations/textures-en.json (38).
  build_runtime_translation.py aceita --textures-en. Prévia en-07 (13.214 textos
  + 38 texturas) ativa, CheckOnly PASS; en-06 apagada. Visual no jogo pendente.
- Títulos de fase (texture/stage_title/ST_*.tex, 19) em inglês com Cambria Bold
  e brilho escuro, nomes de intermission_result_data. Prévia en-08 (57 texturas)
  ativa, CheckOnly PASS; en-07 apagada. Ideias de Davy registradas: ícones de
  teclado (mapa em godzilla-core/gpu/pad.cpp; port já detecta modo teclado) e
  logo em inglês (title_logo.tex; avaliar estética antes de ativar).

### 15/10/2026 — encerramento da sessão Claude (retomar no Codex)

Estado: prévia runtime-preview-en-08 ativa (13.214 textos EN + 57 texturas:
menu Options e títulos de fase). EN 13.084/16.100 no consolidado privado; patch
em translations/localization-en-additions-20261015-01.json + textures-en.json.
FitHud ligado por padrão em EN (run_native.ps1, -OriginalHud desliga).

Retorno final de Davy:
- Options traduzido, mas tamanhos de fonte desiguais entre rótulos (label()
  reduz a fonte para caber: padronizar um tamanho único para todos e encurtar
  textos que não cabem) e botões de seleção mal acabados (relabel() apaga o
  texto com cor sólida e deixa marcas na borda/gradiente: preferir recompor o
  botão a partir de uma faixa limpa do próprio botão, ex. copiar coluna sem texto).
- Jogo melhor e jogável, ainda com stutter; deseja 60 FPS. Jogo pede flip rate 0
  (não há trava de 30). Investigar: compilação de shaders na primeira aparição
  (stutter na abertura), esperas Finish/readback, coletar com
  "Medir desempenho atual EN.cmd". Davy deixou uma partida aberta; não fechar.

Próximos passos sugeridos: (1) acabamento das texturas de Options; (2) ver
títulos de fase no jogo; (3) desempenho/stutter; (4) ícones de teclado: opção
no launcher primeiro, troca dinâmica depois (mapa em godzilla-core/gpu/pad.cpp);
(5) teste de logo EN só com aprovação visual de Davy; (6) quebra de linha por
espaço (rotina de texto do eboot); (7) PT-BR depois.

### 15/10/2026 — retorno do portable, mouse e desempenho no Codex

- Davy relatou Options e títulos dos capítulos bons no portable; isso não
  constitui aprovação definitiva das texturas, conforme esclareceu depois.
  Relatou muitas
  quedas/stutter; prioridade desempenho, alvo RX 580 + i5-9400. Deseja Linux
  junto ao Windows em pasta enxuta. Esclareceu manter somente x64, sem 32 bits.
- Log privado portable: 174 janelas/20.880 intervalos, 23,64 FPS agregados,
  máximo 767,32 ms; cenas/menus misturados. Shader até 22,91 ms, pipeline
  gráfico até 39,59 ms; readback-finish média 14,98 ms. DESEMPENHO.md atualizado.
- Filtro dos logs de sucesso de mapeamento reduz lock/stdout/fflush no
  streaming; KYOEI_MEMORY_TRACE=1 restaura detalhe. Erros/relatórios preservados.
  Novas métricas scheduler-submit/scheduler-wait/queue-lock; Finish mantido.
  Nenhum ganho de FPS declarado sem nova medição.
- Câmera por mouse implementada via SDL3/analógico direito, cursor relativo
  enquanto focado, liberação em perda de foco/modais; F7 alterna captura.
  Teclado IJKL e controle continuam disponíveis. Testes sem partida PASS;
  sensação/sensibilidade e captura real em gameplay ainda pendentes.
- Build PASS; testes mouse/log filter/frame stats on/off/pipeline cache/
  overlay/input mode PASS; cinco regressões de runtime translation PASS.
  CheckOnly EN com PerformanceStats PASS, sem partida iniciada pelo agente.
- Escolhas locais revistas: I'm a man. / I'm a woman. Prévia en-14 preparada,
  13.226 entradas + 59 texturas. Verificação leu pacote: ambas as escolhas
  corretas; todas as 59 texturas byte-idênticas às anteriores.
- Auditoria privada: 13.096/16.100 preenchidos, 3.004 vazios; 2.121 cadastros
  provisórios, 256 modelos de notícia, 320 créditos e 235 revisão individual,
  além de outros grupos. Há notícias/textos narrativos ainda sem EN. Detalhes
  em LOCALIZACAO-PENDENCIAS.md. Nenhum JSON privado alterado nesta continuação.
- Falha de atualização: ferramenta copiou idioma en-13 antes de descobrir
  executável bloqueado pela partida portable PID9416. En-13 havia sido gerada
  sem texturas por caminho incorreto aceito silenciosamente. Não fechar jogo.
  Erro comunicado a Davy. Corrigido gerador para rejeitar pasta vazia/inexistente;
  en-14 recomposta com 59 texturas e verificada. Prévia inválida en-13 removida
  após conferir substituto, publicação e conteúdo; metadados/evidência no diário.
- Builder agora verifica os executáveis antes de qualquer cópia e mantém
  session.lock durante atualização. Atualizações de cópia já finalizada
  atualizam hashes/metadados, sem manter alegação antiga de varredura completa.
  data/saves não são apagados. Recusa com partida ativa verificada.
- Atualizador oculto PID3048 aguarda fechamento do jogo E launcher; ferramenta
  tools/update_portable_when_closed.py. Estado em build/portable-update-status.json;
  stdout/stderr em build/portable-update.*.log. Ainda não declarar portable
  atualizado enquanto estado não for complete; pasta em uso pode conter o
  idioma incorreto da tentativa inicial até a atualização ser aplicada.
- Linux: referência Bloodborne tem caminhos POSIX reais (probe/runtime_memory/
  build.sh); base Kyoei depende de Win32 e bibliotecas .lib. WSL não instalado.
  Nenhum executável Linux entregue. Mapa técnico/critério de entrega em LINUX.md.
  Snapshot Godzilla e projetos irmãos preservados. Nenhuma partida encerrada.

### 15/10/2026 — revisão isolada, nomes romaji e ambiente Linux

- Davy corrigiu: as texturas não estão definitivamente aprovadas. Corrigido
  o registro anterior. Nova galeria privada build/texture-review/index.html:
  59 pares original/inglês, 118 PNGs nativos, zoom e decisões/notas exportáveis.
  Todas pendentes por padrão; aberta no navegador. Conferência Edge/headless
  e screenshot inspecionada. Fonte/documentação em TEXTURAS-REVISAO.md.
- Davy reiterou nome/sobrenome em romaji, edição dentro do jogo e teclado
  virtual apenas quando estiver usando controle. Overlay/modelo já existiam,
  mas portable não habilitava: launcher agora define IN_GAME_DIALOGS=1 em EN.
  Launcher de desenvolvimento inicia checkbox marcado; preferência anterior
  salva ainda pode prevalecer. Fallback nativo preservado se backend falhar.
- Implementada apresentação reversível dos quatro defaults: Ken/Misaki e
  Miharu/Matsubara. Editor mostra romaji completo e devolve UTF-16 original
  ao convidado se mantido. Nome personalizado continua literal e limitado a
  quatro unidades; valores maiores não são truncados nem confirmados.
- Adapter da conversão wcstombs_s confere thunk/GOT/export nativo; atua nos
  dois campos verificados e concatenação no retorno0xac4e64. Armazenamento e
  saves não mudam. Fonte fora do escopo segue à conversão original. Política
  e referências das leituras documentadas em m4/NOMES-PADRAO.md. Cobertura
  visual de todas as telas ainda não comprovada.
- Build PASS. NAMES/modelo/overlay/ABI IME PASS, incluindo guards, quatro
  defaults, customização e ida/volta. Desenho ImGui real verifica que teclado
  virtual some ao usar teclado físico e retorna com controle. Quatro testes
  de xrefs PASS; suporte novo a alvo por endereço sem exportar prosa original.
- Bootstrap EN sem GPU/list-stubs PASS no build e no binário do portable;
  log confirma ligação romaji; nenhum código do jogo executado. Portable
  recomposto após Davy avisar que fechou a partida: 13.226 textos/59 texturas,
  launcher --check PASS, dados pessoais/saves preservados. Nenhum jogo aberto.
- F7 explicado: atalho opcional entre câmera pelo mouse e seta livre; perda
  de foco já libera automaticamente. Não depende de F7 para usar Alt+Tab.
- Pergunta dos 6 GB respondida: é common.psarc completo com tradução, não
  6 GB de inglês. Distribuição futura deve conter alterações/ferramentas;
  reduzir duplicação do runtime ainda é trabalho pendente, sem prazo prometido.
- Linux: tentativas comuns WSL falharam; preparado script com -NoRestart e
  executado com elevação administrativa do Windows (PID5440). Status final
  restart_required: componentes WSL/VirtualMachinePlatform preparados; nenhum
  reinício feito. Ubuntu/toolchain e executável Linux ainda pendentes. Após
  reinício voluntário, retomar scripts/setup_linux_build.ps1 e o backend POSIX.
- Novo achado no log da partida encerrada por Davy: checkpoint do cache Vulkan
  deu timeout no encerramento normal. Não afirmar cache de driver persistido.
  Investigar separadamente; não remover sincronização por suposição. Desempenho
  continua prioridade e sem ganho novo certificado nesta continuação.

### 15/10/2026 — timeout reproduzido e revisão preservada

- JSON de Davy encontrado diretamente em Downloads; importador valida inventário,
  hashes e notas. Resultado: 1 aceita, 58 revisar. Backup privado verificado em
  build/texture-review-backups/e967c3f206d7a4b4: TEX atuais, galeria, revisão e
  manifesto SHA-256. Nenhum ajuste visual aplicado nesta etapa.
- Não há garantia de localização integral: 59 imagens entre 1.412 candidatas
  UI/história; nem todas precisam tradução, inventário visual incompleto.
  Textos ainda têm lacunas narrativas; não afirmar diálogos/legendas 100%.
- Timeout reproduzido sem código do jogo: thread gráfico real ocupado por
  fixture 4,2 s; implementação antiga falhou após três segundos, phase=0.
  Captura Vulkan agora ocorre no proprietário durante sessão; bytes seguem
  para gravador CPU limitado. Saída espera gravações pendentes sem fila GPU.
  Mesmo teste passou em 0,03 ms, exit=0. Teste unitário PASS e build PASS.
- Limites: fixture salva só cabeçalho de 36 bytes. Cópia privada dos shaders
  pessoais encontrou perfil incompatível e não validou cache populado.
  Últimos pipelines desde checkpoint podem ser recompilados depois; prazo de
  disco continua limitado. Nenhuma melhora de FPS medida. Ver DESEMPENHO.md.
- Portable atualizado com correção; verificação launcher/bootstrap EN PASS,
  13.226 textos e 59 texturas, adapter romaji ligado. Saves preservados.
  Nenhuma partida iniciada/encerrada. Linux ainda aguarda reinício voluntário
  e implementação; nenhum binário Linux prometido como entregue.

### 15/10/2026 — progresso salvo e prioridades de Davy

- Pedido explícito: salvar progresso. Código anterior salvo no commit 29a352a;
  portable atualizado, árvore limpa antes deste registro. Restam, segundo Davy,
  cerca de 40% do limite disponível; isso não certifica conclusão de etapas.
- Ordem para a continuação:
  1. Corrigir as 58 texturas conforme revisão importada, preservando a cópia
     atual e oferecendo comparação visual. Não presumir aprovação das novas.
  2. Melhorar desempenho ao máximo com medições de FPS/tempos/stutter e provas
     de correção. Alvo inclui RX580/i5-9400; não prometer taxa sem teste.
  3. Compilar e validar Linux x64, mantendo portable enxuto. Componentes WSL
     aguardam reinício voluntário; ainda não existe executável Linux entregue.
  4. VERIFICAR jogo 100% jogável: progressão do início ao fim, capítulos,
     controles, nomes, saves, diálogos/legendas/texturas e textos necessários
     ao jogador. Inventário ou bootstrap não substituem prova em jogo.
  5. Corrigir toda lacuna/bloqueio encontrado até atingir esse objetivo;
     registrar separadamente o comprovado, o pendente e o que exige teste humano.
- Autorização anterior para trabalho permanece. Manter x64, saves e dump;
  preservar snapshot Godzilla e não alterar projetos irmãos. Não iniciar nem
  fechar partida automaticamente. Próxima sessão começa lendo este diário,
  a revisão preservada e os documentos de desempenho/localização/Linux.

### 15/10/2026 — retomada após reinício, proposta visual e Ubuntu x64

- Davy reiniciou o PC e pediu continuação. Lidos diário, AGENTS, revisão e
  documentos de Linux/desempenho. Executor restrito falha antes de criar
  processos (setup refresh); comandos executados com escalonamento autorizado.
- Nova proposta privada de texturas em build/textures-revised. Galeria
  build/texture-review-v2/index.html aberta no Edge: 59 grupos/177 PNGs,
  original/anterior/revisada. Mantidos versão atual, backup e portable.
- Corrigido centro vertical dos botões; seleções ganham altura baseada nos
  glifos brancos originais, sem alargamento; sombra difusa mais visível.
  Títulos recebem sombra mais larga e Stage maior; aviso inicial maior.
  Amostras visuais inspecionadas; teste de bordas/gradientes PASS.
- Verificação de hashes/decodificação: 47 TEX mudaram; 11 botões selecionados
  já estavam centralizados e ficaram idênticos; ficção aprovada preservada
  byte a byte. Todas as 59 mantêm dimensões, mipmaps, descritores e tamanho
  descomprimido. Nova aprovação visual pendente; não afirmar 58 concluídas.
- Script Linux retomado com token administrativo do Windows: instalados
  WSL 3.0.3/Ubuntu 26.04.1 LTS (WSL2). Nenhum novo reinício executado.
  Ubuntu inicia por root e confirma x86_64. Instalados compilador Clang21,
  CMake4/Ninja, Vulkan/SDL3/FFmpeg de desenvolvimento.
- Novo scripts/setup_linux_toolchain.sh permite repetir instalação ou
  --verify-only. Verificação PASS: compilou e executou programa C ELF x86-64.
  Esse programa é só teste do compilador; ainda não há port Linux compilado.
  Instalação apt terminou exit0; aviso de bus systemd durante triggers não
  impediu o teste. Backend POSIX/TLS/runtime e gráficos continuam pendentes.
- Nenhuma partida iniciada/encerrada; saves, dump e Godzilla preservados.
  Nenhum ganho novo de FPS medido. Pendências: revisão visual da proposta,
  desempenho com medição, backend Linux e validação do jogo até o fim.

### 15/10/2026 — pixels de Options, textos novos, espera GPU e carregador Linux

- Davy aprovou os títulos dos stages, pediu Notice com linhas centralizadas
  como a referência enviada e centralização por pixels dos Options. Pediu
  avanço concreto também em desempenho, textos/jogabilidade e Linux.
- Referência baixada/inspecionada localmente. Notice agora centraliza cada
  linha. Achado: atlas Options tem 44px, mas borda termina em y35; o padding
  transparente inferior deslocava o cálculo anterior. Gerador usa a área
  interna da borda e pixels reais das letras. Teste sintético PASS e medição
  dos 22 TEX comprimidos PASS: espaços superior/inferior diferem no máximo
  1px. Galeria v3 aberta; stages aprovados e ficção preservados byte a byte.
- 34 EN novas realmente inseridas no consolidado privado, com backup e ledger
  player-facing-20261015-02: 7 artigos completos, 6 registros de máscara,
  2 de roteiro, 1 nome de chave e 18 interações. Auditoria estrutural PASS.
  Privado 13.130/16.100, vazias 2.970; 201 exigem revisão individual na triagem.
  Reinserção/releitura integral das 13.130 EN em 135 recursos PASS. Restam
  textos narrativos, nomes e inventário visual; sem prova de 100% jogável.
- Exportado patch só com 34 EN/hashes, sem prosa original. Prévia en-15
  recomposta: união de 13.260 EN e 59 TEX. PSARC criado com hash e releitura
  de substituições verificados. Portable fechado atualizado, preservando
  saves. Leitura direta do portable verificou as 34 novas e 59 TEX; hash,
  launcher --check e bootstrap sem GPU/list-stubs EN PASS. Romaji ligado.
- Melhoria real do adapter de espera GPU: espera ativa limitada a 200us;
  timer de alta resolução de 1ms depois, fallback Sleep(1). Fence acquire,
  timeout e poll/trap original mantidos. Não altera label nem simulação.
  Fixture A/B com timer de 1ms: 50/200/800us detectados em 1,881/1,922/1,974ms
  antes e 0,074/1,703/1,711ms depois. Custo em ciclos de CPU maior, registrado
  em DESEMPENHO.md. Sem pedido de timer, redução maior; não extrapolar para
  gameplay. Build PASS; fence/timeout/relay/100 timeouts sem leaks PASS.
- Linux agora tem alvo ELF real kyoei-linux-bootstrap, derivado do carregador
  fixo com âncoras verificadas; SHA256 loader original
  7de8feb69d3897eee10e11ccaa2b3edb3bcbe44c6190428592d4aeed900fe381.
  Host mmap/mprotect/pthreads próprio; CMake Linux sem libs Windows.
  Imagem própria real: 29.540.440 bytes, 36 segmentos, 88.561 relocações,
  418 imports nativos, 748 ainda sem runtime Linux. --list-stubs obrigatório;
  entrada do convidado ausente/recusada. Nenhum launcher Linux publicado.
- TLS Linux principal implementado com GS, FS da glibc preservado. Testado
  template/zeros/DTV, alinhamento convidado independente do host, quatro
  threads, isolamento/errno, entradas aninhadas e restauração de GS. As 580
  instruções reais conferidas e prefixos preparados na imagem; probe GS do
  host PASS. Testes de recusas do loader PASS. Ainda faltam runtime PS4,
  módulos/TLS dinâmico, memória/faults, GPU/input/saves e validação real Linux.
- Nenhuma partida iniciada/encerrada, nenhum FPS novo medido. Dump, saves,
  snapshot Godzilla e projetos irmãos preservados. Prosseguir com runtime
  POSIX e gargalos medidos; visual/100% da progressão exigem prova em jogo.
- Limpeza conforme regra permanente: removido só common.psarc derivado en-14
  (6.349.079.299 bytes), após conferir SHA256 registrado, substituto en-15,
  hash/leitura do portable e ausência de jogo/atualizador ativo. Metadados de
  prova antigos, referência visual, traduções e backups preservados. Evidência
  privada em build/cleanup-preview-14.json. PT-BR e demais fontes não removidos.

### 15/10/2026 — interrupção do Codex por limite; retomada pelo Claude Code

Pedido de Davy: Linux jogável para o amigo (CachyOS recente, KDE), portable com
exe Windows e Linux, aba de controles no launcher (controle e teclado) com
sensibilidade do mouse, teclas estilo PC, mouse mais sensível, nome do
personagem com mais de 4 letras e usado nas legendas, ícones de teclado no
jogo, opção "voltar à área de trabalho" no menu inicial.

Feito (código compila no Windows; testes PASS; NÃO validado em partida):
- `src/kyoei_pc_controls.h`: teclas PC (WASD já no stick; E/Enter confirmar
  0x2000, Espaço/Backspace 0x4000, Ctrl 0x8000, F 0x1000, Shift 0x800, Q 0x400,
  V 0x100, G 0x200, Esc/Tab 8, M touchpad, H 2, R 4, setas = direcional),
  conferidas no diagrama operation_normal.tex. Mouse: botão esq. 0x2000,
  dir. 0x200, meio 4. Patch aplicado no pad.cpp via CMake.
- `kyoei_mouse.h`: `KYOEI_MOUSE_SENSITIVITY` (padrão 1.8, antes 1.0).
- Launcher: campo de sensibilidade e `KYOEI_NAME_STORE`.
- `kyoei_pc_names.c`: nome longo (até 24) guardado em
  `data/user/character-names.bin`; o jogo recebe token de 4 unidades
  (ZAAA…) e a exibição troca o token pelo nome. Teste cross-process PASS
  (`test_kyoei_pc_names.exe <arquivo> write` e depois `read`).

Falta:
1. Validar no jogo: nome longo na criação e nas legendas (o nome genérico
   japonês nas legendas ainda não foi investigado: provável texto com nome
   padrão fixo, não o campo do jogador).
2. Aba "Controles" do launcher listando todos os comandos (controle e
   teclado) — só a sensibilidade foi adicionada.
3. Ícones de teclado no jogo (troca de texturas de botões quando o modo de
   entrada for teclado) — não iniciado.
4. "Voltar à área de trabalho" no menu inicial — não iniciado.
5. Linux: build gráfico (bbgpu/shadPS4) em WSL Ubuntu parou em
   vk_*.cpp: cabeçalhos Vulkan do sistema incompatíveis com extensão VALVE
   da referência (usar Vulkan-Headers do submódulo). Runtime POSIX: 748
   imports sem implementação Linux. Sem launcher Linux. Longe de jogável.
6. Republicar portable só após validação; o portable atual é o anterior.

### 15/10/2026 (tarde) — Claude Code: itens de PC concluídos e verificados no port

- Voltar à área de trabalho (efd87cd): rotina UnitHudGameTitle 0xaaf9b0
  (cursor +0x1b8, limite 4 itens; +0x1b0 libera entrada) interceptada por
  relay com hash. Baixo no 4º item seleciona a 5ª opção desenhada pelo
  overlay; confirmar duas vezes (E/Enter/○ ou clique) fecha via
  SDL_EVENT_QUIT. Verificado: posição sob o menu e saída com código 0.
  Limite: o sublinhado nativo de "????" continua visível com a 5ª ativa.
- Nome do personagem (a02d1ac): causa real do "nome padrão" encontrada.
  O filtro do jogo (0xb05870) só converte kana meia-largura e devolve -1
  quando não muda nada; o adaptador tratava -1 como recusa e o jogo voltava
  ao padrão (御崎 健 → "Misaki Ken"). Agora mantém o texto. Verificado:
  "My name is Montgomery Alexander." Prompts 姓/名 em inglês.
- Ícones de teclado (10a4244): 21 ícones da fonte + HUD (alternar alvo,
  esquiva QTE, marcadores de conversa, Scene Skip, L1/R1 do menu de pausa,
  botões do intervalo) redesenhados como teclas. Entregues como delta de
  leitura (~2 MB por idioma) aplicado pelo runtime sobre common.psarc,
  sem copiar 6 GB; delta que não confere com a base é ignorado. Verificado
  em jogo: "Switch target" com setas; talk marker com E.
- Launcher (5dcf124): Controles com todas as ações, sensibilidade e
  escolha de ícones (teclado/controle). Portable reconstruído com os
  deltas EN/JA e README de teclas novo.
- Linux: Wine 10 em WSL carrega o executável Windows e mapeia a imagem em
  0x800000000 (bootstrap --list-stubs PASS). Caminho rápido proposto para o
  amigo (CachyOS): Proton/Wine. Port nativo Linux continua pendente
  (runtime POSIX e GPU).

### 15/10/2026 (noite) — anotações de Davy

- Return to Desktop nativo (3ce514e, d34051c): arte gerada no estilo dos
  itens (Segoe UI Semibold ajustada a title_menu_newgame, halo/foco
  ajustados), posição/fade lidos dos nós do menu (slot 13, cor +0x94),
  sublinhado nativo movido, "????" desmarcado; ↑ não pula mais "????".
  Upload como textura separada travava o driver; arte vai no atlas ImGui.
- Quebra por palavras (b03e02e): hook em 0x2e2b10 remede até o último espaço.
  Reticências japonesas → "..." (181); nomes em ordem ocidental com espaço.
  Recomposição EN en-16 (checagem de tokens agora por conjunto, não ordem).
- Ícones: caixa branca removida; segundo demo_skip_icon (texture/hud).
- L1/R1 do menu: hook do UnitMenuManager (0xb2c600) testado; +0x178 não
  muda ao abrir o menu. Revertido; pendente.
- Linux via Wine (WSL, llvmpipe): executa código do jogo e para antes da
  abertura de VideoOut (sem GPU real no WSL; dzn ausente). Não comprovado
  com GPU real. Portable inclui "Start on Linux.sh".
- Portable reconstruído (en-16, deltas EN/JA 41 ícones, arte do título).

### 15/10/2026 — Codex: primeiros módulos do runtime POSIX

- Lidos CONTINUAR-CODEX.md e docs/LINUX.md. Estado inicial do Git limpo.
- RTC e semáforos/event flags derivados do snapshot fixo com âncoras
  verificadas; código próprio em src/linux e integração Linux exclusiva.
  Relógio/localtime POSIX e sincronização pthread com timeout monotônico;
  destruição de mutexes/condições após saída dos esperadores.
- Compilação real Ubuntu x86-64/Clang PASS; quatro CTest PASS (RTC, semáforos,
  TLS e recusas do loader). Testes concorrentes verificam sinalização,
  cancelamento e exclusão com threads efetivamente bloqueadas.
- Teste concorrente também PASS com AddressSanitizer/UndefinedBehaviorSanitizer,
  sem diagnóstico de memória ou comportamento indefinido.
- Auditoria da imagem preparada do dump: 10 imports no runtime Linux,
  418 nos módulos nativos, 738 stubs (antes 748). Log privado:
  build/linux/bootstrap-posix-runtime.log. Nenhum código do jogo executado.
- Linux segue incompleto: memória/threads/arquivos/serviços/saves/GPU.
  L1/R1 da pausa e conferência visual das quebras/ícones/seta continuam
  pendentes. Portable e saves não alterados; Godzilla e snapshot preservados.

### 15/10/2026 — revisão das capturas 1–4 e prioridade a testes Windows

- Pedido: analisar falhas nas quatro imagens da Área de Trabalho; priorizar
  tradução jogável/portable Windows, depois tentar Linux. Pedido adicional:
  nenhuma identificação pessoal/de ferramentas na cópia para os amigos.
- Diagnóstico e limites em docs/REVISAO-HUD-20261015.md. Delta de correções:
  455 strings com pontuação latina, 25 rótulos HUD e dois textos de abertura
  ajustados; 62 recursos no delta, leitura/ida e volta verificadas. Cinco
  literais curtos do executável traduzidos com âncoras. Snapshot intocado.
- Ícones passam de delta fixo a 43 pares de texturas selecionados por entrada
  recente no upload GPU (microtiles, mipmaps e padding verificados). Em jogo,
  setas trocaram para L1/R1 com evento SDL simulado; novo E visto no Save.
  Não confundir simulação com teste de controle físico; fonte e Xbox pendentes.
- Pausa identificada por texturas desenhadas: setas na tela principal, A/D nas
  páginas internas preservando a grade. Janela de contexto evita oscilações
  causadas por apresentações repetidas. Título da janela sem sufixo PC Port.
- Regressão detectada no loading durante tentativa de alterar o campo de
  overflow do medidor. Escrita retirada; save isolado voltou a carregar.
  Evidências build/ui-final-pause.png e ui-auto-{keyboard,controller}.png.
  Saves pessoais não usados para gravar durante a validação.
- Portable pessoal atualizado. Exportação limpa separada preparada por
  tools/export_private_test.py; exclui data/, confere hashes e procura nomes
  pessoais/de ferramentas. Relatório privado build/friends-export-privacy.json:
  PASS, 129 arquivos, zero achados; nenhum dado pessoal copiado. Exportação
  final inclui o iniciador Wine para Linux e passou no launcher --check.
- Quebra universal, objetivo inferior/bússola, alguns prompts e traduções
  restantes ainda exigem trabalho. Nenhuma alegação de campanha 100% concluída.
- Linux: cabeçalhos Vulkan oficiais isolados em build/linux-full/Vulkan-Headers,
  commit c46850864f4661461b0f6cb9922c058ffea4915e. Nova tentativa de bbgpu da
  referência local em andamento; reprodução em scripts/build_linux_gpu_probe.sh.
  Biblioteca de referência não equivale à integração/jogo Kyoei Linux.
- Resultado final Linux: bbgpu compilada com sucesso (ELF x86-64), após
  resolver Vulkan-Headers e Xbyak oficiais em caches isolados. Xbyak:
  cd6bad3cad2e07a3e277c23d2b1f065320024717. Quatro CTests Linux PASS.
  Continua sem integração do jogo: 738 stubs na última auditoria; nenhum
  executável Linux jogável entregue. O script do portable usa Wine.
- Windows registrado no commit e52aa75; testes de UI, tradução, privacidade,
  mouse/pausa, texturas e quebra passaram. Exportação limpa com 129 arquivos
  e nenhum achado de identificação; saves pessoais preservados à parte.

### 15/10/2026 - pausa imediata solicitada (limite de uso)

- Davy pediu seguir com Linux NATIVO; depois pediu interromper e salvar agora.
- Novo alvo opt-in kyoei-linux em cmake/LinuxNative.cmake. Executavel real ELF
  em build/linux-native/kyoei-linux, runtime POSIX completo integrado da
  referencia local e5d04e2ba4864ccf40de6c3cf6aa532855320b0d. Apenas copias de
  build adaptadas por tools/prepare_linux_native.py; snapshot preservado.
- Adaptados escopos NID do Kyoei, base fixa 0x800000000, 580 cargas TLS FS->GS
  verificadas, usuario neutro Player, SDK, heap tracing, AppInfo, condattrs,
  modulos tardios do proprio Kyoei com pthreads/OpenSSL. FSR/object motion
  especificos da referencia desativados. Novos contratos precisam de testes.
- PROVA REAL: libc e nove inicializadores de modulos passaram; entrada original
  executada, 2498 __cxa_atexit, memoria direta ~5,1 GB, quatro threads. Teste
  CPU chegou a sceVideoOutOpen (esperado sem GPU). Log native-cpu-run.log.
- GPU: janela SDL/Vulkan inicializou, 1726 simbolos HLE, SDK 0x04508101;
  execucao passou por VideoOut, chegou a seis threads e parou inicialmente em
  sceErrorDialogInitialize. Acrescentado dialogo de erro SDL (nao e no-op).
- Ultimo build apos ErrorDialog e desativacao FSR PASS. Ultima execucao GPU
  interrompida a pedido do usuario, PID Linux 513 confirmado pelo comando;
  sem prova de primeiro frame, menus ou gameplay. Logs privados em
  build/linux-full/native-{configure,build,cpu-run,gpu-run}.log.
- Teste usa build/linux-native-test-user; nenhum save pessoal alterado. Nada
  do Linux experimental foi inserido na entrega Friends-Test. Privacy Windows
  continua valida; binarios Linux ainda NAO passaram pela limpeza/exportacao.
- Retomar pelo novo bloco inicial de CONTINUAR-CODEX.md. Nao confundir com o
  bootstrap antigo que proibe executar convidado. Ainda faltam mais contratos,
  traducao, controles Kyoei, UI/save/load, primeiro frame, empacotamento/testes.
- Conferencia apos salvar: PID 513 ja havia encerrado antes do SIGTERM; ps
  confirma nenhum kyoei-linux ativo. O teste terminou por sua propria parada,
  nao pelo sinal. Native-gpu-run.log contem o bloqueio final para a retomada;
  chegou tambem a acesso real de arquivo e duas alocacoes flexiveis.

### 15/10/2026 — desempenho Windows, continuação apenas no backend

- Davy trouxe relato de ~20 FPS/slow motion no Linux do amigo e autorizou
  investigação também no Windows. Depois pediu não testar agora e seguir só
  no backend. Não abrir novas sessões automaticamente; gameplay fica para depois.
- Estado inicial limpo. Lidos AGENTS, diário, CONTINUAR-CODEX e DESEMPENHO.
  Registro Windows atual analisado: 302 janelas, mínima de 16,78 FPS por janela,
  cinco janelas scheduler-wait com média de 10,94 ms. Inclui menus/jogo; não é
  benchmark da cidade. Resumo privado performance-current-20261015.json.
- Custo removido: buffers raster de diagnóstico eram zerados em todo desenho
  direto/indireto, mesmo com captura desativada. BeginDraw agora evita essas
  escritas quando o diagnóstico está desligado. Quando ligado, limpa antes
  do pedido F9 também, preservando pedidos entre início do desenho e bindings.
- Novas métricas opt-in: command-batch e present-pacing, mais contagens por
  intervalo acima dos limites de 60/30 Hz, 50/100 ms. São tempos de parede e
  intervalos de apresentação; não provam taxa da simulação/causa de slow motion.
  Analisador separa janelas de diagnóstico e não inventa contagens de logs antigos.
- Roteiro -PerformanceStats usa perfil separado, registro com data e desliga
  traces extras. Sessão isolada PID 3604, aberta antes do pedido de backend,
  mostrou abertura e novas métricas; já havia encerrado pela janela com código
  0 antes da próxima entrada. Nenhum gameplay carregado. Nenhum jogo ficou ativo.
  Log privado performance-review-20261015-215344.log; saves pessoais preservados.
- Windows kyoei.exe recompilado PASS. Testes frame stats/raster ligados e
  desligados PASS; 11 testes do parser raster PASS; analisador de desempenho,
  readback real, ordem de superfícies e propriedade do flip PASS. Contratos de
  readback/fences mantidos. Snapshot fixo, projeto Godzilla e portable intocados.
- Nenhum ganho de FPS ou correção de câmera lenta declarado. Próxima etapa:
  correlacionar as novas métricas em uma cena fixa quando houver teste de jogo.

### 15/10/2026 — engenharia reversa estática do quadro, tempo e HUD

- Pedido de Davy: aprofundar entendimento do jogo, começando pelo ciclo de
  um quadro. Mantida a orientação de backend: nenhum jogo aberto nesta etapa.
- Ferramenta nova tools/inspect_frame_flow.py: índice GNU unwind validado
  contra FDE/CIE e segmentos; 42.194 intervalos de função. Candidatos CALL/JMP
  só aceitos após desmontagem LLVM iniciada na fronteira da função. PLT ligada
  às relocações do boot.bin original. Nenhuma dependência Python nova.
- Receita hash-bound docs/re/frame-flow-facts.json e mapa explicado em
  docs/re/QUADRO-HUD-TEMPO.md. Execução real no ELF preparado: 75 funções,
  90 ligações a chamadores, 254 chamadas indiretas, zero falhas de decode,
  limite não atingido. Seis conjuntos de leituras estáticas e dois encaixes
  condicionais de slots virtuais conferidos. Relatório build/frame-flow-map.json.
- Laço 0x345140 chama slot virtual +0x60; tabela instalada pela aplicação
  aponta a 0x6e5940, que chama 0x345170. 0x344980 é encerramento, não update.
- Tempo: construtor inicializa app+0xe8=1; motor escolhe cálculo fixo ou
  baseado em taxa medida. app+0xf0 recebe segundos e tem consumidores reais
  no quadro Kyoei. Tabelas taxa=[60,-1,60,30,20], flip=[0,0,0,1,2]. Hipótese
  de câmera lenta compatível com passo fixo; modo ativo na cidade NÃO provado.
  Nenhum timestep, seletor, física ou relógio alterado.
- Mapeados envio/flip, SubmitDone, espera por label/evento (0x437920 →
  0x44e540 → WaitEqueue) e polling adicional. Ligação ao runtime Windows
  documentada sem remover fences/publicação/liberação de superfícies.
- HUD: confirmados chamadores 0xa1bc00 e 0xa6cee0 do marcador 0xa6d9a0;
  caminho separado de layout 0x2e0cf0 → medidor 0x2e2b10. Placas por contagem
  e medição/quebra de linha exigem análises distintas. Dispatcher até cada
  objeto de HUD ainda não resolvido; não inferir ligação direta inexistente.
- Doze testes sintéticos do novo analisador PASS, incluindo corrupção de
  unwind, relocação, hash/valor de fatos e chamada/tabela virtual incompatível.
  Quatro testes de referências ELF PASS. Análise real reproduzida sem execução
  de código convidado. Só metadados técnicos/ferramentas versionados.
- Sem ganho de FPS declarado. Portable, saves, dump, runtime, snapshot fixo e
  Godzilla intocados nesta etapa. CONTINUAR-CODEX atualizado para esta prioridade.

### 15/10/2026 — índice integral e aprofundamento de tempo/tarefas/HUD

- Davy pediu continuar a engenharia reversa de tudo que precisarmos; reforçou
  interesse em otimizar o jogo ao entender seu funcionamento. Relato sobre PS4
  tratado como contexto, sem benchmark ou causa de lentidão presumida. Mantido
  backend: nenhum jogo aberto/executado nesta etapa.
- Novo indexador tools/index_guest_code.py: banco privado em
  build/research/guest-code.sqlite com 42.194 intervalos e 2.929.970 instruções.
  Comprimentos cobrem cada função contiguamente; zero falhas/redecodes individuais.
  SQLite integrity_check=ok. Cobertura de instruções não é compreensão completa.
- Novo rastreador tools/trace_guest_fields.py: origem de ponteiros por CFG,
  interseção de estados, ABI SysV, subregistradores, clobbers implícitos e prefixos.
  Relatório final: 1.692 funções selecionadas, 1.334 acessos/endereços de campos,
  2.196 chamadas com argumentos conhecidos, 19 slots; 361 saltos indiretos ainda
  não resolvidos. Spills/heap aliases e efeitos interprocedurais não completos.
- CORREÇÃO da hipótese anterior de câmera lenta: app+0xe8 inicia em 1 no
  construtor, mas a classe Kyoei escreve 0 em 0x6e08a6, estágio44 de45 da
  inicialização 0x6dfda0. Slot+38 da aplicação, jump table e receptor conferidos.
  Inicializador base 0x3440c0 usa RSI para self, RDI para retorno por estrutura.
  Não usar o padrão do construtor como prova de modo fixo no gameplay.
- Binding setDeltaTimeFactor identificado na tabela técnica: 0x603ff0 escreve
  app+0xec em 0x604026. Há restaurações para1.0. Pesquisa nos299 chunks Lua M1
  não encontrou o nome; não implica ausência em todos os recursos/módulos.
- Tempo se propaga por grupo/entidade em0x354be0: app+0xf0 × fator de grupo,
  fator da entidade e escala adicional condicionada. Entidade+14 recebe segundos,
  +18 recebe unidades de quadro.126 leituras estáticas de app+f0 em92 funções.
- Gestor global0x1336200: construtor0x353220/tabela0x125a2f8; fases30/38/40/48/50
  ligadas ao quadro. Fila de tarefas usa registros de48 bytes. Executor0x345970
  resolve métodos ímpares por slot=(identificador-1); casos71/79/81/91 mapeados.
  Wrappers não sinalizam trabalhadores incondicionalmente em cada inserção.
- UnitHudCockpit: registro0xa53330, construção0xa535d0, tabela0x12d7bb8.
  Atualização78→0xa53090; porta80→0x2efed0→slotd8→0x2f0160. Filhos habilitados
  recebem tempo do pai antes de update. Estado/flags preservados; nenhuma HUD
  alterada. Consumidor0x1f6020 identificado como GxUnitRealTimeDemo, não física;
  contém subdivisão de passos, impedindo supor uma etapa por apresentação.
- Documentação nova TEMPO-TAREFAS-ENTIDADES.md e correção de QUADRO-HUD-TEMPO.md.
  Receita system-map-facts:138 funções/116 ligações,9 slots condicionais e7
  leituras, zero falhas/limite. Fatos de campo emfield-trace-facts.json.
- 32 testes PASS (3 índice,13 proveniência,12 mapa,4 referências ELF).
  Provas reais de seletor, propagação e slots PASS; derivados privados ignorados
  pelo Git. Runtime/portable, dump, saves, snapshot e Godzilla intocados.
- Próximo: personagem/física, módulos de fase e custo real das filas em cena.
  Nenhum ganho de FPS, sincronização corrigida ou engenharia reversa total
  concluída declarado. CONTINUAR-CODEX atualizado para a continuação.


### 15/10/2026 — continuação: personagem e física, sem executar o jogo

- Nova ferramenta tools/inspect_guest_types.py: recupera candidatos de registros
  de reflexão via constantes do índice local, invalidando branches, instruções
  desconhecidas, escritas parciais e sobreposições na pilha. Hash do ELF exigido.
- Resultado conservador: 624 funções, 1.039 registros, 59 tipos selecionados
  (exploração inicial permissiva: 62). Não equivale a classes ativas em cena.
- Conferidos UnitCharacter, UnitPlayer e hierarquia registrada. UnitPlayerBase
  possui callback nulo e não é o pai registrado de UnitPlayer.
- Atualização do jogador 0x9fc1a0/slot78 chega a 0x719a90 condicionalmente ao
  bit0 de self+69a. Base passa segundos/+14 e unidades de quadro/+18 ao objeto
  self+6a0 por slots b8/78. Identidade desse objeto ainda pendente.
- UnitPhysicsDynamicsGround instala tabela final 0x12ef290; slot78 chega a
  0x35b7f0 via thunk. Solver e passo físico ainda não identificados.
- Mapa e limites em docs/re/PERSONAGEM-FISICA.md. Seis testes sintéticos PASS.
  Relatório privado ignorado. Nenhum jogo aberto, mudança de runtime, atualização
  do portable ou ganho de FPS declarado. Próximo: objeto+6a0 e mundo físico.
- Limite explícito: UnitPlayer/UnitPlayerBase foram omitidos pelo extrator final
  conservador; seus registros da exploração inicial foram conferidos à parte.


### 15/10/2026 — teste de desempenho do Davy em andamento

- Davy abriu Medir desempenho atual EN.cmd: relatou muito stutter inicial,
  depois melhora e estabilidade percebida; ainda sem comparação controlada.
- Processo confirmado: build/kyoei.exe, PID29924, início22:59:18. Registro
  native-05/logs/20261015_225918.log ainda com0 bytes na leitura durante a sessão;
  sem números de FPS/tempos disponíveis. Cache Vulkan presente (~15,5 MB),
  o que não prova a causa das travadas. Jogo não interrompido pelo agente.
- Aguardar fechamento normal para analisar o registro; compilação de shaders
  ou carregamento são hipóteses, não diagnóstico nem ganho comprovado.


### Teste real do começo — 15/10/2026, 22:59

Davy testou o começo com ação e relatou stutter inicial seguido de melhora.
Fechamento normal, exit0; log privado native-05/logs/20261015_225918.log
(543.291 bytes). Análise em build/research/performance-20261015-225918.json.

- 124 janelas de120 intervalos:14.880 intervalos,373,502 segundos medidos.
- Média global39,84 FPS; médias das janelas17,35–60,05 FPS. Não são FPS mínimo
  e máximo instantâneos. Menus/cenas/carregamentos sem marcação se misturam.
- Maior intervalo584,04 ms;1.107 intervalos acima de50 ms,35 acima de100 ms.
- command-batch:168 janelas de120 chamadas, média18,35 ms/chamada, máximo
  519,30 ms. Mede o escopo CPU que processa lotes; não é tempo puro de GPU
  nem pode ser somado aos outros escopos sobrepostos. Principal alvo a detalhar.
- Shaders:600 chamadas resumidas, máximo13,02 ms; pipelines gráficos:600,
  máximo16,03 ms. Nenhum compile-stall registrado. Isso não prova ausência
  de custos em amostras incompletas, mas não sustenta culpar shaders pelas
  maiores travadas. TLS0; nenhuma linha de falha detectada pelo analisador.
- Pacing médio16,34 ms/chamada. Não equivale a FPS do jogo.

Serve como referência inicial reproduzível do começo; sem comparação da mesma
sequência na versão anterior, não quantifica ganho causado pelas mudanças.
Próximo: detalhar o lote de comandos e correlacionar pausas sem alterar o jogo.


### 15/10/2026 — detalhamento das pausas de comandos

- Integração CMake preserva snapshot: mede callbacks de comandos, espera
  WaitVoLabel, Draw/DrawIndirect, DispatchDirect/Indirect e CpSync.
  Conferidos1 callback,1 label wait,16 draws,9 dispatches e1 CpSync na cópia
  gerada. Escopos encerram antes de qualquer co_yield; não cobrem a vida
  inteira de uma corrotina. Medem duração de chamada CPU, incluindo esperas
  e subchamadas; não são tempos GPU e não podem ser somados entre si.
- Novas categorias14–18 no frame_stats. Pausas>=50ms nas categorias12–18
  geram até8 registros por categoria/thread, com término no relógio monotônico
  para conferir sobreposição. Registro esparso cobre janelas incompletas;
  limite não é contagem total de travadas. Analisador preserva atribuição
  diagnostic e timestamps, sem inferir causalidade automaticamente.
- Compilação kyoei/test_kyoei_frame_stats PASS; teste C++ ativo/desligado,
  limite8 de10 pausas e analisador Python PASS. Launcher EN PerformanceStats
  CheckOnly PASS. Nenhum jogo iniciado. Warning u8path preexistente.
- build/kyoei.exe atualizado; mesmo Medir desempenho atual EN.cmd. Portable
  intacto. Próximo teste: repetir começo para distinguir espera de apresentação
  e chamadas de renderização. Ainda sem correção de performance nova: medir
  antes de escolher alteração de sincronização.


### 15/10/2026 — segundo teste e correção do custo de diagnóstico

- Davy testou novamente; log privado20261015_231142.log,5.728.756 bytes,
  exit0.46 janelas/5.520 intervalos/188,749s,29,25 FPS globais mistos,
  intervalo máximo551,09ms;40 intervalos>100ms. Não comparar diretamente
  aos39,84 FPS da sessão anterior: sequência/duração diferentes e coleta mudou.
- video-label-wait: máximo30,57ms; scheduler-wait241,76ms, readback-finish
  241,97ms, linear-downloads241,98ms; prepare292,74ms. Essas durações são
  inclusivas, não somar. A espera de liberação da imagem não explica sozinha
  as maiores pausas; leitura sincronizada de dados GPU é alvo concreto, sem
  prova de que explique todas as travadas. command-batch máximo491,50ms.
- Falha na escolha de frequência do diagnóstico:54.419 janelas raster-draw
  (6.530.280 chamadas medidas) e570 dispatch. Mais de54mil linhas geradas;
  timers/sort/log podem perturbar desempenho. Sessão não serve como A/B limpo.
- Corrigido: raster-draw/dispatch coletam1 em128 chamadas, antes de ler relógio.
  Agregados mantêm120 amostras e indicam sample_stride=128; analisador preserva
  metadado e explica que máximos/pausas cobrem só as chamadas selecionadas.
  Demais categorias permanecem integrais. Sem alegar ganho medido no jogo.
- Build kyoei e teste C++ PASS, modo desligado PASS, teste Python PASS.
  Teste confirma1 registro por15.360 chamadas amostradas, limite de pausas
  preservado. Nenhuma partida aberta pelo agente. Portable intacto.
- Próximo: investigar a sincronização de readback; não ativar batching como
  solução automática, pois logs conhecidos trazem lotes de1 imagem.


### 15/10/2026 — readback: investigação e evidência esparsa

- Cadeia conferida: FindTexture(Storage)/FindRenderTarget marcam imagens lineares
  modificadas; ProcessDownloadImages grava image->buffer, Finish submete a fila
  e aguarda o tick antes de publicar em memória guest. Não remover a espera
  sem resolver dependências: o jogo pode consumir esses resultados.
- Lotes registrados no segundo teste:1 imagem/1 espera/256bytes. Não basta
  habilitar batching; tamanho de transferência sozinho não explica fila longa.
  Não foi estabelecido que o máximo de242ms pertença ao mesmo lote256bytes.
- Medição nova preserva essa correlação: até8 esperas>=50ms em readback-finish
  e scheduler-wait por thread/categoria. Readback síncrono inclui bytes,largura,
  altura e término monotônico. Assim compara fila/espera/cópia sem per-frame spam.
- Build/teste C++ ativo/desligado e analisador Python PASS, incluindo registro
  de256bytes/8x8 sintético. Nenhum jogo iniciado, fence alterado ou ganho prometido.


### 15/10/2026 — atualização pequena para a distribuição Windows

- Davy pediu preparar o repasse das atualizações sem novo download20GB.
  Criados tools/build_runtime_update.py e scripts/apply_runtime_update.ps1.
  ZIP privado build/Kyoei-Windows-Update-20261015.zip:7.310.399bytes; SHA256
  5cc4b5d025b4e0846325e1371d6fbee8609c62e08d6ff47b525b8d6bdd3586b7.
- Contém5 arquivos:runtime próprio limpo,aplicador,CMD,hashes,leia-me. Não
  contém jogo/dump/PS4 preparado,traduções,saves,cache ou logs. Só Windows x64.
  Runtime novo53bf81d723e2af7675094e763e89cda3eb5c59f42fce4d6ca3febdb63d93d33b;
  base aceitae577a569efc574c1c5d23fce5e5feae907196a999663772348f1c6ca0b2b8a2e.
- Extrair na raiz do jogo,abrir Atualizacao-Kyoei/Aplicar atualizacao.cmd com
  jogo e launcher fechados. Valida título/versão/hash do payload e da base,
  trava sessão,copia/verifica e usa File.Replace com backup do runtime em
  data/update-backups. Reaplicação é idempotente. Manifest base preservado.
- Três testes sintéticos PASS:aplicação/preflight/backup/saves/reaplicação,
  runtime incompatível e payload corrompido. ZIP real CheckOnly contra pasta
  Friends-Test PASS; hash base igual manifest;tokens pessoais conhecidos sem
  ocorrências no PE. Pastas Friends-Test e Portable não alteradas.
- Instruções/limites em docs/ATUALIZACOES-AMIGOS.md. Não enviado aos amigos
  pelo agente. Linux precisa build/pacote separado; não alegar atualização
  Linux nem ganhoFPS. Pacote Windows contém o runtime atual acumulado.


### 15/10/2026 — atualização Linux da versão distribuída via Wine/Proton

- Correção da orientação anterior: Friends-Test contém Start on Linux.sh que
  inicia o launcher Windows via Wine/umu-run. Portanto a atualização desse
  pacote usa o mesmo runtime PE, sem exigir build Linux nativo. O ELF nativo
  experimental é outra frente e não foi substituído nem distribuído aqui.
- Criados tools/apply_runtime_update_linux.py, teste próprio e opção
  --linux-wine em build_runtime_update.py. ZIP privado pronto:
  build/Kyoei-Linux-Wine-Update-20261015.zip,7.311.112bytes; SHA256
  3166ed4e66694032ee3ecbf1bf95ba0a163df1e102da337953882320f161715a.
- Mesmo payload/base do update Windows. Cinco arquivos,sem jogo,saves,cache,
  prefixo Wine ou dados originais. Aplicação: extrair na raiz e executar
  sh Atualizacao-Kyoei/Aplicar-no-Linux.sh. Requer Python>=3.11,sem sudo.
- Confere hash/título/versão,recusa executáveis mapeados em/proc,serializa
  aplicadores Linux via flock,verifica/copia backup e publica por rename
  atômico. Usuário mantém jogo/launcher fechados: flock não substitui os
  bloqueios Windows do Wine. Saves e prefixo preservados.
- Quatro testes Linux Ubuntu/WSL PASS:preflight/substituição/backup/save/
  permissões/reaplicação,payload corrompido,base errada,executável mapeado.
  Testes corretamente ignorados em Windows. ZIP real CheckOnly contra
  Friends-Test PASS,sem alterar distribuição original. Instruções corrigidas
  em docs/ATUALIZACOES-AMIGOS.md. Nenhum jogo iniciado ou update enviado
  externamente. Compatibilidade de gameplay/FPS ainda depende de teste.


### 15/10/2026 — base da investigação: contadores da fila GPU

- Davy pediu começar pelo começo. Sem nova sessão após231142; não é possível
  atribuir a pausa242ms a um recurso específico nos registros antigos.
- Conferidos Scheduler::Finish/Wait e MasterSemaphore::Wait: Finish submete
  antes de aguardar; Wait só faz flush adicional se tick ainda não enviado;
  MasterSemaphore retorna cedo quando tick conhecido/com refresh já terminou.
  Nenhuma espera duplicada removível demonstrada. Snapshot preservado.
- Integração Kyoei passa a registrar requested/next lógico/known_before/
  known_after/forced_flush em Scheduler::Wait>=50ms, até8 por thread. Usa
  contadores já disponíveis, sem consulta Vulkan adicional. Tempos e diagnostic
  correlacionam com readback. Contadores known são cache, não timestamps GPU,
  e diferença de ticks não representa quantidade de desenhos nem tempoGPU.
- Build kyoei/teste C++ PASS; teste limiar49ms e limite8/10 PASS; analisador
  Python PASS; launcher PerformanceStats CheckOnly PASS. Jogo não iniciado.
  Sem nova otimização, cache apagado ou promessaFPS. Updates ZIP anteriores
  permanecem identificados e não foram regenerados para esta coleta local.
- Próximo: Davy repetir começo em Medir desempenho atual EN.cmd para capturar
  readback bytes/dimensões junto dos contadores. Só então escolher correção.


### 15/10/2026 — teste233924 e primeira redução de consultas por desenho

- Log privado20261015_233924.log,424.152bytes,exit0.41 janelas/4.920 intervalos,
  149,489s,32,91 FPS globais mistos,máximo567,34ms.753 intervalos>50ms,
  34>100ms. Médias não são comparação controlada com sessões anteriores.
- Nenhuma esperaScheduler/readback>=50ms nesta sessão; janelas completas:
  scheduler-wait máximo27,16ms,readback-finish27,31ms,linear27,33ms.
  video-label máximo23,26ms. A pausa242ms anterior não se reproduziu;
  não explica sozinha as travadas recorrentes. Sem erro detectado no analisador.
- Sampler128 ativo:354 janelas raster-draw,contra54.419 na coleta antiga;
  ainda indica milhões de chamadas no trecho. Não extrapolar duração exata
  totalGPU a partir das amostras. command-batch máximo440,02ms.
- Achado concreto no código: Draw/Dispatch chamam PopPendingOperations; antes
  havia Refresh/getSemaphoreCounterValue mesmo com fila pending_ops vazia.
  Cópia integrada Kyoei agora retorna cedo quando a fila está vazia.
  Fila com callbacks mantém Refresh/FIFO/IsFree/publicação originais.
- Conferidos ResourcePool::CommitResource (Refresh quando busca falha),
  Scheduler::IsFree (Refresh quando necessário), SubmitExecution (Refresh
  após envio antes de alocar commandbuffers), MasterSemaphore::Wait (Refresh
  antes de bloquear). Não removidas essas atualizações nem fences/readbacks.
- Chave KYOEI_POLL_EMPTY_OPS=1 restaura polling vazio anterior no mesmoexe
  para comparação. Padrão ativa retorno cedo. Snapshot/Godzilla intocados.
- Build PASS; testes existentes readback-batch/GPU-wait/frame-stats e
  analisador PASS; launcher CheckOnly PASS. Esses testes não substituem
  gameplay ou provam ganhoFPS. Novo build local pronto para comparar começo.
  Não iniciado jogo nem regenerados ZIPs dos amigos antes da validação.


### 15/10/2026 — teste234620 do retorno cedo com pending_ops vazio

- Log privado20261015_234620.log,392.698bytes,exit0;52 janelas/6.240 intervalos,
  198,611s,31,42FPS globais,máximo666,10ms;855 intervalos>50ms,36>100ms.
  Sessão anterior149,489s/32,91FPS:não comparar médias como regressão/ganho.
- Blocos por ordem de1200 intervalos:56,25/52,02/25,03/24,65/23,92FPS e
  último parcial32,35. Antes:56,43/52,14/23,91/23,35 e parcial33,04.
  Pequena diferença nos blocos lentos é indício,sem alinhamento de cena não
  é ganho comprovado. Nenhum erro detectado; não implica validação visual.
- Correlação nova concreta: imagem1x1,pitch/download256bytes; leitura91,41ms
  termina40348010,28ms,esperaScheduler90,69ms termina40348009,72ms,
  requested9339/next9340/known_before9332/known_after9339,forced_flush0.
- Segunda imagem1x1/256bytes:readback132,83ms termina40362401,54ms,
  espera132,29ms termina40362401,17ms,requested10011/next10012/
  known_before10010/known_after10011,forced_flush0. Escopos inclusivos.
  O tick exigido já estava submetido; não houve flush extra nesse caminho.
  Não é largura de banda da cópia por si só; investigar trabalho anterior,
  agendamento e ponto de consumo. Contador antes é cache, não fila GPU viva.
- video-label máximo22,68ms; shaders máximo8,56ms nos600 registros resumidos;
  command-batch máximo665,27ms. Essas leituras não explicam todas as travadas.
- Otimização de fila vazia permanece candidata local; sem aumentoFPS validado
  nem update novo para amigos. Próximo backend: identificar produtor/consumidor
  da imagem1x1 antes de considerar leitura adiada. Não remover fences.


### 15/10/2026 — investigação dirigida: luminância e exposição automática

- Davy autorizou seguir trabalhando; quando pedir interrupção, salvar progresso
  e preparar novos updates dos amigos. Nesta etapa não gerar ZIPs novamente
  nem iniciar jogo; pacotes anteriores continuam identificados.
- Rótulos técnicos CTextureLuminance/CTextureLuminanceManage e
  GetResult_UpdateInternalLuminance ligaram funções a um candidato de feedback
  gráfico. ApplyEffects_UpdateAutoExposure0x67430 chama gestor0xd1ec0 em678a9;
  InitializeAutoExposure0x4e4e0 chama configuração0xd2ad0 em4e681.
- Gestor usa verificação0xd04d0 e wrappersd0e50/d0e90 com ESI0 no caminho
  analisado; wrappers atualizam viad08e0 e retornam float+74 ou vetor+64.
  Histórico de resultados no gestor. ESI0 ainda sem contrato de bloqueio
  confirmado: não presumir que possa adiar a publicação com segurança.
- Backend0xcf400 resolve slot70; getter4f670 retornaobj+40. Restam
  implementação concreta/flag100000 e ligação do recurso ao runtime observado.
  A imagem1x1 dos registros NÃO foi identificada como luminância ainda.
- docs/re/LUMINANCIA-READBACK.md e luminance-flow-facts.json. Receita executada:
  37 funções,75 ligações de chamadas,78 chamadas indiretas,zero falhas/limites.
  Relatórios derivados privados em build/research,sem dados originais no Git.
- Coleta adicionada: até16 combinações únicas endereço/formato/papel de imagem
  1x1 por thread,roles1storage/2targetlookup/3download. Só PerformanceStats;
  repetições não logam. Readback lento inclui endereço para correlação.
  Endereço pode ser reutilizado,lookup não prova shader executado.
- Build kyoei/teste C++ PASS; teste1000 repetições gera1 linha,roles separados,
  teto16 PASS; modo desligado PASS; analisador formatos antigo/novo PASS;
  12 testes do mapa PASS. Snapshot/dump/saves/iluminação/fences intocados.
- Próximo: recurso/endereço/formato contra backend/query real; só depois
  considerar async. GanhoFPS ainda sem prova; polling vazio segue candidato
  local. Não pedir teste idêntico sem usar a nova hipótese.


### 16/10/2026 — updates cumulativos prontos para os amigos

- Davy adiou gameplay para amanhã e pediu novos updates se houve progresso.
  Incluído runtime atual:otimização de callbacks vazios e coleta limitada
  de1x1/luminância. Não apresentar como aumento de FPS comprovado.
- build_runtime_update.py aceita --accept-update repetível,conferindo
  título/versão/plataforma/schema e hash do payload ZIP anterior.
  Três testes do gerador PASS (válido,versão errada,payload corrompido).
- Novos pacotes cumulativos,base original+update15/10 aceitos:
  Windows build/Kyoei-Windows-Update-20261016.zip:7.311.654bytes,SHA256
  df8cbd08c599332b2b1c87a7d0e3ab9ce91e151b332cc169832123f5acc59f7d.
  Linux/Wine build/Kyoei-Linux-Wine-Update-20261016.zip:7.312.368bytes,SHA256
  faee99ef256fc9b35f943b25c31fdd650dcb1280daa23cd96c1744103f7d9355.
  Payload compartilhado509c01e74e7613ddc17033aa922cd3448b1c05329133f430e85e612d3a9dc7c6.
- Testes aplicador Windows3/Linux4 e frame_stats ativo/desligado PASS.
  Verificação adicional dos ZIPs reais em4 instalações temporárias:
  Windows/Linux × base original/update anterior; aplicação,reaplicação,
  backup exato,save e prefixo Wine preservados PASS. Sem arquivos do jogo
  nem identificadores pessoais conhecidos no payload.
- Relatório privado build/research/update-validation-20261016.json.
  docs/ATUALIZACOES-AMIGOS.md atualizado. Extrair sobre pasta do jogo e
  substituir arquivos de Atualizacao-Kyoei. Launchers/saves/cache intactos.
  Nenhum jogo iniciado,distribuição local original alterada ou mensagem
  externa enviada. Teste gameplay do recurso1x1 fica para amanhã.


### 16/10/2026 — comparação histórica de FPS solicitada por Davy

- Reanalisados23 logs com frame stats (12/13/15),mais diário e observação
  de gameplay. Inventário privado build/research/fps-history-20261016.json.
  Incluído log235349:41 janelas,136,07s,36,16FPS mistos,máximo567,26ms.
- Maior avanço técnico demonstrado:12/10,TLS direto;19–33mil exceções por
  apresentação antes,zero depois;fixture com leituras32ms→0,02–0,10ms.
  Não é fator de ganhoFPS. Relato humano de melhora registrado no própriodia.
- Trechos de ação observados12/10:22,74/25,41FPS; sessões atuais ainda têm
  blocos lentos23–25. Sem rota A/B alinhada,não afirmar saltoFPS significativo
  nem percentual/dia vencedor em gameplay. Médias13/15 contêm menus/carga.
- docs/COMPARACAO-DESEMPENHO-20261016.md contém tabela/limites. Sem novo jogo
  aberto,código alterado ou teste solicitado. Updates16/10 preservados.

### 16/10/2026 — análise do teste 200526 de Davy

- Sessão mais recente confirmada: log privado20261016_200526.log,340.160bytes.
  Relatório build/research/performance-20261016-200526.json;49 janelas,
  5.880 intervalos,145,724s resumidos,40,35FPS mistos; janela mais lenta17,82FPS.
  Média inclui menus/carga e não comprova ganho contra sessões anteriores.
- 485 intervalos>50ms,31>100ms; maior700,21ms. Sem falhas detectadas pelo
  analisador. TLS handled0. Validação visual continua sendo a observação humana.
- Nova coleta funcionou: endereço0x105beeed00 aparece em storage(role1) e
  download(role3),imagem1x1,pitch64,formato37. Isso relaciona os dois papéis
  no registro,mas não identifica luminância,shader produtor ou consumidor.
- Nenhum readback/esperaGPU>=50ms registrado. Janelas completas:readback-finish
  máximo18,09ms,linear18,11ms,scheduler-wait18,83ms. A espera longa antiga
  não se reproduziu nesta sessão e não explica sozinha as travadas recorrentes.
- command-batch máximo492,47ms;shader-compile8,51ms;graphics-pipeline4,58ms;
  video-label18,78ms. Escopos não são timestampsGPU; não somar escopos inclusivos
  nem atribuir automaticamente a pausa700ms ao batch492ms.
- Próximo: decompor processamento de command-batch e ligar recurso observado
  ao produtor/consumidor real. Nenhuma alteração de runtime ou novo jogo aberto.
  Registro200147 anterior sem frame stats analisado separadamente,sem confundir
  com a sessão de medição. Relatórios privados preservados;updates intactos.

### 16/10/2026 — decomposição dos lotes gráficos após teste200526

- Davy pediu continuar trabalhando. Revisão de Liverpool::Process demonstrou
  que command-batch mede um laço que drena fila dinâmica, não um único envio.
  O máximo492,47ms anterior não identifica sozinho uma chamada lenta.
- Nova coleta Kyoei em src/kyoei_command_batch.h,integrada por CMake somente
  na cópia gerada: duração total,soma/maior retomada externa,contagens de
  retomadas,tarefas concluídas,retornos sem conclusão e visitas a filas vazias.
  end_ms/diagnostic permitem correlação. Nenhum timer atravessa co_yield.
- PerformanceStats apenas;primeiros8 lotes>=50ms por thread. Após teto,
  encerra contagem/relógios detalhados. Durante coleta há duas leituras de
  relógio por retomada;possível custo em fila com muitas retomadas documentado.
  yielded não prova ociosidade;tempos incluem esperas/callbacks internos;
  diferença total/resumes não identifica automaticamente locks ou contenção.
- Analisador aceita novo registro e logs antigos. docs/re/COMMAND-BATCH.md
  explica contrato,limites e ausência de timestampsGPU. Snapshot/Godzilla
  preservados;ordem/condições de sincronização inalteradas.
- Build kyoei e teste final PASS. Teste C++:tarefas executadas com coleta
  ativa/desligada/teto esgotado,retorno sem conclusão,contagem,limiar49ms,
  teto8 e atribuição de diagnóstico PASS. Analisador Python PASS.
  Launcher PerformanceStats CheckOnly via PowerShell -ExecutionPolicy Bypass
  PASS;invocação direta anterior bloqueada pela política local de scripts.
  git diff --check PASS. Nenhum jogo iniciado nem ZIP regenerado.
- Build local pronto para coleta futura;sem ganhoFPS demonstrado nesta etapa.
  Próximo:usar decomposição para distinguir retomada longa de fila com muitos
  envios/retornos. Registro200526 não contém os novos campos retroativamente.

### 16/10/2026 — teste201734: retomadas longas e pipelines

- Davy executou nova medição. Log privado20261016_201734.log,640.983bytes,
  exit0;77 janelas/9.240 intervalos,306,680s resumidos,30,13FPS mistos.
  1.195 intervalos>50ms,61>100ms,máximo2.917,03ms. Sessão mais longa e
  percurso não alinhado:sem conclusão de regressão/ganho contra40,35FPS.
  Relatório privado build/research/performance-20261016-201734.json.
- Coleta detalhada funcionou:8 lotes,8–12 retomadas cada;completed=resumes,
  yielded0,empty0 em todos. Soma das retomadas difere do total no máximo0,03ms
  nos valores arredondados. Exemplos:230,37ms,total/resumes230,37ms,
  maior retomada230,33ms;306,68ms,resumes306,67ms,maior267,67ms.
  Evidência aponta execução dentro de retomadas,não giro entre filas nestes
  oito lotes. Não generalizar para sessão inteira:coleta esgotou cedo.
- graphics-pipeline máximo429,21ms em840 chamadas resumidas;registros esparsos
  97,64/220,51/176,46ms,entre outros. Escopo inclui criação de GraphicsPipeline
  e RegisterPipelineData/trabalho associado;isolá-los antes de culpar somente
  driver Vulkan. Não há end_ms nesses registros para correlação temporal exata.
  Shader-compile máximo11,86ms;video-label19,06ms;queue-lock0,93ms.
- Retornou espera de imagem1x1/256bytes no endereço0x105beeed00:114,89ms,
  esperaGPU114,17ms,requested18336,next18337,known_before18331,after18336,
  forced_flush0. Recurso storage/download confirmado no teste anterior,mas
  associação à luminância ainda não provada. Escopos inclusivos,não somar.
- command-batch máximo2.931,23ms em resumo posterior ao teto da coleta;
  não há decomposição desta pausa. Não atribuí-la aos pipelines ou readback
  apenas pela proximidade no arquivo. Capturas do cache de driver7,49–15,83ms,
  writer completed12/failed0/latest_saved1. Sem falhas detectadas no analisador.
- Próximo foco:separar criação Vulkan/registro persistente no caminho de novos
  pipelines e conferir cache/warmup existente antes de mudar sincronização.
  Nenhum runtime alterado,jogo iniciado ou novo teste solicitado nesta análise.

### 16/10/2026 — separar driver, construtor e registro de pipelines

- Davy autorizou a próxima etapa. Caminho de GetGraphicsPipeline agora mede
  graphics-construct(19) e graphics-register(20),mantendo escopo externo7.
  Unidade vk_graphics_pipeline gerada localmente mede graphics-driver(21)
  somente em createGraphicsPipelineUnique;preloading excluído da categoria21.
  Objeto explícito substitui unidade do arquivo estático;link final PASS.
  Snapshot/Godzilla intactos.
- Novas categorias usam resumos120 chamadas e até8 compile stalls>=16,67ms
  por categoria/thread. Registros compile stall agora incluem end_ms monotônico
  para correlação;analisador mantém compatibilidade com linhas antigas sem ele.
  Driver está dentro do construtor,dentro do escopo externo:não somar tempos.
- Achado relevante no log201734:aviso de perfil incompatível na inicialização.
  WarmUp fecha a base e retorna sem pré-carregar pipelines;registro posterior
  retorna cedo enquanto fechada. Cache Vulkan de driver é separado e continuou
  atualizado. Não atribuir pausas só à gravação de arquivos:WriteVector já
  enfileira a escrita na thread PipelineCacheIO.
- ProfileMatches mantém comparação exata por tamanho/bytes. Com PerformanceStats
  ligado,uma rejeição registra tamanhos e até8 offsets/valores diferentes.
  Não aceitar cache incompatível nem apagar perfil antes de explicar diferença.
  Ainda sem prova se causa é capacidade alterada,formato ou representação.
- docs/re/PIPELINE-STAGES.md documenta escopos,limites e comportamento da base.
  Build runtime/testes PASS;C++ frame stats novas3 categorias,diagnóstico,
  timestamps,limite8,modo desligado PASS. Cache:perfil truncado,compatível,
  diferenças limitadas8 e ausência de logs desligado PASS;testes existentes
  de persistência/fila PASS. Analisador Python antigo/novo PASS.
  Launcher CheckOnly PerformanceStats PASS;git diff --check PASS.
- Nenhum gameplay iniciado,nem cache/saves reais alterados,nem ZIP regenerado.
  Build local pronto para investigar rejeição e etapas em medição futura.
  Sem correção de FPS ou validação visual alegada nesta etapa.

### 16/10/2026 — teste211419 e correção da rejeição por padding do perfil

- Davy corrigiu o agente:houve novo teste antes da resposta sobre desempenho,
  e o agente respondeu sem consultar o log mais recente. Analisada agora a
  sessão correta20261016_211419.log,407.032bytes,exit0. Relatório privado
  build/research/performance-20261016-211419.json;95 janelas/11.400 intervalos,
  248,256s resumidos,45,92FPS mistos;539 intervalos>50ms,34>100ms,máximo549,43ms.
  Sem comparação controlada de percurso,não comprova melhora de FPS.
- Separação funcionou:600 chamadas resumidas por categoria,pipeline externo
  máximo3,69ms,construtor3,69ms,driver3,44ms,registro0,03ms. Nenhum compile
  stall de pipelines;shader-compile18,30/23,60ms. Readback sem stall>=50ms.
  command-batch máximo496,33ms. Logo pipelines lentos da sessão anterior não
  se reproduziram,não atribuir a eles as travadas atuais.
- Causa concreta da rejeição do cache identificada:perfil72bytes em ambos;
  diferenças somente offsets66–71 (cached22/0/61/24/141/118 versus
  current158/85/103/10/6/158). Os primeiros66bytes coincidem. No tipo real
  Shader::Profile,os campos terminam em supports_shader_stencil_export no
  offset65;66–71 são padding final de alinhamento. memcmp bruto rejeitava
  capacidades iguais por bytes sem significado.
- Correção Kyoei:ShaderProfileMatches exige tamanho72 e compara todos os
  66bytes significativos;ignora somente padding final. Guardas de compilação
  verificam ABI da cópia fixa. Mantém formato salvo,checagens dos campos e
  diagnóstico de rejeições reais. Snapshot e cache existente preservados.
  A integração WarmUp usa a nova comparação;sem apagar cache ou relaxar campos.
- Teste com Shader::Profile real:padding diferente aceito;mutação individual
  de cada um dos66bytes significativos rejeitada;perfil71/73bytes rejeitado.
  Testes prévios de cache,persistência,fila e diagnóstico PASS. Build kyoei,
  frame stats ativo/desligado,analisador antigo/novo,launcher CheckOnly e
  git diff --check PASS. Nenhum gameplay iniciado nesta etapa.
- docs/re/PIPELINE-STAGES.md atualizado. Correção funcional pronta,mas
  pré-carregamento real e impacto nas travadas ainda não validados no jogo.
  O cache Vulkan do driver é separado e já vinha funcionando;esta correção
  não promete eliminar as demais pausas. Nenhum ZIP regenerado.

### 16/10/2026 — pré-compilação real na GPU:693 pipelines

- Davy perguntou pelo atalho EN e autorizou testar compilação prévia de shaders.
  Confirmado:Medir desempenho atual EN.cmd chama build/kyoei.exe via launcher
  com PerformanceStats. WarmUp já acontece na inicialização normal do backend.
- Executado runtime atual em --gpu-test com KYOEI_CACHE_SELF_TEST=1,cache real
  do próprio projeto e mesma GPU. Antes do teste,conferido nenhum kyoei aberto.
  Nenhum código adicional necessário para habilitar o pré-carregamento.
- Resultado real:Preloaded693 pipelines;sem rejeição do perfil,sem aviso de
  stale pipelines;backend Vulkan pronto,exit0. Confirma em execução real que
  a correção dos bytes de padding permite reutilizar a base existente.
- Driver cache19.670.149bytes,captura16,94ms,writer completed1/failed0,
  latest_saved1. Self-test encerrou antes de guest_main,sem partida:
  runtime files opened0/reads0/writes0,maps0. Saves não acessados.
  Tempo0,03ms no log é somente flush do writer,não duração do pré-carregamento.
- Evidência privada build/research/precompile-20261016.log. Única alteração
  externa esperada:atualização normal do blob Vulkan no cache já configurado.
  Dump/saves/base de shaders preservados;nenhum pacote de amigos regenerado.
- Atalho normal continua fazendo WarmUp antes da partida. Isso cobre pipelines
  conhecidos da base,não todos os shaders/cenas do jogo. Redução de travadas/FPS
  ainda depende de observação numa nova partida após esse pré-carregamento.

### 16/10/2026 — teste212733:pré-carregamento validado em partida

- Davy testou após pré-compilação. Analisado o log mais recente confirmado:
  20261016_212733.log,239.364bytes,exit0. Relatório privado
  build/research/performance-20261016-212733.json;34 janelas,4.080 intervalos,
  117,677s resumidos,34,67FPS mistos,máximo533,82ms;478 intervalos>50ms,
  23>100ms. Sem comparação controlada com sessões de duração/percurso diferentes.
- Partida confirmou Preloaded693 pipelines,sem rejeição do perfil. Só uma
  mensagem Compiling graphics pipeline;nenhum compile stall registrado.
  Categorias de compilação não fecharam120 chamadas;ausência de resumo não
  significa que o custo é zero. Detilers internos ainda compilam sob demanda.
- Oito lotes detalhados novamente sem yielded/empty;retomadas concentram tempo.
  Exemplo359,34ms,11 retomadas,maior299,42ms. command-batch máximo504,98ms
  no resumo;prepare261,73ms,present35,06ms,video-label39,21ms.
- Evidência concreta de espera longa recorrente:imagem1x1/256bytes no endereço
  0x105beeed00,readback386,15ms termina37059253,89ms;GPU wait385,55ms termina
  37059253,37ms,requested4753,next4754,known_before4752,known_after4753,
  forced_flush0. Tick já submetido;contador anterior é cache,não timestampGPU.
  Espera incluída no readback:não somar tempos nem culpar largura de banda.
- Sem falhas detectadas pelo analisador. Cache driver writer completed3,
  failed0,latest_saved1. Correção/preload funcionalmente validados na partida,
  mas persistem pausas grandes. Não prometer que shaders explicam as travadas.
- Próximo:investigar trabalho anterior ao tick e produtor/consumidor da imagem,
  além dos detilers internos sob demanda;identidade de luminância segue hipótese.
  Nenhum runtime alterado,jogo iniciado ou teste repetido solicitado nesta análise.

### 16/10/2026 — trabalho direto no backend após cobrança de desempenho

- Davy pediu resolver as travadas, sem depender de novas rodadas manuais.
  O agente executou os testes:usuário/cache/savedata copiados para a pasta
  privada build/research/detiler-fix-20261016. Nenhuma partida original encerrada;
  um fechamento genérico foi rejeitado pela revisão automática. Conferida a
  linha de comando --user isolada e fechado apenas o processo de teste por ID.
- Corrigida compilação tardia de nove detilers internos, que não pertencem ao
  cache de pipelines do jogo. A integração gera tile_manager e tiling_comp.h
  localmente a partir da referência fixa, preparando as variantes observadas
  antes do backend ficar pronto. GPU real:47,10ms;execução de jogo60,41ms.
  As nove listas de defines coincidiram com o log212733,nenhuma compilação
  desses detilers após backend ready. Lazy antigo:KYOEI_LAZY_DETILERS=1.
- Proteção de primeira utilização:marcas por TileManager identificam pipelines
  antecipados ainda não usados. Se a primeira imagem requer amostras/array
  diferentes,descarta somente o pipeline não submetido e compila o solicitado.
  Depois da primeira utilização não descarta pipelines em uso. Teste GPU real
  com quatro amostras PASS,NUM_SAMPLES=4,exit0. Testes de propriedade/slots PASS.
- Investigado envio antecipado em mudanças de passe. Implementado e testado
  isoladamente,mas retirado do código normal por falta de ganho demonstrado.
  Fontes do experimento preservadas só em pesquisa privada. Primeiras medições
  foram invalidadas por ângulos diferentes de câmera;mouse sozinho desligado
  não bastou. Rodadas posteriores isolam controle físico e recentralizam via R3.
- Mailbox29,76FPS/Fifo29,77FPS nos recortes de vista parada;diferenças de foco
  limitam a comparação. Sem justificativa para trocar padrão Fifo.
- Captura de pilhas após a medição encontrou thread gráfica em ordenação dos
  intervalos de BindVertexBuffers. Uma pilha não prova porcentagem de CPU.
  LLDB exigiu Python3.dll no PATH;primeira tentativa falhou antes de anexar.
  A captura posterior registrou detach,mas processo isolado terminou anormal
  depois,exit -1073740972. Não usar essa execução para validar encerramento.
- Caminho rápido de união de intervalos conectados:prova sobreposição/adjacência
  por min/max;se houver vão,não toca na saída e executa sort/merge original.
  Mesmo mapeamento/barreiras/bindings,sem reutilizar dados entre quadros.
  KYOEI_LEGACY_VERTEX_SORT=1 restaura o algoritmo anterior no mesmoexe.
  100.000 casos aleatórios e bordas PASS. Fixture200.000 conjuntos de8 views:
  51,51ms original/36,94ms rápido;não é porcentagem de ganhoFPS.
- Duas partidas isoladas com recentralização+giro programado,controle/mouse
  físicos fora da entrada:recortes35s,10 janelas/1.200 intervalos em cada.
  Original33,52FPS/máximo50,80ms;novo32,87FPS/máximo166,65ms. Ambas exit0,
  imagens conferidas;foco ausente nas duas,câmera e animações ainda não idênticas.
  Não há melhora de FPS comprovada por essas amostras,nem regressão atribuída.
  Execução final do novo código sem depurador encerrou normalmente,sem falhas.
- Build,testes novos,GPU self-test,launcher CheckOnly e git diff --check PASS.
  docs/re/ENVIO-ANTECIPADO.md registra contratos/limites. Referência Godzilla,
  dump e saves originais preservados;ZIPs dos amigos não regenerados.
- Restam pausas grandes sem solução comprovada. Investigar transferência de
  readback para publicação por callback com buffer próprio e espera cooperativa
  da corrotina, mantendo resultado pronto ANTES do SignalFence original.
  Esse desenho ainda não foi implementado neste ponto do diário. Não alegar
  que remover a espera ou retornar resultado antigo seria uma correção válida.

### Checkpoint solicitado por Davy — interrupção para salvar
- Salvo protótipo de readback cooperativo, desligado por padrão; só ativa com
  KYOEI_COOPERATIVE_READBACK=1. Compilação, testes CPU ligado/desligado e
  autoteste na GPU real passaram. Ainda não validado em partida com o caminho
  ligado; não declarar correção das pausas nem ganho de FPS.
- Autoteste GPU: pixel RGBA correto, padding preservado e staging liberado.
  Evidências privadas: build/research/detiler-fix-20261016/gpu-cooperative.log
  e gpu-cooperative.console.log. Nenhuma alteração no dump/saves originais.
- Davy interrompeu a investigação para compilar e preparar as atualizações dos
  amigos; depois pediu salvar imediatamente. Pacotes ainda não regenerados.
- Falha Linux relatada: verificação de processos aborta ao encontrar maps de
  qualquer processo do mesmo usuário sem permissão de leitura em /proc.
  tools/apply_runtime_update_linux.py:require_closed confirma esse caminho.
  Correção e testes do atualizador ainda pendentes; /usr/bin/kitten exit 1
  acompanha o aborto e não foi demonstrado como causa independente.
- Próxima tarefa: corrigir a detecção Linux com verificação segura dos processos
  relevantes, testar e gerar/validar os dois pacotes cumulativos. Manter o
  protótipo experimental desligado na distribuição normal.

### Retorno do teste de Davy — log 20261016_224040
- Relato: travamento no começo; personagem em sprint com baixa qualidade,
  depois melhora visual e aumento percebido de velocidade.
- Log conferido: encerramento normal exit 0; cache do driver salvo sem falhas.
  Pausas de command-batch de 814,84 ms e quadro máximo de 951,05 ms.
  Trechos medidos caem a 14,06–17,91 FPS e depois chegam a 24,91 FPS;
  são cenas diferentes, sem comparação controlada de ganho.
- Há novas compilações de pipelines durante a partida. Isso pode contribuir
  com pausas, mas o log não prova causa da qualidade visual do personagem.
  Readback síncrono ainda registra esperas de 55,67 e 106,23 ms.
- Não atribuir melhora ao protótipo cooperativo: ele segue desligado no atalho.
  Nenhuma mudança de resolução/qualidade foi aplicada nesta conferência.

### 17/10/2026 — atualização dos amigos e correção Linux
- Retomado do checkpoint a pedido de Davy. Corrigido require_closed: maps
  protegido de processo identificado alheio a Wine/Proton não aborta o update.
  Identidade indisponível, Wine ou executável Windows protegido continuam
  bloqueando com PID. Caminhos mapeados comparados integralmente e com escapes
  octais/deleted interpretados, cobrindo espaços no nome do launcher.
- Onze testes Linux/WSL PASS; três testes do gerador e três Windows PASS.
  Build kyoei e testes vertex ranges, primed detilers e cooperative ligado/off
  PASS. Nenhuma mudança ao núcleo Godzilla fixo, dump ou saves originais.
- Gerados ZIPs cumulativos 20261017 Windows (7.327.905 bytes) e Linux/Wine
  (7.329.069). Aceitam original e updates 15/16. Payload limpo SHA256
  f4e2385bff0ec2dd7f58b78ed141f8c2d910e6fbe16e641f1f72e66a0e4b0aae.
  Aplicação/reaplicação real em seis instalações descartáveis PASS, conferidos
  backups, saves, prefixo e payload; arquivos/personal tokens conferidos.
  Relatório build/research/update-validation-20261017.json; hashes dos ZIPs
  registrados em docs/ATUALIZACOES-AMIGOS.md. Nenhum pacote enviado externamente.
- Readback cooperativo segue desligado por padrão. Iniciado teste isolado
  cooperative-20261017 com cópia privada de saves/cache; resultado pendente
  neste checkpoint. Não alegar ganho de FPS nem resolução das travadas.

### 17/10/2026 — teste de readback e preferência de áudio
- Partida isolada com cooperativo ativo completada, imagem conferida e exit 0.
  Comparador síncrono também exit 0. Recortes: 38,52 FPS/50,24 ms maior quadro
  cooperativo (12 janelas); 35,71 FPS/67,45 ms síncrono (10 janelas).
  Foco ausente 10/61 vs 61/61, enquadramento/animação diferentes. Não atribuir
  ganho; manter opt-in. Contratos/limitações em docs/re/ENVIO-ANTECIPADO.md.
- Logs/ranges/medições/imagens cooperative-20261017 e synchronous-20261017
  preservados em build/research/detiler-fix-20261016. Nenhum save original tocado.
- Davy pediu próximos testes mudos porque está vendo filme. Silenciada sessão
  Windows somente do processo de teste 21152, retorno 1 sessão e GetMute=true;
  outros aplicativos preservados. Ferramenta tools/mute_test_audio.ps1 verifica
  o executável alvo. Regra registrada em AGENTS.md; harness privado run.ps1
  configura SDL_AUDIO_DRIVER=dummy nas próximas execuções. Atalho normal intacto.
- Removidos exe/pdb e objetos obsoletos de test_kyoei_submit_budget: sem alvo
  no CMake/Ninja e sem processo em execução; fontes únicas privadas preservadas.
- Pacotes 20261017 prontos e validados; protótipo segue desligado. Próxima
  investigação: pausas de carregamento e qualidade durante sprint relatadas por
  Davy; medir progresso real entre filas antes de promover readback cooperativo.

### 17/10/2026 — reprodução do desfoque; fidelidade exigida
- Davy esclareceu: personagem borrado e sem detalhe durante sprint. Reproduzido
  com cópia privada de save/cache e áudio dummy mudo: cena inteira borrada,
  HUD nítida; vida 100 para 90; imagem volta a ficar nítida depois de parar.
  Não é prova de textura faltando nem de comportamento correto do PS4.
- Partidas sprint-20261017, sprint-traced-20261017, sprint-draws-20261017 e
  faithful-packets-20261017 fecharam normalmente (exit 0). Evidências privadas
  em build/research/detiler-fix-20261016. Nenhum save original modificado.
- Adicionado diagnóstico opt-in KYOEI_STREAMING_TRACE: escopos lentos de
  texturas/buffers/desenhos/callbacks/pacotes e combinações de dimensões.
  Sem alterar renderização ou sincronização. Limites por categoria/opcode
  evitam que esperas de menu consumam a evidência toda. Desligado por padrão.
- Primeiros uploads lentos 27–42 ms não explicam sozinhos lote de ~300 ms;
  a pausa original ~800 ms permanece sem causa corrigida. Não alegar ganho.
- Prototipada opção de remover desfoque; teste CPU passou, mas Davy interrompeu
  para exigir 100% de fidelidade ao PS4. Retirados opção/atalho/fontes/parâmetro
  da integração imediatamente e recompilado. Teste isolado da opção fechado
  antes do sprint (exit 0); sem confirmação visual. Pesquisa única arquivada
  apenas em build/research/discarded-blur-option-20261017.zip. Não reativar.
- Regra de fidelidade salva em AGENTS.md: preservar efeitos/resolução interna/
  velocidade/comportamento; comparar com referência antes de chamar efeito
  original de bug. Sem reduzir qualidade, remover efeitos ou alterar timestep
  para disfarçar desempenho. ZIPs dos amigos anteriores não contêm a opção.
- docs/re/DESFOQUE-E-TRAVADAS.md registra observações e limites. Build normal e
  launcher CheckOnly PASS após retirada. Teste startup-opcodes-20261017 ainda
  em andamento neste checkpoint; registrar resultado antes de concluir.
- Resultado final startup-opcodes-20261017: exit 0, cenário conferido por
  capturas anteriores do mesmo perfil; recorte desta execução é início/entrada
  da fase, não benchmark de gameplay estabilizado. Upload 64 MiB: ~40 ms;
  pacote 0x2d correspondente: ~46 ms. Sem comando isolado explicando a pausa
  original de ~800 ms neste teste. Escopos inclusivos não provam tempo GPU.
- Nenhuma alteração de efeitos permaneceu no código ou no executável final.
  Derivados do teste da opção descartada removidos após confirmar ausência
  de dependências e processos. Pesquisa única preservada em arquivo privado.
  Build e CheckOnly PASS; diagnóstico novo é opt-in. Travadas ainda pendentes.

### 17/10/2026 — busca exata de imagens e continuidade autorizada
- Otimizada a escolha exata em FindImage: candidata com mesmo início está na
  primeira página. Mantida a última candidata na ordem original, todos os
  critérios de compatibilidade/Picked e mutex; sem cache auxiliar persistente.
  Falta de exata segue integralmente para o caminho anterior de sobreposições.
  Efeitos, resolução, recursos, sincronização e tempo convidado intactos.
- 5.000 layouts aleatórios comparados ao algoritmo de referência PASS; 3.061
  acertos. Verificação em jogo das duas buscas em cada chamada PASS, marco de
  10.000 consultas, imagem conferida e exit 0. Variáveis de verificação e
  comparador anterior documentadas em docs/re/DESFOQUE-E-TRAVADAS.md.
- Par comparativo de mesma vista/25s: original 30,09 FPS e máximo 48,77 ms;
  novo 30,14 FPS e máximo 62,98 ms; seis janelas/720 intervalos cada. Foco fora
  48/48 em ambos; imagens conferidas; ambos exit 0. Não comprova ganho de FPS
  nem resolve a pausa principal. Evidências image-{old,fast}-20261017 privadas.
- Davy pediu continuar testando autonomamente até resolver performance e,
  depois, revisar/enxugar o projeto para quem usar no GitHub. Regra registrada.
  Builds/cache/logs de pesquisa confirmados como ignorados pelo Git.
- Preparado amostrador privado de stacks do thread GpuCommandProcessor via
  GetThreadContext/StackWalk64, sem debugger attach. Ainda sem amostra válida:
  primeira janela de perfil terminou enquanto se preparava a medição de cota.
  Segundo teste profile-street2-20261017 em andamento neste checkpoint.

### 17/10/2026 — cota monitorada e desligamento autorizado
- Davy autorizou continuar testando/corrigindo performance autonomamente e
  checar uso do GPT Work a cada cinco minutos. Confirmou explicitamente que
  13% significa RESTANTES. Nesse limite: interromper tarefas, salvar tudo e
  desligar PC. Não exige nova aprovação ao chegar ao limite.
- Skill OpenAI Docs consultada; documentação oficial confirma uso compartilhado
  Work/Codex e consulta por painel/status. Percentuais lidos da telemetria local,
  não estimados pelo tamanho do contexto. Fonte em docs/DESEMPENHO.md.
- Corrigido check_usage: pin na conversa raiz, sem escolher auxiliar/revisor
  mais novo. Esses auxiliares tinham sessão/conta compartilhadas, mas não são
  o arquivo a selecionar por data. Leitura PowerShell do log grande ficou
  lenta; substituída por Python limitado a 8 MiB, eventos válidos recentes.
  Quatro testes PASS, incluindo 13/13,1%, semanal, dados antigos/futuros,
  log grande e escrita parcial. Última leitura inicial armada: 44% principal,
  91% semanal em 01:14:32; os valores continuam mudando durante o trabalho.
- Monitor privado PID 2236, build/overnight/performance_watch.ps1, armado com
  intervalo 300s e threshold13. Confere dados frescos antes de agir; salva ZIPs
  verificáveis com hashes em cada leitura e antes de desligar. Testes novos
  recusam iniciar se performance-stop-requested existir. Shutdown /s /t 0,
  sem /f; não matar aplicações alheias por nome. Save/dump originais intactos.
- Primeiro fsync com descritor rb falhou no Windows; corrigido para r+b.
  Redirecionamento stderr de aviso Git também foi corrigido; checkpoint real
  validado por hashes/CRC e rodadas Once sem desligamento passaram antes de armar.
  Fonte de controle fica ignorada em build; não distribuir auto-shutdown.

### 17/10/2026 — perfil da rua e tentativa de cache descartada
- Amostrador privado profile-street2-20261017 funcionou sem debugger attach;
  jogo encerrou normalmente (exit 0). Entre 100 stacks do processador de comandos,
  30 incluíram espera de condição dentro de ProcessGraphics, 17 preparação de
  pipeline e 17 BindResources (categorias sobrepostas; não são percentuais de CPU).
  Suspensão para amostrar altera tempos: não usar essa execução como benchmark.
- Experimentado cache limitado/thread-local de ParseFetchShader: comparação de
  todos os bytes consumidos, página única, snapshot antes/depois, sem reutilizar
  descritores de recursos. Testes de mutação/colisão/página PASS; verificação
  em jogo de mais de 10.000 acertos contra decoder original PASS, exit 0.
- Par fetch-heavy-{old,fast}-20261017: 25,67 versus 25,70 FPS, máximos 68,15
  versus 84,32 ms; seis janelas/720 intervalos em cada, exit 0 e foco 0/81 perdido.
  Capturas mostram momentos diferentes do ataque: original HP90 em pé, novo
  HP70 caído. Não são estados visuais equivalentes; resultado não prova ganho
  nem regressão atribuível ao cache. Performance continua pendente.
- Por não haver benefício demonstrado, removidos cache, alvo de teste e adaptação
  do CMake; executável recompilado com decoder original. Pesquisa única preservada
  e conferida em build/research/discarded-fetch-cache-20261017.zip, SHA256
  4896db36fa6e643cb926e82588cb5bbbad410cf291ad5037670456a529b74787.
  Nenhum efeito, resolução, velocidade ou sincronização removido. Dump/saves
  originais intactos. Próximo perfil deve separar CPU convidada e esperas,
  controlando dano/animação do gigante antes de comparar FPS.

### 17/10/2026 — perfil ampliado e custo do diagnóstico desligado
- guest-cpu-profile-20261017: exit 0, foco perdido 0/81. Em intervalo de ~2s,
  processador de comandos acumulou 1.578 ms de CPU, main convidado 672 ms;
  três threads convidadas amostradas passaram 71/78/76 de 100 amostras em
  espera de condição. Não confundir amostras de parede com porcentagem de CPU.
- gpu-long-profile-20261017: exit 0, foco perdido 0/80. Mil amostras privadas
  do processador de comandos após a janela de medição: 720 incluíram Draw,
  207 GetGraphicsPipeline, 169 BindResources, 117 StageSpecialization,
  100 ParseFetchShader, 98 BindVertexBuffers, 43 Scheduler::Wait.
  Categorias se sobrepõem; medição por suspensão não vale como benchmark FPS.
- Antes da suspensão, três consultas nvidia-smi: GPU 35–38%, 3.868 MiB,
  clock 1.215–1.335 MHz. Uso inclui outros aplicativos; não estabelece sozinho
  gargalo. Conjunto de evidências aponta para custo do frontend gráfico na CPU
  e esperas, sem provar causa única. Host Xeon E5-2640 v3, 8 cores/16 threads.
- Ctor de KyoeiStreamingTrace::Scope apareceu como PC em 21/1.000 amostras,
  mesmo sem registros de streaming ativos. Cada pacote/desenho inicializava
  metadados antes de conferir Enabled. Alterado para retornar antes desses
  preenchimentos quando desligado; start continua zero e destructor retorna
  sem ler payload. Modo ligado mantém os mesmos campos/tempos e limite.
  Build PASS. Teste mudo streaming-off-fast-20261017 em andamento; registrar
  resultado antes de promover o pequeno ajuste. Não há ganho de FPS comprovado.
- Perfis, deltas e amostrador permanecem privados em build/research; não adicionar
  ferramentas de suspensão nem dados de jogo ao pacote dos usuários.

### Parada automatica autorizada por cota - 2026-10-17T01:47:08.5305607-03:00
- Cota restante: principal 13.0%, semanal 86.0%.
- Testes encerrados; checkpoint final verificado em build/overnight. Desligamento solicitado por orientacao de Davy em 17/10/2026. Retomar do diario e do Git; performance ainda depende das evidencias registradas.


### 17/10/2026 - abertura solicitada por Davy
- Iniciado o launcher normal City Shrouded in Shadow.exe a pedido de Davy, sem alterar qualidade ou configuracoes de desempenho. Processo inicial PID 22004. Nenhuma nova melhora de FPS comprovada nesta abertura.
  Davy esclareceu em seguida que "rodar no talo" significa trabalhar para
  atingir 60 FPS, e nao apenas abrir o jogo. Investigacao retomada abaixo.

### 17/10/2026 — retomada com meta explícita de 60 FPS
- Conferido o teste streaming-off-fast-20261017 anterior: log encerra com
  Exit code 0. Nao havia resultado pendente de encerramento; o ajuste de
  inicializacao do diagnostico desligado permanece sem ganho de FPS provado.
- Sinal antigo performance-stop-requested arquivado apos nova autorizacao
  de continuidade. Nenhum monitor de desligamento rearmado nesta retomada.
- Experimento opt-in de eliminar somente chamadas setVertexInputEXT com
  descricoes identicas no mesmo scheduler/tick. Recursos, buffers e barreiras
  continuam atualizados em todo desenho. Teste de stride, rate, divisor,
  formato, offset, mudanca de scheduler/tick e listas vazias PASS; build PASS.
  Comparacao inicial: 22,57/28,47 FPS, maximos 232,81/91,70 ms, 840/960
  intervalos. Mesma vista/HP100, ambos exit0 e foco ausente em todos os checks.
  Variacao grande de pausas limita atribuicao; nao promover ganho como provado.
- Encontrado temporizador comum do Windows na apresentacao. Fixture privada
  de 12 esperas por caso: pedido 16,667 ms levou mediana 31,201 ms no timer
  comum e 17,023 ms no high-resolution; pedido 8 ms levou 15,484/8,326 ms.
  Nao alterado relogio global nem configuracao de outros aplicativos.
- Adaptador experimental exclusivo da apresentacao preserva periodo/debito
  de Common::AccurateTimer e usa handle high-resolution com fallback original.
  Nao substitui funcoes gerais de thread nem altera clocks/timestep do guest.
  Teste com APIs simuladas: fallback desativado/nao suportado, falha ao armar,
  sinal negativo/limite de espera, quadro atrasado sem dormir e fechamento PASS.
- Par timer-high/timer-legacy: 27,16/26,85 FPS, maximos 83,84/110,74 ms,
  960 intervalos cada, ambos exit0. Cadencia do timer melhorou (p95 ~17 ms
  em vez de ~30 ms), mas nao prova ganho de FPS nem resolve a meta de 60.
- Todos os testes usam SDL audio dummy e save/cache privados. Comparacao
  em primeiro plano, sem giros de camera, em andamento neste checkpoint.
  Candidatos continuam opt-in; nada alterado nos pacotes dos amigos.
- Par de candidatos juntos (tentativa de foco sem sucesso): 27,54/27,62 FPS,
  maximos 79,32/100,48 ms, 960 intervalos cada, sem beneficio demonstrado.
  A janela aparece para Davy; o SO recusou foco automatico e a maioria dos
  checks permaneceu em segundo plano. Nao alterar aplicativos alheios.
- Candidato seguinte: cache limitado de entradas da especializacao de shader,
  com recursos atualizados normalmente em cada chamada. Confere runtime,
  bindings, descritores normalizados, codigo fetch completo dentro de uma
  pagina, quantidade de modulos e epoca de carregamento/edicao dos shaders.
  Antes/depois do construtor original precisam concordar para salvar entrada;
  primeira observacao fetch apenas aquece o snapshot. Default desligado.
- 6.000 casos diferenciais de enderecos/tamanhos, imagens, fmask, samplers,
  mudanca entre observacoes, runtime/bindings/owner/modulos/epoca/pagina PASS.
  Build PASS. stage-verify-20261017 manteve construtor/busca originais e
  conferiu toda previsao, incluindo marco de 10.000, sem divergencias; exit0.
- A/B na mesma partida, blocos de 20s, descartando primeiro resumo apos troca
  e blocos incompletos: original 26,29 FPS/720 intervalos, candidato 28,06/480.
  Blocos originais 25,19 e 27,49; candidato 28,06. Variacao/carga externa e
  amostra desbalanceada nao sustentam atribuir ganho. Foco ausente em todos
  os checks dos tres blocos completos. Nao declarar meta de 60 FPS resolvida.
- Normalizacao adicional exclui num_records de buffers depois de preservar
  nullness/IsStorage: esse tamanho nao integra os campos da especializacao
  fixa. Os 6.000 casos continuam PASS. Verificacao real normalizada em andamento;
  proxima medicao registra acertos e prolonga janela para quatro blocos completos.
  Fontes experimentais ainda sem commit; evidencias privadas em build/research.
- Verificacao normalizada terminou exit0, com todas as previsoes comparadas
  ao caminho original, incluindo marco de 10.000, sem divergencia.
- A/B normalizado na mesma partida: blocos antigos 29,93 e 29,88 FPS;
  blocos novos 31,75 e 32,00 FPS. Ponderado: 29,90/31,89 FPS, com
  960/1080 intervalos. Acertos do cache ~66,2%. Primeiro bloco novo teve
  foco ausente em 25/39 checks; segundo novo e primeiro antigo em primeiro
  plano em todos os checks. Ganho modesto consistente nesta vista, nao 60.
  Exit0; captura final inspecionada, sem corrupcao visual aparente.
- Davy relatou pequenas travadas ao girar a camera. Na verificacao anterior
  existem esperas de GPU de 226,48 e 254,25 ms no readback 1x1 e uma nova
  permutacao de shader. Correlacao nao prova causa: nao atribuir as pausas
  apenas a compilacao nem remover o feedback da GPU. Nova verificacao com
  giro e caminhada iniciada (stage-camera-verify-20261017), ainda pendente.
- stage-camera-verify-20261017 terminou exit0, 21/21 checks em primeiro
  plano. Giro/corrida confirmados nas capturas, sem corrupcao aparente;
  marco de 10.000 previsoes e nenhuma divergencia. Resumos 22,51 FPS,
  maximo 149,40 ms: verificacao duplica trabalho e capturas perturbam o
  trecho; nao usar esse numero para medir ganho do cache.
- Experimento com 64 entradas: exit0, acertos ~68%, sem beneficio
  demonstrado. Bloco novo 7 coincidiu com compilacao do teste e nao serve
  para comparar FPS; bloco novo 9 limpo 30,16 FPS, antigos ~29,86.
  Voltado a 8 entradas. Ranges e logs preservados no perfil privado.
- Retiradas da integracao somente as experiencias de vertex input e timer
  desta sessao, sem beneficio de FPS demonstrado. Fontes e delta arquivados
  em build/research/discarded-performance-20261017; nao e reestruturacao
  geral. Executavel recompilado sem os dois caminhos.
- Candidato refinado projeta os campos de recursos efetivamente lidos pela
  especializacao fixa, preservando nullness, formatos/conversoes, swizzle,
  storage, bindings e runtime. Binder continua recebendo dados atuais de
  filtros, tamanhos e enderecos em todo desenho. 12.000 mutacoes diferenciais
  e lotacao/expulsao PASS; build PASS. Validacao real com camera em andamento
  (stage-projected-camera-verify-20261017), candidato ainda opt-in.
- Verificacao refinada com camera/corrida concluiu exit0, 23/23 checks com
  foco, marco de 10.000 e nenhuma divergencia; captura de corrida conferida.
  A/B refinado iniciado, sem compilar nem capturar durante a amostra.
- A/B refinado terminou exit0. Quatro blocos completos: antigos 29,90 e
  29,88 FPS; novos 30,15 e 30,28. Ponderado 29,89/30,22 FPS, 960 intervalos
  por caminho; maximos antigos 50,25/51,67 ms, novos 35,70/35,63 ms nesta
  vista parada. Acertos ~79%; ganho medio insuficiente para promover como
  solucao de desempenho. Foco ausente em 6/39 checks do primeiro antigo;
  demais tres blocos integralmente em primeiro plano. Captura final conferida.
- Cache permanece desligado por padrao. Nao atribuir ao candidato refinado
  os 31,89 FPS do primeiro candidato de snapshots: sao implementacoes e
  coletas diferentes. Nem a media de 60 FPS nem a correcao do stutter de
  camera foram demonstradas. Avanco funcional/testes registrado em f56f059;
  testes encerrados normalmente, sem alterar efeitos, resolucao ou simulacao.

### 17/10/2026 - rodada com orcamento de 20 pontos da cota de 5h
- Davy cobrou resolver desempenho dentro de 20% de uso. Leitura inicial
  da telemetria da conversa: 27% restante (09:54 local), portanto reserva
  de 7% para esta rodada; nenhum desligamento automatico rearmado.
- Referencia budget-baseline-20261017: 29,90 FPS/1080 intervalos, maximo
  51,35 ms. Depois da amostra, NVML mediu GPU global 36-38%, clocks graficos
  1410-1560 MHz, memoria 7000 MHz; nao atribuir uso de outros apps ao jogo.
  CPU em 2s: GpuCommandProcessor 1109 ms, guest main 703 ms. Perfil posterior
  de 1000 pilhas: 343 esperando VO label, 586 em Draw; driver, decodificacao
  fetch e alocacoes entre os custos ativos. Amostragem nao integra o FPS.
- Novo candidato atua diretamente em ParseFetchShader: chave somente nos
  bytes das instrucoes, sem refazer a chave de descritores da experiencia
  anterior. 128 slots limitados por thread, ate 512 bytes dentro da mesma
  pagina; primeira observacao apenas aquece e uma segunda leitura original
  precisa estar entre snapshots iguais. Recursos seguem lidos normalmente.
  Arquivo fonte original adaptado em build, sem mudar copia fixa/ABI.
- Teste de 10.000 mutacoes, alteracao durante parser, comprimento/pagina e
  colisao PASS; build PASS. Verificacao de todas as previsoes reais inclui
  os nove campos dos atributos (mais rigorosa que operator== original),
  marco de 10.000 sem divergencia. Giro/corrida concluiu exit0, foco ausente
  em 8/20 checks; nao usar esse teste de verificacao para alegar ganho FPS.
  fetch-ab-budget-20261017 em andamento; novo cache ainda opt-in.
- Davy adicionou seis arquivos de fontes de outros ports e pediu priorizar
  a analise, depois encerrar a analise por causa da cota. Extraidos apenas
  codigo/documentacao para pesquisa privada; nenhum executavel externo rodou.
  Relatorio em docs/re/PORTS-REFERENCIA-20261017.md, inventario/hashes ao lado.
- Diferencas verificadas: Bloodborne tem gravacao Vulkan paralela e cache
  direto do parser; GOW3/GR2/TLG/ISS preferem RAM cacheada para stream e a
  familia TLG memoiza transicoes parciais. P.T. usa logica e renderer proprios.
  Comparador de especializacao corrigido da familia TLG ja existe na base.
  Nao copiar patches de FPS especificos de outros jogos para o Kyoei.
- Teste stream-cached-budget-20261017: 30,00 FPS/1080 intervalos, maximo
  62,13 ms, exit0; foco ausente em 57/69 checks. Flags reais 0xe (RAM cacheada)
  contra 0x7 (device local/host visible/coherent) no caminho antigo. Sem
  ganho comprovado contra referencia de 29,90 em primeiro plano.
- fetch-ab-budget-20261017 encerrou ainda no menu, sem amostra. Nova tentativa
  fetch-ab-device-budget-20261017 concluiu exit0: ponderado 26,29/27,12 FPS,
  840 intervalos por caminho. Blocos antigos 27,87/24,44, novos 27,50/26,85;
  quase todos os checks sem foco. Taxa de acertos ~82-84%, sem ganho suficiente
  nem correcao de stutter. Nao promover candidato com essa evidencia.
- A pedido de fechar a analise, nenhum novo teste iniciado. Fontes unicas e
  delta CMake dos dois candidatos desta rodada arquivados em
  build/research/budget-candidates-20261017; integracao de producao restaurada.
  Executavel local de pesquisa permanece recriavel; nenhum pacote/atalho
  recebeu novas flags. Testes fechados normalmente, sem processo kyoei ativo.
  A diferenca estrutural de gravacao serial merece investigacao integrada,
  nao mais uma rodada de flags. 60 FPS continuam nao demonstrados.


### 17/10/2026 — atualizador cumulativo para amigos
- Davy escolheu entrega manual por ZIP e teste no Linux via Wine/Proton. O port Linux nativo continua sem executar o jogo. Sem endpoint de releases, manter atualização manual; não adicionar busca automática sem canal publicado.
- Recompilado `kyoei.exe` com CMake de produção restaurado. Gerados `Kyoei-Windows-Update-20261017b.zip` e `Kyoei-Linux-Wine-Update-20261017b.zip`, cada um com runtime, aplicador, instruções e manifesto/hash. Nenhum dado do jogo foi empacotado.
- Pacotes cumulativos aceitam Friends-Test original e payloads de 15, 16, 17 e 17b. Validação em oito combinações (quatro bases × Windows/Linux-Wine): check-only sem escrita, aplicação e reaplicação, hash final, backup, save e prefixo preservados. Relatório privado: `build/research/update-validation-20261017b.json`.
- SHA256 dos ZIPs: Windows `5c8d50a2ed3ba199eafb2c62734713c3e9e58bfb6aa234f09317968ce1df907b`; Linux/Wine `0655b01cc6f525b7170ab22ebdb97163f8e251891123b4c4db4b8210a1296c14`. Runtime comum `6196e2a968e248af422d080f21fa8449e67ce55e726501adf64743c6033f0e55`. Instruções em `docs/ATUALIZACOES-AMIGOS.md`.
- Portable pessoal atualizado pelo `tools/update_portable_runtime.py`; runtime SHA256 bate com o dos dois ZIPs. Launcher `--check` PASS. Saves/dados existentes: 37.352.560 bytes. O manifesto pessoal marca `privacy_scan_passed=false`, pois esta atualização local não executou uma nova finalização/varredura; o portable continua privado e não foi distribuído.


### 17/10/2026 - gravacao Vulkan delegada: ganho local de 34,8%
- Davy pediu continuar usando os outros ports como base e resolver desempenho.
  Diario, estado Git e comparativo dos seis ports lidos; Godzilla nao alterado.
- Implementada fila externa por scheduler, inspirada na separacao de gravacao
  do Bloodborne, preservando ABI do backend precompilado. Um worker por
  scheduler, comandos/arrays em blocos reutilizaveis de 64 KiB, fila limitada.
  Sem segmentos paralelos por fluxo ou submissao assincrona nesta etapa.
- Adaptadores cobrem todas as 276 entradas vkCmd do header local. Valores,
  bindings, descritores, barreiras, attachments e transferencias possuem
  snapshots. Payload nao suportado/pNext drena antes de executar diretamente.
  Configure rejeita entradas novas nao revisadas. Drenagem antes de encerrar
  buffer/submeter/destruir; ImGui privado drena depois de beginRendering.
- Testes: 100.000 comandos ordenados + 100.000 snapshots com reuso, mutacao
  imediata, limites, campos irrelevantes de descritores invalidos, imagens,
  copias, cores, fallback e vida util PASS. Cache fetch anterior reaproveitado:
  10.000 mutacoes/limites/colisoes PASS. Temporizador de apresentacao anterior
  reaproveitado com mesmo periodo/debito, sem alterar relogios do guest; PASS.
- Primeiras variantes nao resolveram: fila inicial 29,19 FPS/360 intervalos;
  controle separado 28,55/480; blocos reutilizaveis 29,40/2760. Sem foco.
  Primeiro A/B combinado 28,55/28,34, 1200 intervalos por caminho, exit0.
- Perfil posterior ao A/B identificou drenagens em CopyBuffer e ColorWriteMask.
  Corrigida assinatura de ColorWriteMask (inclui firstAttachment) e snapshot
  das copias. Segundo A/B 28,75/29,78, 1200 intervalos por caminho, exit0.
  Incluidas depois transferencias de imagens, clear, blit, viewport e scissor.
- Ultimo A/B, mesma partida/vista, timer preciso em AMBOS os caminhos:
  originais 29,87 e 29,39 FPS; novos 40,06 e 39,89. Ponderado 29,65/39,97
  (+34,8%), 1320/1800 intervalos; maximos 104,66/37,31 ms. Primeiro resumo
  apos cada troca descartado. 197/197 checks sem foco; sem compilar, capturar
  ou suspender threads durante os blocos. Exit0. Nao atribuir todo ganho ao
  temporizador nem comparar diretamente com variantes antigas distintas.
- Camera/caminhada/corrida: exit0, 34/34 checks sem foco, capturas conferidas,
  efeitos de explosao/dano preservados. 10.000 decodificacoes reais completas
  comparadas ao parser original, nenhuma divergencia. FPS desse teste nao e
  benchmark: verificacao e capturas acrescentam trabalho. Stutter global e
  meta de 60 FPS continuam nao demonstrados.
- Fixture GPU real RGBA/padding/publicacao PASS, inclusive depois das novas
  transferencias e com defaults da previa; executavel sanitizado instalado
  no portable tambem PASS. Todos os testes abertos usam SDL audio dummy;
  audio dos launchers normais preservado.
- Compilacao de pesquisa ON e compilacao limpa normal OFF PASS. Pesquisa fica
  atras de KYOEI_EXPERIMENTAL_RENDERER (OFF por padrao no repositorio), sem
  custo de compilar a fila/caches experimentais para clones normais. Na previa
  ON os tres caminhos ficam ativos por padrao; flags =0 permitem caminho antigo
  ao executar o runtime diretamente. Perfil local build esta ON.
- Portatil pessoal atualizado com a previa: runtime sanitizado SHA256
  91f5573df20446a765e85223ffcff1bca528b42e52aa656ec065d57e98da7c02.
  Tres arquivos de dados/saves identicos por hash antes/depois. Runtime antigo
  6196e2a968e248af422d080f21fa8449e67ce55e726501adf64743c6033f0e55
  conferido dentro do ZIP 20261017b como reversao. Nenhum ZIP para amigos mudou.
  Ambos launchers --check PASS. Portable continua privado, manifesto sem nova
  finalizacao/varredura. Evidencias em build/research; detalhes publicos em
  docs/re/GRAVACAO-VULKAN.md. Build temporario de verificacao OFF removido apos
  conferir resultado/dependencias/processos; log mantido. Testes encerrados.
- Ganho comprovado apenas nessa vista na RTX 2060 local. Nao declarar 60 FPS,
  ausencia de stutter, validacao de outras GPUs/Linux ou jogo inteiro concluido.


## 17/10/2026 - segunda rodada: copias por draw e colisoes de fetch

- Pedido de Davy: estudar e melhorar mais. Leitura do diario e das fontes
  locais de referencia, sem modificar Godzilla nem a copia fixa. Testes mudos
  via SDL dummy, com perfis somente depois dos trechos medidos.
- Perfil inicial: 1.000 pilhas da thread grafica, 402 em espera, 64 com
  especializacao, 20 com parser fetch original, 14 com alocacao da conversao
  optional<const FetchShaderData> para optional<FetchShaderData> no draw.
  O trabalhador de gravacao tem folga; nao atribuir todo limite a ele.
- Tentativa de comparar descritores completos antes da especializacao,
  inspirada em TLG/ISS: controles 32,73/33,21 FPS contra 29,20/30,22 FPS.
  Alternativa rejeitada e removida do codigo. Patch/evidencias privados
  preservados em build/research/raw-stage-rejected-20261017.patch e logs.
- Correcao pequena em CMake: GetDrawOffsets da copia gerada recebe o tipo
  exato optional<const FetchShaderData> por referencia, evitando copiar o
  vetor a cada draw. Nenhuma mudanca de layout/ABI. Perfil posterior de
  1.000 pilhas: zero amostras dessa alocacao; nao mede ganho isolado de FPS.
- Cache fetch agora com 128 grupos de duas entradas, substituicao limitada.
  Mantidos limite512/mesma pagina, comparacao integral de bytes e duas
  observacoes em torno do parser. Record exige endereco exato da Probe.
  Teste PASS: 10.000 mutacoes e 10.000 alternancias colidentes, mais
  comprimento/pagina/observacao misturada/propriedade/substituicao.
- Build ON PASS. A/B fetch-ways-ab-20261017, mesmo processo/vista/Fifo,
  gravacao+timer+correcao da copia iguais nos dois caminhos. Blocos de uma
  entrada: 33,87/33,51 FPS; duas: 35,49/35,15. Media ponderada
  33,7032/35,3211 FPS (+4,8%), 1.560/1.440 intervalos; max52,36/49,04ms.
  Primeiro resumo por bloco descartado; nenhum restante diagnostic=1;
  196/196 checks sem foco. Sem build/captura/perfil durante a amostra,
  exit0. Controles de colisoes: falhas172.470/1.146.594 (15,04%) versus
  88.080/1.204.012 (7,32%): aproximadamente metade do trabalho refeito
  por consulta. Nao somar ao +34,8% anterior nem comparar FPS absolutos
  de sessoes diferentes. 60 FPS e stutter global continuam nao comprovados.
- Duas entradas por padrao na previa ON; KYOEI_FETCH_CACHE_WAYS=1 mantem
  controle de uma entrada; controle privado por arquivo para A/B. Defaults
  normais do repositorio continuam com renderer experimental OFF.
- Giro/caminhada/corrida fetch-ways-camera-20261017 exit0, 34/34 sem foco;
  capturas conferidas, fogo/dano/desfoque preservados. Parser verificou
  10.000 decodificacoes completas sem divergencia. FPS desse teste nao
  mede performance: verificacao e capturas acrescentam trabalho.
- Portatil pessoal atualizado. Runtime sanitizado SHA256
  108d266d052a9871195ed83963321f53026dd2e9cc440b033b2bde11ab24966c.
  Fixture GPU instalada pixel/padding/publicacao PASS; ambos launchers
  --check exit0; tres arquivos persistentes identicos por hash. Testes
  encerrados. Audio normal dos atalhos preservado. Nenhum ZIP para amigos
  modificado; manifesto privado continua sem nova finalizacao/varredura.
- Metodo completo em docs/re/GRAVACAO-VULKAN.md; resumo atualizado em
  docs/DESEMPENHO.md. Sem reestruturacao ampla ou alteracao da qualidade.


## 18/10/2026 - registros de vida util do stream, sem alterar os dados

- Davy pediu continuar trabalhando. Diario, AGENTS e perfis anteriores lidos;
  referencia local Bloodborne/TLG/GOW3. Sem modificar Godzilla nem a copia fixa.
  Testes no mudo via SDL dummy, sem mexer no audio dos outros aplicativos.
- Encontrado erro em StreamBuffer::Commit: current_watch_cursor aponta para
  a proxima posicao livre, mas era usado ao tentar ampliar o ultimo registro
  do mesmo tick. Referencias usam cursor-1. Adaptacao local em copia de
  buffer.cpp via CMake, helper ExtendLast, sem mudanca de layout/ABI.
  Map/Commit, flush/invalidate, rotacao e protecao por ticks preservados.
- Teste de referencia: 100.000 alocacoes aleatorias, alinhamentos, ticks,
  crescimento de armazenamento e 440 voltas. Mesmos ticks de protecao em
  todas as consultas, registros100.000->5.644 no conjunto sintetico. PASS.
- Nova fixture GPU opt-in dentro da infraestrutura --gpu-test: 2.048 copias
  de128bytes para destinos distintos, stream com16 alocacoes, 127 voltas e
  envios parciais a cada37 copias. Comparados todos262.144bytes depois da
  conclusao. PASS com compactacao e com referencia individual. Fixture
  RGBA/padding/publicacao tambem PASS em ambos. Alinhamento considera atom
  size/noncoherent; headers VMA depois dos Vulkan e funcao com ligacao C.
- A/B stream-watch-ab-20261018: Fifo, gravacao+fetch2+timer iguais, cache de
  dados desligado. Warm80,094s depois do stage201, blocos0/1/1/0 com controles
  nas pontas. Sem compactacao36,33/35,74FPS; com38,15/37,36FPS. Ponderado
  36,0111/37,7521FPS (+4,8348%), 1.560/1.680 intervalos. Primeiro resumo por
  bloco descartado; nenhum resumo diagnostic=1; 196/196checks sem foco.
  Sem compilacao, captura ou pilhas durante a amostra. Exit0. Maximos
  40,65/51,66ms nao melhoraram; nao declarar stutter resolvido ou60FPS.
- Experimento separado de reutilizar snapshots de uploads pequenos, mesmo
  tick/bytes completos, reaproveitou aproximadamente37%, mas FPS variaveis.
  Primeiro AB0/1/0/1:30,40/30,18/33,31/38,55. Repeticao com watches corrigidos,
  warm80 e ordem1/0/0/1:39,16/34,12/34,72/33,15. Medias positivas nao sao
  ganho consistente; cache/flags/alvo de teste removidos do codigo mantido.
  Fontes/integracao arquivadas em build/research/upload-reuse-rejected-20261018;
  ultimo rascunho de captura depois de Map nao foi compilado/medido. Executavel,
  PDB e objetos obsoletos removidos apos conferir substitutos/dependencias,
  ausencia do processo e hashes das copias de evidencia. Mantidas provas.
- Build final ON e teste de watches PASS; codigo novo so no renderer
  experimental (OFF continua padrao do repositorio). Compactacao ON por
  padrao na previa pessoal; =0 e referencia individual conservadora, sem
  restaurar acesso incorreto a posicao livre. Controle privado amortizado
  para A/B. Nenhuma alteracao de resolucao, efeitos, shaders ou simulacao.
- Giro/caminhada/corrida stream-watch-camera-20261018 exit0, 34/34checks sem
  foco, capturas inspecionadas com explosao/dano/particulas/desfoque preservados.
  10.000 comparacoes completas fetch sem divergencias. FPS desse teste nao
  e benchmark, por verificacao e capturas. Testes encerrados.
- Portatil pessoal atualizado; runtime sanitizado SHA256
  f095a67312bdb4ab71e011157c869c4ff72f3c43cb638b09089575b47199aa6d.
  Runtime instalado passou as2.048copias/127voltas e fixtureRGBA/publicacao;
  ambos launchers --check exit0. Tres arquivos persistentes identicos por hash
  antes/depois (excluidos caches/logs). Audio normal dos atalhos preservado.
  Nenhum ZIP para amigos modificado; manifesto privado sem nova finalizacao.
- Metodo em docs/re/STREAM-WATCHES.md; resumo em docs/DESEMPENHO.md. Nao
  somar percentuais de sessoes diferentes, prometer60FPS ou validar outras
  GPUs/jogo inteiro. Sem reestruturacao ampla, mantida so a correcao comprovada.


## 18/10/2026 - sessao continua de correcao do stutter

- Orientacao atual de Davy: continuar trabalhando; quando a janela principal atingir10% restantes, encerrar testes/compilacoes do projeto, salvar/checkpoint verificado e desligar o PC. Substitui os limiares anteriores de7% e13%. Monitor privado sem forcar descarte de aplicativos; apenas testes do projeto sao encerrados pelo monitor. Sessao de telemetria escolhida explicitamente:01a14ba4-5a99-7423-8c18-a0c65460bdb7. Checkpoints a cada5min, parada apenas com leitura recente; falha de checkpoint impede desligamento. Fontes e diagnosticos continuam privados em build/research; registrar resultados antes da parada.

- Diagnostico novo validado: trace por quadro/escopos >=8ms, reservado e limitado131.072eventos em memoria, dump somente no encerramento. Writer fecha sincronizado, overflow declarado, parser recusa trace truncado/janela sem QPC. Testes C++ e Python PASS, FrameStats ativo/desligado PASS, build PASS. Sem ganho de performance reivindicado por instrumentacao.
- Referencia parada300s:12.960intervalos,43,21FPS,2>50ms/0>100ms,max51,89ms. Rota com giro/corrida/efeitos90s:3.176intervalos,35,22FPS,p95=34,19,p99=50,06,p99,9=98,19,max131,35ms;37>50ms,3>100ms. Trace38.306eventos,zero descartes,exit0,163/163sem foco. Sem capturas dentro da janela. Batches longos coincidem com picos; readback256bytes/21,25ms so explica parte de um deles. Nao culpar readback por todos os picos. Metodo em docs/re/STUTTER-TRACE.md.
- Investigacao continua, monitor privado de10% ativo (PID e log em build/overnight/stutter-watch-20261018.*), sessao exata indicada acima. Teste cooperativo da mesma rota em andamento; nao e resultado validado ainda. Portatil pessoal permanece na versao funcional anterior, sem instalar apenas diagnosticos.
- Cooperativo concluido:25,30FPS,408>50ms/10>100ms,max166,18ms,zero descartes. Contexto final diferente (HP80/caido vs HP100/em pe); nao e A/B controlado nem prova de regressao causal. Mantido desligado. Novo detalhamento opcional por pacote/draw/imagem/backlog Vulkan compilado; testes de gravacao, trace, FrameStats ativo/desligado e analisador PASS. Rota detalhada em andamento; relogios por pacote acrescentam custo, nao comparar seu FPS como ganho. Nenhuma nova correcao de stutter validada ainda.
- Detalhamento validado na rota stutter-packets-20261018:exit0,38.130eventos/zero descartes,2.248intervalos; max149,74ms e quatro>100ms. Dados diagnosticamente perturbados, sem comparacao de FPS. Picos coincidem com batches110..146ms; nao ha pacote/draw/imagem individual longo suficiente para explicar inteiro. Espera VO16,7ms e callbacks/readback~13..15ms explicam partes; pacote pode incluir suspensao de coroutine, nao e tempo puro CPU/GPU. Proxima coleta stutter-motion-profile-20261018 amostra stacks durante movimento, com detalhamento por pacote desligado; resultado ainda pendente. Portatil nao atualizado com diagnostico.
- Perfil em movimento concluido exit0,1.000amostras:628incluem Draw,188BindResources,112GetProgram,64StageSpecialization,37folhas NtAlertThreadByThreadId originadas em notificacoes da fila Vulkan. Esperas234 principalmente VO; nao somar inclusivos. Coleta com suspensoes de thread nao e benchmark. Detalhado:zero eventos backlog>=1ms/chunk>=8ms; pacotes longos principalmente WaitRegMem(642)/EventWriteEos(225), imagens raras. Experimento em andamento: evitar notify_one quando worker ja ocupado ou com fila pendente, decisao sob mesmo mutex da espera. Ordem e barreiras intactas, opt-in KYOEI_VULKAN_COALESCE_WAKES=1, controle privado KYOEI_VULKAN_WAKE_CONTROL. Build/teste200mil comandos por modo+100despertares por modo PASS, fixtureGPU2.048copias/127voltas/publicacaoRGBA PASS. Comparacao wake-ab-20261018 ainda pendente; nao instalar/promover sem beneficio.
- Wake ABBA concluido:39,02->41,25FPS agregado, mas p99=34,78->49,08ms e max50,47->86,49ms;1->15intervalos>50ms. Primeiro segundo de cada bloco excluido,25s/bloco; nenhuma promessa de ganho/stutter. Repeticao invertida wake-ba-20261018 em andamento. Rascunho adicional192comandos/chunk (original96, teto32KiB igual) em src ainda NAO compilado/testado; KYOEI_VULKAN_LARGER_BATCH/controle privado KYOEI_VULKAN_BATCH_CONTROL, ambos desligados por padrao. Nao compilar enquanto a repeticao mede. Portatil continua na versao anterior funcional.
- Repeticao wake BAAB:35,89->34,44FPS(-4,06%),p99=34,79->34,92ms,max66,79->50,09ms,3->2intervalos>50ms. Beneficio inconsistente; sinalizacao/flags/setter de coalescencia removidos do codigo ativo. Fontes/evidencias arquivadas em build/research/wake-coalesce-rejected-20261018 (snapshot inclui rascunho seguinte de lote192). Experimento de lotes192 separado em compilacao, original96 continua padrao; nenhum ganho validado. Nao repetir automaticamente coalescencia amanha.
- Lotes192 compilados e passaram ordem/ownership/backlog+100despertares em96/192, GPU2.048copias/127voltas/publicacaoRGBA PASS. Comparacao batch-ab-20261018 em andamento, lotes96 continuam padrao. Rascunho independente em src/kyoei_fetch_shader_cache.cpp:512buckets/2vias vs128/2, mesma verificacao integral dos bytes e duas observacoes; opt-in KYOEI_FETCH_CACHE_LARGE/controle privado *_CONTROL. Ainda NAO compilado/medido; nao instalar/promover. Sem mudanca de efeitos/resolucao/simulacao.
- Lotes ABBA:34,23->36,63FPS, mas p99=35,21->79,43ms,max51,11->235,31ms,5->38>50ms e0->9>100ms. Picos concentrados no bloco2, sem compilacao longa identificada; repetir batch-ba-20261018 invertido (em andamento), sem culpar causalmente so a opcao. Cache512 compilado/generico de mutacoes/collisoes PASS, mas ainda sem comparacao real/diferencial10mil. Preparado diagnostico novo em FrameStats: reutilizar contadores de amostragem para registrar chamadas draw/dispatch por batch12, campos address/bytes; teste dedicado batch-trace/parser e documentacao preparados, ainda NAO compilados/validados. Objetivo distinguir aumento de trabalho de esperas, nao contar primitivas/quadros. Nao compilar durante o benchmark.
- Lotes BAAB:35,98->35,53FPS(-1,26%),p99=53,47->48,94ms,max84,67->86,00ms,25->13>50ms. Resultado misto/inconsistente; candidato192 e flags/setter retirados do codigo ativo. Snapshot/evidencias em build/research/batch192-rejected-20261018. Gravacao original96 restaurada, teste PASS. Nao promover nem reinstalar automaticamente as duas tentativas rejeitadas.
- Contadores de chamadas por batch compilados/validados: FrameStats normal/desligado PASS; modo batch-trace confirma exatamente5draw/3dispatch e ignora100draw de outra thread; analisadorPython PASS. Contadores existentes reutilizados, sem novas consultas de relogio por draw. Cache512 continua opt-in e sem beneficio validado; fetch-large-ab-20261018 em andamento. Portatil pessoal permanece em c01d852, sem diagnosticos/prototipos novos.
- Monitor privado reativado PID18548: uso consultado a cada5min acima20%, cada60s em<=20%, cada15s em<=13%; checkpoints continuam5min e final obrigatorio verificado antes de shutdown sem /f. ParserPowerShell validado. Limiar final permanece10% da janela principal, somente leituras recentes. Ultima leitura durante retomada25%; compilacoes/testes novos conferem marcador de parada. Logs/PID no mesmo caminho privado.
- Cache512 ABBA:39,54->40,20FPS(+1,66%),p99=34,67->34,88ms,max50,10->49,85ms,1->0>50ms,nenhum>100ms. Ganho pequeno, nao prova stutter resolvido. Contadores brutos:128teve293.524falhas/4.495.734consultas (inclui aquecimento);512teve45.676/2.752.725. Nao confundir reducao de falhas com FPS. Contagem nova real funciona: medianas1383..1388chamadas draw/batch em todos os blocos, max1398;17..18dispatch nos piores. Repeticao fetch-large-ba-20261018 em andamento; sem instalar/promover ainda.
- Monitor atualizado novamente, PID15116: mantidos10%/frequencias/checkpoint5min; ao atingir reserva, para testes/jobs e abre30s para agente salvar/pausar antes do checkpoint final/desligamento. Cancelamento explicito pelo marcador stop-stutter-watch-20261018 durante essa janela impede shutdown. Parser validado, ultima leitura20%. Esse intervalo e para concluir o salvamento, nao continuar experimentos.
- Cache512 BAAB:41,85->42,22FPS(+0,87%),p99=34,37->34,44ms,max35,64->35,33ms,nenhum>50ms em ambos. Controles reduzidos de falhas, mas beneficio FPS pequeno e sem melhora de stutter; expansao arquivada/adiada em build/research/fetch-large-deferred-20261018, fonte/flags removidas do codigo ativo. Nao houve verificacao real dedicada10mil para512 nem instalacao no portatil.128buckets/2vias originais restaurados, build PASS. Nao somar percentuais com outros avancos.
- Nova rota normal stutter-counts-route-20261018 em andamento: codigo validado+contadores batch, sem detalhamento por pacote, sem verificacao fetch extra e sem amostrador/captura dentro da janela. Proximo passo analisar draw/dispatch dos batches nos picos reais. Todos os candidatos de tuning desta rodada removidos do codigo ativo; mantidos apenas diagnosticos limitados e testados. Nenhuma nova correcao de stutter provada nesta sessao; nao marcar objetivo concluido.
- Rota final concluida exit0/163checks sem foco:38.882eventos/zero descartes;3.383intervalos/90,121s,37,54FPS,p95=33,96,p99=50,06,p99,9=83,14,max133,30ms;35>50ms/2>100ms. Captura conferida:personagem em pe/HP90,fogo/desfoque preservados. Contexto difere da primeira rota HP100; NAO reivindicar ganho comparando medias dessas partidas. Stutter continua reproduzido.
- No maior intervalo133,30ms, batch134,26ms tem2.043draw/17dispatch, callback8,48ms/espera8,24ms. No116,98ms, batch109,31ms tem1.509draw/18dispatch; readback256bytes20,26ms/espera19,99ms explica so parte. Cena parada tinha medianas~1.383..1.388draw/batch. Quantidade maior contribui em alguns picos, mas nao explica tudo; tempos sao wall e incluem esperas. Relatorio privado analysis-stutter-counts-route-20261018.json e CSV/range correspondentes.

### Retomada depois da reserva de10%

- Nao iniciar outro teste nesta noite. Objetivo nao concluido: pausar na reserva autorizada, salvar e desligar sem forcar descarte de outros aplicativos. Monitor PID15116, caminho/PID/log em build/overnight/stutter-watch-20261018.*; ultima leitura12% antes do salvamento.
- Codigo mantido: instrumentacao limitada opt-in (commits8c4c8f0,6ae8987,3fd760a); todos os tunings desta rodada retirados. Portatil pessoal ainda c01d852/runtime SHA256 f095a67312bdb4ab71e011157c869c4ff72f3c43cb638b09089575b47199aa6d. Nao anunciar stutter resolvido nem atualizar portatil com prototipos rejeitados.
- Primeiro ler este diario/Git e verificar processos/uso. Marcador build/overnight/performance-stop-requested bloqueia novos testes; remover somente apos Davy retomar o trabalho e confirmar nova janela disponivel. Nao rearmar shutdown/limiares antigos automaticamente.
- Proximo alvo: distinguir CPU efetivamente executada de desagendamento/esperas nos batches lentos, antes de tentar outro cache. Considerar QueryThreadCycleTime opt-in por batch, mantendo ciclos como ciclos (nao converter usando GHz presumidos), teste de isolamento e sem consultas por draw. Isso e proposta, NAO implementado. Depois perfilar BindResources/GetProgram/ObtainBuffer conforme evidencia; respeitar ABI da biblioteca precompilada. Nao repetir coalescencia/lote192/cache512 sem evidencia nova.
- Evidencias e snapshots de experimentos permanecem privados em build/research. Nenhum dump/save/projeto Godzilla/copia fixa alterado; testes mudos via SDL dummy. Checkpoint final deve ser verificado antes de shutdown; diario e commit permitem retomar amanha.

### 18/10/2026 - reserva de10% autorizada por Davy
- Janela principal restante: 10.0%. Testes e compilacoes do projeto encerrados; checkpoint local solicitado. Retomar do diario/Git e dos logs em build/research. Desligamento autorizado por Davy depois do checkpoint verificado.

## 18/10/2026 - retomada: referencia Uncharted e ciclos por batch

- Davy adicionou novo projeto em third_party e pediu continuar. Identificados
  fontes Uncharted0.2.3 TAR/ZIP e pacote Windows; analisados os fontes do TAR,
  SHA2566ba36265422b656f410b69b4968f5b8c2ce9321dcb57c3a1e77c827d8d867a69.
  Sem executar o pacote nem alterar Godzilla/copia fixa. Analise concreta em
  docs/re/REFERENCIA-UNCHARTED.md: ResetBindings une intervalos apenas compute;
  comentario da referencia relata custo ao ordenar tambem draws. Carga deles
  nao equivale aos muitos draws/poucos dispatch do Kyoei. Copias delegadas ao
  worker dependem de arenas/publicacao/sincronizacao proprias; nao transplantadas.
- Reserva anterior conferida apos retomada: telemetria recente96%, pedido de
  parada falso. Marcador antigo arquivado em performance-stop-previous-20261018.txt.
  Nao rearmado monitor/desligamento da noite anterior. Nenhum teste antigo ativo.
- Implementado KYOEI_STUTTER_CYCLES=1, opt-in Windows: duas consultas
  QueryThreadCycleTime por batch12, nenhuma por draw. Stage112 e metadado
  associado por thread/end/duracao exatos; nao altera percentis nem duplica
  escopos no analisador. Amostras invalidas/regressivas/de outra thread omitidas,
  zero valido mantido. Limite/overflow do recorder preservado. Sem mudar ABI da
  biblioteca precompilada, efeitos, resolucao, simulacao ou sincronizacao.
- Testes C++ de API real/isolamento/erros/zero/pares exatos PASS; FrameStats
  normal/desligado e batch-trace (ciclos off por padrao) PASS. Analisador Python
  com/sem ciclos e traces antigos PASS. Compilacao completa PASS; aviso u8path
  ja existente, sem erro. Ultima alteracao apenas reforcou teste do default.
- Rota muda stutter-cycles-route-20261018 exit0,163/163checks sem foco,
  45.752eventos/zero descartes.2.825intervalos/90,071s,31,36FPS,p99=50,32ms,
  max117,01ms;56>50ms/2>100ms. Todos2.825batches completos tinham ciclos.
  Medianas32,196ms/46.328.322ciclos/1.642draw (independentes). Maior batch
  124,597ms,2.038draw/17dispatch,255.536.242ciclos;125.386ciclos/draw contra
  mediana27.043. Outro103,659ms/1.772draw/216.284.345ciclos. Picos tem consumo
  de ciclos elevado tambem; nao atribuir tudo a desagendamento nem isolar funcao
  culpada. Nao converter ciclos usando GHz presumidos. Proximo alvo continua
  perfilar BindResources/GetProgram/ObtainBuffer em movimento equivalente.
- Sem compilacao/captura/amostrador na janela medida; captura posterior
  inspecionada,personagem em pe/HP90,fogo/desfoque presentes. Sem A/B nem ganho
  de performance reivindicado; instrumentacao tem custo e contexto varia.
  Teste encerrado. Portatil pessoal preservado em c01d852,sem instalar diagnosticos.
  Nenhum ZIP de amigos alterado. Fontes do diagnostico/documentos versionados;
  trace/relatorios/scripts de inspecao permanecem privados em build/research.

## 18/10/2026 - trabalho autorizado ate o limite da janela de5h

- Davy pediu continuar trabalhando ate esgotar o limite de5h. Monitor privado
  work-window-20261018.ps1/PID/log iniciado na sessao atual exata, com checkpoints
  verificados a cada5min. Margem final1% ou1min antes do reset desta janela para
  salvar; sem rearmar shutdown antigo. Primeira telemetria91%, depois90%.
- Perfil novo no giro inicial stutter-turn-profile-20261018 exit0:1.000amostras,
  547incluem Draw,159BindResources,123GetProgram,57StageSpecialization;
  339folhas de espera por condition-variable,33de espera Vulkan,19em alocacao.
  Copias optional<FetchShaderData> dentro de GetProgram aparecem no alocador.
  Nao e benchmark: amostragem suspende a thread. Captura final falhou porque a
  janela estava minimizada; nao restaurada so para produzir uma prova visual.
- Candidato pequeno em src/kyoei_fetch_return.h: no retorno de GetProgram,
  mover o fetch temporario ja construido quando TODOS os campos sao iguais ao
  fetch do modulo escolhido. Caso contrario copia original. A igualdade original
  ignora size/inst_offset/data_format/num_format; a guarda nova compara tudo,
  com representacao unica dos atributos comprovada no compile. Nao modifica
  modulo, shader, parser, dados do jogo ou ABI; nenhuma busca/copia e pulada
  por presuncao. Apenas experimental ON, opt-in KYOEI_FETCH_RETURN_MOVE=1.
- Build PASS; teste dos campos omitidos/ownership/null/10.000mutacoes PASS.
  A/B mesmo processo fetch-return-ab-20261018 em andamento; controle privado
  amortizado a512consultas, ordem0/1/1/0. Sem instalar/promover antes do resultado.
- Copia fetch: ABBA36,73/38,27FPS original versus38,52/38,41candidato;
  agregado37,51->38,46(+2,53%),sem>50ms. Repeticao BAAB original31,25/36,95
  versus candidato30,80/32,20;agregado34,09->31,51(-7,59%),p99=34,85->51,61ms,
  9->32intervalos>50ms. Variacao entre blocos impede causalidade simples, mas
  nao ha beneficio consistente. Candidato retirado, fontes/patch conferidos
  por hash em build/research/fetch-return-rejected-20261018; nenhuma instalacao.
- Considerada reducao de estados Vulkan repetidos; nao implementada: bindPipeline
  ocorre a cada draw e estados estaticos invalidam os dinamicos. Referencia
  oficial consultada: docs.vulkan.org/spec/latest/chapters/pipelines.html.
  Nao pular setters/vinculos sem acompanhar as declaracoes de cada pipeline.
- Proximo diagnostico opt-in KYOEI_VIEW_STATS=1 em copia gerada de image.cpp:
  contar buscas/passos/indice encontrado,4resumos no maximo,sem relogio por
  chamada. Build em andamento. Determinar se a busca de views justifica uma
  otimizacao antes de adicionar outra pista/cache. Nenhuma mudanca no portatil.
- Diagnostico expandido para KYOEI_LOOKUP_STATS,4resumos de100mil consultas
  por tipo/thread; totals/empty/cap/isolation/disabled testados PASS. Controle
  privado so ativa no giro inicial. image-view~1,51..1,53passos/consulta,max3;
  shader-permutation~1,78no primeiro bloco e2,66..2,71depois,max42,zero misses,
  ~74..85%de indice0. Nao justifica cache de views; foco nas variantes.
  Rota exit0; FPS perturbado por coleta nao e benchmark. Depois do cap os
  contadores agora param tambem a leitura do controle, para nao prolongar custo.
- Candidato KYOEI_SPEC_LOOKUP_FILTER opt-in: filtro negativo de binding inicial
  antes da igualdade completa, mantendo excecao both-bitsets-none e a busca
  do primeiro compativel. Nao e MRU/cache de recursos. Teste10mil listas com42
  variantes,invalidas/assimetricas/unbound/first-match PASS. Compilacao e modo
  diferencial real10mil preparados; A/B ainda pendente. Sem promover/instalar.
- Filtro passou10mil comparacoes reais antes do aquecimento,sem divergencia.
  ABBA27,98->29,71FPS agregado; cauda tem picos grandes nos controles,nao
  declarar causalidade/stutter resolvido. Repeticao BAAB em andamento.
- Preparada alternativa independente para custo do Scope de diagnosticos
  desligados (11folhas no perfil): guarda/destrutor inline,Begin/Finish fora
  do caminho inativo,mesmos campos/layout/relatorios. So rascunho privado em
  build/research/scope-guard-20261018,codigo ativo ainda original. Microteste
  Clang-O2,50milhoes de scopes:455,8/463,5ms antigo,89,9/75,8ms proposto;
  nao e FPS nem prova de ganho no jogo. Paridade4modos preparada,pendente;
  nao compilar ate acabar a medicao do filtro.
- Filtro de variantes BAAB37,12->36,47FPS(-1,74%),p99=34,42->34,81ms,
  2->3intervalos>50ms. Beneficio nao consistente com ABBA; fontes/CMake
  arquivados e conferidos em spec-lookup-rejected-20261018,alvo/flags retirados
  do codigo ativo. Busca original restaurada. Contadores limitados mantidos
  por enquanto como diagnostico experimental,desligados por padrao.
- Scope: paridade reference/candidate nos modos disabled/streaming/detailed/both
  PASS. Preparada gravacao A/B no MESMO executavel com wrapper temporario de
  duas implementacoes; escolha preservada por vida do objeto. Teste especifico
  de troca de modo antes do destrutor PASS (membro ativo correto da union).
  Controle lido somente no batch e no maximo4vezes/s,transicoes com QPC no log;
  nenhuma leitura de arquivo por draw/pacote. Build e4modos do wrapper PASS.
  scope-guard-ab-20261018 em andamento. IMPORTANTE: src/kyoei_streaming_trace.h
  esta TEMPORARIAMENTE na versao A/B; CMake chama ScopeGuardControl::Refresh.
  Depois de medir,remover wrapper/controle/call e restaurar candidate.h ou
  reference.h de build/research/scope-guard-20261018 conforme resultado.
- Monitor atual atualizado/reiniciado e conferido: janela54% recente,mesmo
  deadline. Em1% ou1min antes do reset,escreve stop,fecha SO testes cuja origem
  e build/kyoei.exe com --user em build/research,e faz checkpoint verificado.
  Sem shutdown do PC e sem tocar nos aplicativos de Davy. Nao rearmar regra10%.
- ScopeABBA38,338->38,592FPS(+0,66%),p99=34,38->34,55ms,7->2>50ms,
  max52,34->61,15ms. Variacao entre blocos,sem ganho conclusivo/stutter.
  ScopeBAAB em andamento. Funcionalidade dos4modos e lifetime do wrapper PASS.
- Rascunho separado de guarda curta para KyoeiFrameStats::Work,apenas privado
  em frame-guard-20261018: Begin/Finish fora da guarda inativa,amostragem1/128
  e contagens exatas preservadas. Mesmos testes original/candidato normal/off/
  batch/cycles PASS. Micro50milhoes: off634/606ms->178/177ms; sampled670/661ms
  ->229/227ms. Nao e FPS. Ainda NAO aplicado ao port,nao misturar com ScopeAB.
- ScopeBAAB38,978->39,876FPS(+2,30%),p99=34,29->34,33ms,nenhum>50ms;
  ABBA anterior+0,66%. Ambos logs confirmaram todas as transicoes nas janelas,
  zero descartes/exit0. Cauda/60FPS nao resolvidos; ganho modesto nessa vista.
  Mantida apenas guarda curta simples; wrapper/union/controle/call retirados.
  Contadores temporarios de lookup tambem arquivados/conferidos e retirados,
  inclusive copia extra de image.cpp,para nao deixar custo de compilacao/codigo
  sem uso. CMake atual so acrescenta teste funcional da guarda. Build e4modos
  finais PASS. Rota scope-guard-final-route-20261018 em andamento,sem instalar
  no portatil. Metodo/numeros em docs/re/STUTTER-TRACE.md.
- Rota final Scope concluida exit0,163/163checks sem foco,zero descartes;
  3.113intervalos/90,205s,34,51FPS,p99=50,16ms,max116,33ms;48>50ms/1>100ms.
  Captura inspecionada,em pe/HP100,fogo/desfoque presentes. Diferente da rotaHP90,
  nao comparar medias como ganho. Guardas finais e4modos testados,sem wrapper.
  Stutter continua; portatil pessoal nao atualizado com este conjunto diagnostico.
- Guarda de streaming final salva em4740a38. Proximo experimento agora ativo
  TEMPORARIAMENTE: src/kyoei_frame_stats.h contem duas implementacoes deWork
  com wrapper/union e escolha capturada. CMake chama FrameGuardControl::Refresh
  ANTES de criar Work12,para manter draw/dispatch da mesma implementacao no batch.
  Teste lifetime com janelas de120relatos PASS; normal/off/batch/cycles PASS no
  wrapper. frame-guard-ab-20261018 em andamento,Scope ja fixado na guarda curta.
  Ao terminar,restaurar frame-guard-20261018/candidate.h ou reference.h e retirar
  call/controle/wrapper do codigo ativo. Nada instalado no portatil.
- Work com estatisticas CPU ligadas:ABBA34,610->34,457FPS(-0,44%);
  BAAB35,322->35,195(-0,36%),caudas mistas. Metadados por batch conferidos,
  medianas1383..1389draw,controles reais confirmados. Nao ha ganho FPS provado.
- Necessidade metodologica: FPS antes exigia KYOEI_FRAME_STATS=1,ligando
  justamente Work/sampling. Adicionado KYOEI_PRESENT_INTERVALS_ONLY=1 para
  manter somente traceStage100/resumos de apresentacao sem habilitar Work.
  Teste120presents/zero contadores draw/nenhum scope CPU PASS; modos antigos
  normal/off/batch/cycles PASS. Guardas globais de Enabled para Work nao mudam.
  frame-guard-frameonly-ab-20261018 em andamento,run privado forca FRAME_STATS=0.
  Wrapper temporario deWork segue ativo; retirar depois das comparacoes.
- Investigacao AnyInRange so privada em bit-range-20261018: consulta de bits
  sem temporario128bytes.1milhao de casos vs original/oraculo escalar PASS;
  microtiny159/159ms->69/74ms,mixed161/173->132/124,checksums iguais. Nao
  integrado nem medido no jogo; precisa distribuicao real/prova antes de portar.

- Work somente intervalos:ABBA43,103->44,633FPS(+3,55%);BAAB42,091->42,376
  (+0,68%),p99 praticamente igual;BAAB max35,030->50,189ms,0->1>50ms.
  Controles reais conferidos,so stage100,zero descartes,exit0. Ganho variavel,
  stutter nao resolvido. Mantida guarda curta simples;wrapper/controle retirados.
- Corrigida validacao do build:Copy-Item preservou data antiga do cabecalho
  Scope,logo objetos da comparacao podiam persistir na rota antes chamada final.
  Fontes commitadas estavam corretas;atualizadas datas de ambos cabecalhos e
  rebuild dos dependentes(inclusive rasterizer/buffer_cache/texture_cache).
  Nove modos frame/streaming PASS. Rota limpa guards-clean-final-20261018
  em andamento,FRAME_STATS0/INTERVALS_ONLY1,muda,sem atualizar portatil.

- Rota limpa guards-clean-final-20261018 concluida exit0,164/164sem foco.
  Sem traceCSV configurado nesta execucao;nao atribuir p99/zero descartes.
  Nos22grupos completos de120intervalos dentro da janela:2.640intervalos,
  media aproximada29,75FPS,max117,33ms,59>50ms/2>100ms (valores arredondados
  dos resumos). Nenhum relato Work. Captura conferida:em pe/HP80,fogo/desfoque
  presentes. Contexto diferenteHP90/100 anterior,nao comparar FPS como ganho.
  Teste confirma funcionamento final;stutter segue. Build padrao OFF em verificacao.

- Limpeza:conferidos ausencia de refs em CMake/src/build.ninja e de processos,
  arquivos experimentais arquivados;removidos apenas exe/pdb/obj/CMakeFiles dos
  testes fetch_return/spec_lookup/view_stats/lookup_stats e image.cpp gerado
  do diagnostico retirado. Dados/saves/fontes atuais/evidencias preservados.
- Build OFF isolado inicialmente configurado sem BUILD_TYPE herdou Debug e
  falhou no link por _ITERATOR_DEBUG_LEVEL0/2(prebuilt fixo). Reconfigurado
  RelWithDebInfo,igual ao build principal;nao alterado codigo para mascarar isso.

- Build isolado OFF/RelWithDebInfo concluido85/85,exit0. Nove modos funcionais
  frame/streaming tambem PASS nesta configuracao. Build principal ON e rota
  limpa ja validados. Portatil permanecec01d852;nenhum teste aberto.
- Perfil existente do giro consolidado em host-profile-turn-summary-20261018.txt:
  1000amostras,547inclusiveDraw,159BindResources,155GetGraphicsPipeline;
  339folhas em espera de condvar,19operator==,11Scope antigo. Amostras nao
  isolam picos nem sao porcentagemCPU. Proximo passo:perfil dirigido aos batches
  longos com ciclos elevados/mesma trajetoria;nao repetir filtro de variants,
  fetch-return ou fusao draw sem evidencia nova. AnyInRange segue so privado.

- Encerramento da janela:avancos funcionais5c504d0(ciclos),4740a38(Scope),
  4ee1878(Work/intervalos). Build principal ON/rebuild limpo,OFF/RelWithDebInfo
  e nove modos em ambas configuracoes PASS;rota final exit0. Sem wrappers A/B
  ativos,sem testes abertos;portatil pessoal preservado. Stutter/60FPS pendentes.
- Removido build/guard-off temporario apos verificar processos/dependencias:
  158.334.931bytes;logs de configure/build/testes preservados. Tambem removida
  somente third_party/Bloodborne-Windows (1).zip,121.908.719bytes,identica ao
  original preservado;SHA256D2DF6AA37530F0D21F577C5CC05BD3366B1F95E8F402B96CAB89BBD8C8A219E5.
  Nenhum arquivo do dump/save/trabalho unico apagado.
- Para retomar:ler docs/re/REFERENCIA-UNCHARTED.md e STUTTER-TRACE.md. Priorizar
  perfil dos picos em movimento,nao repetir atalhos rejeitados. Para medir FPS
  sem Work:FRAME_STATS=0,PRESENT_INTERVALS_ONLY=1 e STUTTER_TRACE definido no
  processo ANTES de iniciar. Registrar foco/rota/HP/transicoes;nenhuma compilacao,
  captura ou profiler dentro de janelas comparadas. Perfil separado nao e A/B.
  Ao copiar cabecalhos experimentais,tocar data e confirmar objetos recompilados.

- Revisao final da hipotese AnyInRange:refs locais Bloodborne0.51/GOW3/GR2/TLG/
  ISS ja usam consulta direta em bit_array.h/RegionManager. ISS inclui AVX no
  miolo e cpu_any atomico,com estrategia propria de locks. NAO importar esse
  atalho atomico nem retirar locks do Kyoei:MemoryTracker atual protege CPU/GPU
  com scoped_lock. Chamadas existem por buffer de vertices/indices/ObtainBuffer,
  mas frequencia/distribuicao/tamanho ainda nao medidos no Kyoei. Sem isso,
  microteste privado nao justifica integrar. Rascunho escalar pequeno preserva
  caminho original nos ranges grandes;nenhum AVX novo/requisitoCPU acrescentado.

- Janela atingiu reserva1% as17:42;monitor escreveu performance-stop-requested,
  sem shutdown. Nenhum teste aberto. Guia PORT-NATIVO agora explicita
  CMAKE_BUILD_TYPE=RelWithDebInfo,evitando Debug incompativel com prebuilt.
  Encerramento salvo em commit/checkpoint verificado;continuar so em nova janela.

- Retomada de18/10:diario,REFERENCIA-UNCHARTED e STUTTER-TRACE lidos;Git limpo
  no inicio. Terminal isolado falhou na inicializacao;leituras/testes via
  PowerShell explicito fora do isolamento funcionaram. Nenhum jogo iniciado.
- Analisador agora lista batches inteiros por duracao/ciclos,conservando
  limites monotonicos/thread/draws/dispatches para perfil dirigido. Testes
  cobrem limites,diagnostico,metadados exatos/ausentes,thread diferente e zero
  draws. Coleta existente reanalisada:2.825batches com ciclos,metricas de
  apresentacao preservadas;maior124,5966ms/255.536.242ciclos. Sem ganho FPS.
- Lacuna concreta:amostrador privado antigo nao registra QPC por SAMPLE;
  pilhas antigas nao podem ser ligadas aos picos por ordem. ID Windows tambem
  nao equivale automaticamente ao identificador C++ do trace. Documentado
  requisito de QPC/contexto/tempo de suspensao e identificacao antes de nova
  coleta dirigida. Renderizador/portatil/dump/saves preservados;stutter pendente.

- Retomada noturna de18/10 a pedido de Davy:perfil privado com QPC por amostra
  e tempo de suspensao compilado. ID Windows relacionado ao hash C++ por FNV1a
  dos4bytes,conferido no STL local. Primeira coleta terminou nas maquinas,sem
  fogo;maior batch103ms contem73ms de suspensao do profiler,nao atribuir ao jogo.
- Coleta stutter-timed-overlay-20261018:3.000pilhas,36.145eventos/zero descartes,
  exit0. Fogo/HP80/personagem caido na captura. Suspensao mediana2,92ms/max54,63;
  FPS do profiler nao e benchmark. Nos maiores batches houve40pilhas em espera
  de readback(763,7ms),21em DestroyAllocation/FreeDedicatedMemory(577,3ms),14em
  CreateAllocation(357,5ms). Hipotese concreta de custo do driver/alocador,nao
  prova isolada de causa de todos os picos. Diálogos integrados usados nos testes
  para evitar cancelamento da lista nativa/minimizacao. Saves montados read-only.
- Candidato BDA:retirar exigencia dedicada forcada,opcional no build ON.
  Teste real Vulkan32buffers:32alocacoes dedicadas vs1compartilhada;enderecos
  unicos,fill/copy GPU,conteudos vizinhos e enderecos sobreviventes PASS.
  Quatro rotas sem profiler/Work,ordemABBA,exit0/zero descartes:
  A1 39,55FPS/max469,02ms;B1 28,37/max400,22;B2 31,44/max103,16;A2 34,91/max101,81.
  A1 terminou nas maquinas/HP100,B1noutra vista/HP80,B2/A2na mesma calcada/HP80;
  foco variou. NAO atribuir diferencas inteiras ao candidato. B2/A2p99=50,40/50,18ms,
  50/47intervalos>50ms e2/2>100ms. Beneficio nao demonstrado;flag/candidato
  RETIRADOS do codigo ativo. Patch/teste/relatorios privados preservados.
- Instrumentacao VMA privada TEMPORARIA em CMake,buffer/image gerados e
  build/research/resource_trace_20261018.h. Mede diretamente criar/destruir
  buffers/imagens(stages220..223),sem suspensao nem timing por pacote/draw.
  resource-route-20261018 em andamento;retirar injecoes privadas e reconstruir
  ao concluir. Nada instalado no portatil nem alterado no Godzilla/copia fixa.

- resource-route-20261018 terminou exit0.5.806chamadas VMA,3.135na janela:
  1.553criacoes de buffer(269,61ms)/372liberacoes(131,59ms),603criacoes de
  imagem(10,97ms)/607liberacoes(2,18ms). Maiores batches112,74/108,69/95,42ms
  incluem182/180/192chamadas,uniao temporal25,40/26,70/26,21ms. Maior chamada
  individual3,33ms. Outro batch83,84ms nao tem VMA. Logo,recriacao em volume
  explica parte dos primeiros picos,nao todos;proximo passo medir merges/
  expansoes do BufferCache antes de mudar heuristicas. Sem profiler suspensivo.
- Todas as injecoes VMA privadas retiradas,CMake igual ao HEAD,rebuild limpo
  PASS. Subalocacao BDA rejeitada tambem ausente. Evidencias/patches privados
  preservados;nenhuma melhoria de FPS declarada nesta rodada.
- A pedido de Davy,iniciado teste da traducao natural do Claude em worktree
  G:/kyoei-toshi-natural,commit7d426f9 congelado em copia privada.3.130EN
  alteradas em9recursos(201..208 e item_data),13.260entradas/135recursos
  validados por reinsercao/tokens/nomes. Fonte principal cobre todos93caracteres;
  outras fontes nao cobrem e-agudo de cafe:conferir quando essas falas aparecerem.
  Pilotos de abertura/rua inalterados pelo Claude. Previa privada independente
  runtime-natural-test-7d426f9-20261018,hash9955ca6a811693a939ad3d88ad15a33be20f6f79cf2702d6d3418c832d21b160.
  Nao ativada no manifesto principal nem mesclada ao trabalho do Claude.
- Davy pediu novos atualizadores para os amigos. Build isolado de producao
  OFF/RelWithDebInfo PASS77/77. Gerador aceita --runtime para empacotar esse
  executavel sem substituir o binario de teste. Windows/Linux-Wine20261018
  gerados,cumulativos para original/15/16/17/17b.3testes gerador+3Windows+
  11Linux/WSL PASS;10cenarios com ZIPs reais PASS(preflight/aplicar/reaplicar/
  hash/backup/save/prefixo). Hashes e instrucoes em ATUALIZACOES-AMIGOS.md.
  Somente runtime;traducao natural separada;60FPS/stutter ainda pendentes.
- Smoke final com o executavel extraido do ZIP Windows: save001 carregado,
  rua renderizada, HP100, prompt de teclado e objetivo completo no minimapa.
  Delta privado natural+UI aceito e 43 pares de prompts carregados. Teste mudo,
  sem substituir o runtime pessoal. Captura update-smoke-scene-20261018.png.
- Revisao natural: confirmadas visualmente na loja as falas alteradas do homem
  escolhendo bento (Stage201 p0/k325,k328), sem corte, incluindo contracoes.
  Evidencia natural-store-verified-7d426f9.png. Nao equivale a revisar todas
  as cenas201..208. Previa compacta privada de2.532.599bytes,62recursos,
  leitura virtual verificada;nao distribuida aos amigos. Previa integral
  mantida como referencia do teste visual original;manifesto publico intacto.

### 18/10/2026 - desempenho, reserva de 5% e hipotese de shaders
- Davy confirmou: atualizadores18/10 sem traducao natural. Pediu continuar
  desempenho e parar imediatamente a5% restantes da janela5h, salvando estado,
  sem pedido de desligamento. Leitura local atual18% no inicio;monitor privado
  performance-reserve5-20261018 consulta a cada5s e fecha so testes do projeto,
  escreve marcador e checkpoint. Limite antigo1% substituido nesta execucao.
- Hipotese de Davy: tela de compilacao completa, pois depois do inicio o jogo
  fica fluido. Conferido WarmUp existente: prepara pipelines conhecidos antes
  do guest e9conversores internos. Coleta resource-route ja pre-carregara703.
  Nas janelas90s resource-route e stutter-cycles-route:zero mensagens novas
  de compilacao,zero eventos6/7/8/19/20/21>=8ms,mas batches112,74/124,60ms.
  Nao prova ausencia de qualquer custo interno do driver/compilacao curta;
  mostra que uma tela isolada nao tem evidencia de resolver todos os picos.
- Sonda TEMPORARIA no CMake gerado,sem alterar copia fixa:CreateBuffer230,
  ResolveOverlaps231,alocacao232,JoinOverlap233,metadados234,Register235.
  Stage230 address/bytes pedido original;234 address=numero de overlaps,
  bytes=tamanho final. Build PASS. Rota buffer-growth-20261018 em andamento,
  muda,mesmo en16/cache de teste. Remover hooks e rebuild antes de encerrar.
- buffer-growth-20261018 concluiu exit0/55.411eventos/zero descartes.
  Janela90s:1.918criacao,375,92ms total;363,91ms na alocacao(96,8%),
  busca0,522ms,join2,46ms,registro6,69ms.1.558sem overlap,233com1,126com2,
  1com5:nao ha base para baixar limiar de stream apenas por causa de merges.
  Maiores batches126,13/124,10ms incluem28,49/24,00ms de criacao;resto ainda
  nao explicado. Outro62,73ms sem nenhuma criacao. Nao e benchmark de ganho.
  Captura posterior e foco163/163ausente registrados;sem comparacao de FPS.
- Segunda sonda buffer-sync adiciona SynchronizeBuffer240,UploadCopies241,
  ObtainBuffer242,ObtainBufferForImage243,RefreshImage244,FindImage245,
  apenas eventos>=0,1ms. Build PASS;rota buffer-sync-20261018 em andamento.
  Patch privado salvo como buffer-sync-instrumentation-20261018.patch;
  CMake fonte ja RESTAURADO ao HEAD durante execucao (nao muda processo
  carregado). Build principal precisa rebuild limpo quando o teste terminar.
- buffer-sync-20261018 concluiu exit0,76.999eventos/zero descartes.
  1.516criacoes298,13ms;alocacao288,71ms. Maior batch126,35ms:uniao conhecida
  86,94ms,criacao28,35ms,refresh31,36ms,video-label14,45ms(nao somar aninhados).
  Batch90,15ms so0,47ms explicado. Sem compiles novos na janela. Captura vista:
  em pe/HP90,fogo/Ultraman;foco ausente163/163,nao comparar FPS com outra rota.
- Build principal limpo PASS apos retirar TODOS os hooks de crescimento/sync;
  grep em CMake e fontes gerados sem KyoeiGrowthTrace. Portatil e ZIPs dos
  amigos preservados. Nenhuma correcao de desempenho promovida nesta rodada.
  Conclusoes e proximo alvo em STUTTER-TRACE.md:Map/esperas do staging,copia de
  memoria e comandos curtos;medir ciclos para picos90ms nao cobertos.
- Distribuicao temporal buffer-sync:0..10s teve1.335criacoes/256,97ms e
  42intervalos>50ms;10..20s148criacoes/12intervalos>50ms;20..30s23/zero;
  30..60s7/4;60..90s3/1. Corrobora aquecimento percebido por Davy,mas
  movimento/vista mudam;nao e ensaio controlado frio/quente ou ganho de FPS.
  Maiores batches126/112/98/95ms aos1,54/1,42/1,31/1,64s da janela;
  pico90,15ms nao coberto aos10,37s(transicao para correr na rota).
- Estado para pausa:sem testes abertos,CMake e fontes gerados sem sondas,
  rebuild limpo PASS. Evidencias e patches privados preservados. Proximo passo
  permanece separar staging.Map/esperas,copia de memoria e geracao de comandos,
  com ciclos por batch. Nao instalar candidato nem declarar stutter resolvido.

- Pedido posterior de Davy: modo foto inclu?do em docs/PLANO.md, ap?s a
  melhoria de desempenho. Etapas: ocultar HUD/capturar, depois c?mera livre
  com simula??o pausada, avaliar exposi??o/profundidade de campo. Port do
  P.T. como refer?ncia futura; viabilidade de pausa/c?mera ainda a investigar.
  Apenas registro documental; investiga??o e testes de desempenho pausados
  pela reserva de5%, sem iniciar implementa??o do recurso.

- Por pedido explicito de Davy, portable pessoal atualizado com o runtime de
  producao do ZIP20261018, SHA256183bcfe24386fc698ac681b4bf4e36acd30ae3c511fb82e534173a9934229641.
  Baseline pessoal f095a673..aa6d conferida com manifesto;nao constava no
  allowlist dos amigos. Somente copia privada extraida do aplicador recebeu
  essa baseline conhecida;ZIPs publicaveis intactos. Aplicacao atomica com
  backup exato em data/update-backups/f095a67312bdb4ab71e011157c869c4ff72f3c43cb638b09089575b47199aa6d.exe.
  Hashes de2.061arquivos existentes de data/ preservados;manifesto pessoal
  atualizado,privacy_scan_passed=false mantido. Launcher --check exit0.
  Prova build/research/personal-update-20261018/verification.json. Partida
  aberta em build/kyoei.exe nao interrompida. Traducao natural nao incluida.
  Investigacao de desempenho segue pausada pela reserva;somente atualizacao
  expressamente pedida executada,sem novo benchmark ou alteracao grafica.


### 19/10/2026 - retomada e separacao de upload/espera
- Retomada autorizada por Davy;Git limpo no inicio. Terminal isolado falha
  antes de executar;comandos fora do isolamento funcionam. Janela renovada
  99%/semanal43%;monitor5% reativado,sem desligamento.
- Medicao privada upload-detail-r2-20261019 terminou exit0,zero descartes,
  2593batches com ciclos. Rota90s,foco ausente163/163:diagnostico,nao ganhoFPS.
  Captura conferida:em pe/HP90,fogo/Ultraman/HUD. Teste mudo,usuario privado,
  saves montados read-only. Roteiro privado corrigido com Process.Refresh;
  primeira tentativa no titulo nao usada para conclusoes da rua.
- Staging Map>=0,1ms:37chamadas/9,22ms,max1,30;Wait4/1,54ms,max1,01;
  Commit21/6,39ms. Upload2733/657,23ms,copia647/129,95ms,criacao1547/289,31ms.
  Valores aninhados/abaixo0,1ms omitidos. Maior batch117,38ms/250,16Mciclos
  tem40,38ms de uniao conhecida;outro109,64ms tem49,09ms. Espera longa do
  staging nao explica esses picos nesta rota;sem compiles novos registrados.
- Proximo alvo:tempo/contagem acumulados por batch nas fases de preparacao e
  execucao dos draws,para capturar muitas operacoes curtas sem CSV por draw.
  Preservar qualidade/locks/barreiras. Conclusoes em docs/re/STUTTER-TRACE.md;
  patch/CSV/captura/relatorios privados em build/research. Sem ganho promovido.
- Fontes geradas restauradas byte a byte e rebuild limpo PASS;nenhuma sonda
  KyoeiUploadTrace no CMake/fontes ativos. Nenhum teste aberto;portatil e
  atualizadores dos amigos preservados.54.664eventos/zero diagnosticos.


### 19/10/2026 - acumulacao de fases dos desenhos
- Continuacao autorizada por Davy;diario lido,Git limpo no inicio;reserva5%
  reativada. Sonda temporaria em copias geradas rasterizer/Liverpool acumula
 16fases por batch/thread,sem CSV por draw. Teste de nesting,retorno,contagem
  e isolamento entre threads PASS. Nenhuma mudanca na copia fixa/CMake.
- Primeira coleta lotou:38.808descartes,excluida de conclusoes da rota completa.
  Repeticao limita registro aos batches>=40ms:exit0,66.020eventos/zero
  descartes,2062batches/1234grupos completos,zero sem correspondencia/diagnosticos.
  Foco149/163ausente,captura outro angulo/HP90/fogo/Ultraman/HUD;nao compararFPS.
- Pico inicial152,47ms:103,69ms draws,61,64ms BindResources(incluindo40,85ms
  texturas/20,16ms buffers),22,38ms vertices. Maior198,29ms aos43,90s tem
  draw61,84ms e leituraGPU120,73ms(esperas aninhadas),mais label9,66ms.
  Separadas duas cargas:preparacao em muitas chamadas curtas e readback longo.
- Em andamento resource-totals-20261019:24fases,incluindo detalhes de caches;
  teste acumuladores PASS. Fontes geradas das4unidades ja restauradas,processo
  carregado ainda tem sondas. Fazer rebuild limpo apos concluir. Teste mudo,
  usuario privado. Nenhum candidato de desempenho promovido nesta rodada.

- resource-totals-20261019 concluiu exit0,60.145eventos/zero descartes,
  2425batches/552grupos completos de24fases,zero sem correspondencia/diagnosticos.
  Foco143/163ausente,captura HP100/em pe/fogo/Ultraman/noutro ponto da calcada.
  Sem comparacaoFPS com a rotaHP90. SavesSYSTEM-DATA/SAVEDATA-001 read-only.
- Pico198,11ms:FindImage3,16ms/8452chamadas,RefreshImage33,54ms/10128,
  CreateBuffer25,34ms/103,UploadCopies10,81ms/1848;tempos inclusivos.
  Readback74,65ms no mesmo batch. Busca de imagens nao e o custo principal
  desses primeiros picos;nao repetir tuning de lookup/stream sem evidencia.
- Pico195,93ms aos30,48s tem readback161,82ms/draw29,01ms. Picos152,34/150,40ms
  tem prepare111,06/119,58ms na mesma thread,draw26,07/18,50ms e label12,84/10,66.
  Proximo alvo:RefreshImage(hash,buffer de origem,detile/upload) e prepare
  (aquisicao/copia/esperas). Sem alterar qualidade,sincronizacao ou simulacao.
- Encerramento:4fontes geradas restauradas byte a byte,sem KyoeiDrawTotals
  em CMake/fontes ativos,rebuild limpo PASS. Nenhum teste aberto;sondas
  temporarias retiradas. Portatil/ZIPs preservados;nenhum ganhoFPS declarado.

- Orientacao adicional de Davy19/10:priorizar testes em1080p(monitor dele),
  adicionar no plano opcoes graficas/resolucao com impacto real de desempenho
  e compatibilidade com outras GPUs. ReferenciaPS4 como padrao,qualidade
  configuravel por escolha do jogador,sem mudar simulacao. PC do amigo:
  GTX1660SUPER/32GBRAM,CPU informado AMD1550G a confirmar. Pedido registrado
  em docs/PLANO.md;continuar investigacao de desempenho,sem pausar apos diagnostico.
- Davy confirmou o processador do amigo:AMD Ryzen5 5500GT,com GTX1660SUPER
  e32GBRAM. Plano corrigido;resultado nessa maquina ainda depende de teste real.


### 19/10/2026 - continuacao persistente e candidato de upload
- Davy pediu continuar sem parar ao encerrar uma coleta;investigacao segue
  ate haver correcao testavel ou atingir a reserva5%,sem desligamento.
- leaf-timing-20261019:73.082eventos/zero descartes,2868batches/299grupos
  completos,zero sem correspondencia. Pico137,86ms temRefresh33,81ms,
  dos quais32,57ms emObtainBufferForImage. Outro125,70ms tem46,38/41,71ms.
  Prompt/hash/detile/upload pequenos. Nesta rota,preparemax4,45ms eFlush4,42;
  esperas por quadro/fence nao chegaram0,1ms. Nao extrapolar a todas as rotas.
- source-1080-20261019 confirmou janela/saida1920x1080 fullscreen e superficie
  finalPS4 de1920x1080.512fontes>=0,1ms/455,99ms,505copiasCPU/453,90ms,
  966.737.408bytes;Map/Commit/consultaGPU sem eventos>=0,1ms. Copy-lock confirma
  lock<0,1ms,custo em memcpy:1050chamadas/550,66ms,1.420.765.696bytes;rotas
  diferentes,nao comparar FPS. Sondas na copia privada rt_memory,ja retiradas
  do CMake/fontes ativos;core fixo preservado.
- Microteste Vulkan real512MB upload/WC flags0x6:copias SSE2stream exatas,
  alinhamento/bordas/GPU roundtripPASS,sem vantagem consistente em22MB.
  CandidatoNT nao integrado. Mesma conclusao com CRT dinamico do jogo.
- Microteste Windows de duas visoes da mesma secao:primeiro memcpy22MB pelo
  backing13..14ms/5472..5474pagefaults,visao guest4,7..5,9ms/zero faults na
  maioria das repeticoes. Demonstra custo de primeira leitura da outra visao,
  nao por si so ganhoFPS no jogo.
- Candidato Kyoei em src/kyoei_upload_source.h/CMake:API dedicada so ao caminho
  de stagingCPU de imagens,apos excluir dadosGPUmodificados. Mantem lockVMA,
  memcpy e fallbackphysical para guard/noaccess/COW/private/range incompleto,
  transferencia<256KB ou KYOEI_UPLOAD_ALIAS=0. Core/Godzilla intactos.
  Teste nativo de bytes/protecoes/fronteirasPASS. Verificacao privada dentro
  do jogo:32uploads aliased iguais ao backing byte a byte. Rota saiu0,
  captura1080p conferida:HP80/personagemcaido/fogo/HUD;nao usar como benchmark.
- ComparacaoABBA em andamento,alias0/1/1/0,mesmo binario SHA256
  25054c102d4da4d7f46dc1b22c833534913dcec46f042d87f5f1aee7d2587f41,
  saida1080p e mesma superficiePS4,verificacao extra desligada. Mede todas as
  copias>=256KB(stages390original/391fallback/392alias),sem filtro por tempo.
  Aguardar4rodadas;comparar bytes/custos e conferir contextos antes de ganhoFPS.
  CMake/generatedC ja limpos do helper privado,binario de comparacao ainda o
  contem. Rebuild final necessario depois das4rodadas. Nada instalado no
  portatil pessoal nem distribuido aos amigos. Sessao da comparacao55967.
- As quatro rodadas moveis terminaram,mas capturas mostraram posicoes/HP
  diferentes:nao sao prova de ganhoFPS. Corrigido key-up apenas no roteiro
  privado seguinte,com scancode e flags de soltura;personagem parado em1080p.
- ABBA estatico completo,zero eventos descartados. FPS original17,91/12,32;
  candidato15,21/16,56. p99 original98,58/211,41ms;candidato148,99/132,43ms.
  Mesma posicao/camera/HP100,mas fase do video da TV e carga variam. Nao ha
  ganho consistente de FPS comprovado. Copias grandes custaram58,53/54,43ms
  originais e41,59/49,44ms candidato em60s,com volumes diferentes. Candidato
  agora desligado por padrao,ativacao apenas KYOEI_UPLOAD_ALIAS=1. Teste de
  bytes/protecoes repetidoPASS;sonda privada de copia retirada na recompilacao.
- Proxima coleta privada:memoria totalVulkan e volume/idade dos buffers,
  sem ativar descarte. RunGarbageCollector define limpeza mas nao percorre
  LRU;TouchBuffer tambem nao ocorre em todo uso. Nao ligar coleta sem estudar
  idade real,dadosGPUmodificados e vida de recursos:risco de thrashing.
- Pedido atual de Davy:modo foto em paralelo. Frente isolada em
  build/research/photo-mode,sem alterar core fixo;investigar capturaPNG/HUD
  primeiro,pausa/camera livre dependem de prova. Autoriza paralelismo apesar
  da ordem antiga no plano. Testes de jogo sempre mudos e coordenados.
- Sonda privada de memoria encerrou antes da cena e nao gerou amostras
  utilizaveis;nao concluir causa nem tamanho do cache por esse teste.
  Sonda removida dos gerados/binario na recompilacao seguinte.
- Piloto foto integrado:H permanece comando original/manual,F12 pede PNG
  nativo8bit antes de scaling/overlay. Corrigido na revisao o posicionamento
  do readback:Finish troca command buffer,captura deve anteceder a obtencao
  do cmdbuf usado na apresentacao. Build completoPASS. Primeiro smoke saiu0,
  mas perdeu foco15/15 e nao salvou fotos;bloqueio de F12 fora de foco
  preservado. Repeticao privada com foco realWindows em andamento.

- Foto: segunda tentativa de foco também sem captura/exit0. Sonda privada de pedido único no frame120 gerou photos/Kyoei-17924318058722041-1.png,1920x1080,tela inicial,visual conferido;log Photo saved. Isso prova readback/PNG,não F12 nem HUD em partida. Sonda removida dos gerados;rebuild limpo obrigatório. Fotos ignoradas no Git.

- Encerramento na reserva5% conforme combinado:teste foto-readback saiu0,binario final recompilado/linkado apos restaurar e atualizar timestamp do presenter. Busca confirma ausencia de photo_probe_frames/MEMORY_SNAPSHOT/verificacao privada nos gerados. Commits0817300(experimento upload opt-in) e4b3b05f(piloto foto). Performance ainda sem ganhoFPS comprovado;proximos passos:validar F12/H em janela ativa,captura de cena e retomar memoria/CPU. Nenhum desligamento.

### 19/10/2026 - retomada apos renovacao e F12 validado
- Davy autorizou continuar consumindo o restante antes da renovacao;revisao automatica bloqueou por limite de uso,sem executar a ultima leitura. Apos renovacao,telemetria100% primaria/28% semanal.
- Tentativa memory-snapshot-retry falhou antes do boot:Windows1450 na reserva de memoria,exit21. Sistema tinha cerca de6,7GiB de commit disponivel. Nao fechar apps nem mudar pagefile. Depois havia17,8GiB disponiveis,e memory-snapshot-ready saiu0.
- Durante essa coleta Davy clicou na janela e apertou F12. Novo photos/Kyoei-17924437280986637-1.png1920x1080,visual conferido:rua,personagem,fogo,Hud/mapa,semdesktop. F12 real em partida validado. HUD continua visivel;nao afirmar ocultacao/restauracao validada. Interacao/foto contaminam benchmarkFPS;usar rodada somente como diagnostico de memoria.
- Memoria na rua:device Vulkan~3,0..3,4GiB,bufferpayload~325..431MiB nesta rota. Contagem fria nao prova desuso,pois TouchBuffer parcial. Nao ligar GC incompleto por suposicao.
- Nova sonda privada agrega tempo de GetDeviceMemoryUsage no GC de buffers/texturas;mesmas consultas/decisoes originais,sem alterar qualidade. Remover e recompilar ao terminar. Fontes limpas salvas em build/research/*.budget-clean.

- ABBA saida1080/720/720/1080 concluido,hash24a59cec...,exit0 todos/zero descartes. FPS35,55/35,37/38,03/38,10;ponderados36,82/36,70. Todos29/29 semfoco,samevendingHP100;capturas conferidas. Superficie1920x1080 emtodos;nenhum ganho por diminuir apenas janela. Metodo em docs/re/SAIDA-720P-1080P.md.
- GPUtimestamps privados: primeira sonda falhou no driver(exit30) porque vkCmdWriteTimestamp e enfileirado e End precisava outra Drain antes de fechar commandbuffer. Corrigida sonda,repeticao gpu-query-fixed saiu0. Pares1/8commandbuffers,queries sem WAIT/resolucao por disponibilidade. Na janela20s:197amostras draw,max13,994ms;107>10us mediana8,605/p9511,711ms;94present mediana0,045/max0,049ms. Nao sao temposGPUporframe nem estimativa total. Sondas removidas dos gerados/recompiladas antes do HUD.
- H testado 0/1/2/3 na rua com capturas:alternaguia grande/pequeno/oculto;HP,mapa,objetivo,interacao permanecem. Corrigida descricao anterior de H como ocultacaoHUD inteiro.
- F10 nativo opt-in integrado emsrc/kyoei_photo_hud.c:hash96bytes de2f01f0,relayexistente/ABI,traversia filhosHUD,atomicrelaxedhidden0,restaurafoco/modal/acoes de dialogo/mapa. Observador separado16raizes. CompilacaoPASS;testeSDLPhotoInputPASS. photo-gate-observe chegoucidade/observou16raizes e saiu0 comHUDvisivel;nenhuma ocultacao real provada ainda. Preparando controleprivadofora de foco para fotos antes/oculto/restaurado;retirar src/kyoei_photo.h provisoriamenteinstrumentado e recompilar ao terminar.
- Previa foto concluida na rua: duas vistas antes/oculto/restaurado conferidas; HP/mapa/objetivo/interacao somem e voltam, mundo/animações continuam. Restauracao automatica sem foco conferida. Controle privado removido; rebuild limpo e PHOTO INPUT PASS.
- Sequencia F10/F12/F10/F12 com foco real Windows e sem sondas no jogo gerou photos/Kyoei-17924463386867959-1.png (sem HUD) e Kyoei-17924463417077706-2.png (HUD restaurado), ambos1920x1080/conferidos. Cobertura de cutscenes/legendas/transicoes pendente; previa permanece opt-in.
- Launcher: checkbox Photo preview em Controls, sexta linha opcional de settings (antigos continuam desligados), F10/F12 documentados e H corrigido como guia. C# compilado; preview visual conferido sem cortes. README do empacotador atualizado, sem gerar/distribuir pacote nem mudar atalho pessoal.
- Comparacao stats1/0/0/1 em1080p concluida,exit0 todos,zero descartes,sem frames>50ms:41,10/41,37/40,24/40,38FPS;ponderados40,74ligado/40,81desligado(+0,17%,sem ganho consistente). Mesma compilacao55ddcde...,60s warm/~20s medicao,mesma posicao/camera/HP100 capturas conferidas,sem foco todos. IntervalsOnly1/trace mantidos,ciclos0 emambos. Nao alterar launcher por essa diferenca; nao extrapolar para todos diagnosticos.
- Revisao foto: restaurar tambem por M(mapa),F(mensagens),E/interacao e clique esquerdo; casos adicionados ao teste SDL. Compilacao final em andamento apos concluir ABBA,sem contaminar medidas. Cutscenes/legendas/camera livre/pausa e teste1660SUPER continuam pendentes.
- Build final kyoei+test_kyoei_photo terminou0; PHOTO INPUT PASS incluindo novos atalhos/clique. Nenhum jogo de teste ficou aberto. Revisao diff/check e registro final; fontes privadas fora da compilacao. Pacote pessoal e dos amigos permanecem sem atualizacao nesta etapa.
- Davy pediu como testar: criado Testar modo foto.cmd na raiz,1080p janela,ingles/dialogos e PHOTO_HUD1 somente no processo filho. Usa executavel atual/saves normais; nao muda atalho nem audio. CheckOnly passou sem abrir jogo. F10/F12 e pasta photos informados.
- Retomada 19/10 apos limite: Davy relatou icone de mensagem/notificacao ainda visivel no F10 e pediu camera livre/pausa. Confirmado que e icone, nao celular aberto. Pesquisa em paralelo continuou autorizada pelo pedido original. Nenhuma correcao do icone afirmada ainda.
- Candidato scene integrado opt-in KYOEI_PHOTO_SCENE=observe/preview, padrao desligado: pausa nativa6f4600 no inicio3542e0; override curto eye/target/up/FOV em326300, restaurando fontes apos compositor. Threadguest/assinaturas/posse pausa e camera/gestor revalidados. CamFreeView nativa encontrada, lifecycle nao mapeado; nao instanciar.
- Build inicial/PHOTO INPUT PASS. Observador saiu0; leitura externa na cidade confirma cameraativa viewport0 table127d4f8,eye(-2,879;1,644;-77,154),target(-7,477;1,644;-77,150),FOV40. Amostras iniciais vieram de loading/camera zerada, nao erro do viewport0.
- Grupo10 UnitEffectAttach table12d2320. Correcao de analise: nativePause limpa mascara0x1(bit0) de+20; desenho355b70 exige0x2(bit1), preservada. Nao concluir sumico de efeito pela nomenclatura antiga. Prova visual ainda pendente.
- Inputs cena: ponte bloqueia botoes/sticks/gatilhos nativos desde F10; entrega movimento/camera no host,Space/Ctrl vertical,mouse/JKIL e rodaFOV. Tests SDL adicionais verificam neutralizacao e sem zoom em janela inativa. Uma falha de link no teste (stub hud_hidden ausente) corrigida; build+teste passaram. Previa real muda em perfil privado em andamento.
- Primeiro teste realscenepreview saiu0: pausa adquirida/liberada duasvezes,F12gerou8PNGs. TV/NPC/personagem/chuva ficam parados e retomam comHUD; antes/depoiscamera original manteve alvo/FOV. Contudo L/W/Space nao mudaramimagem: camera livre NAOvalidada. Acrescentada recomposicao explicita da vistaativa aposfase3542e0 porque pausaomitefluxonormal; repetir prova.
- Filtrophone integrado separadamente,opt-in KYOEI_PHOTO_PHONE1: hook324720 só omite spritesdescendentes da raizMailIconglobal1534530/table12dd330 quandoHUDhidden. Revalida raiz/árvore bounded256/depth8; semcaches deponteiro ou modificacao defila. Somente caminhoestrutural provado; atribuicao aoiconeDavy ainda pendente comnotificacaoativa.
- Build full/PHOTO INPUT PASS/PHONE MEMBERSHIP PASS em20/10: teste realdewalkerproveniencia mundo foraárvore,tipos,ciclo,badread,count/depth/work. Roteiro compose em perfil privado/mudo em andamento.
- Previa compose saiu0 com giro/avanco/subida/FOV reais. Fotos1792465439..5450 mostraram camera livre e retorno/reentrada,HUD e posicao original preservados. PNGs pausados diferem so367pixels em380..400/859..905; efeito/regiao naoidentificado,nao declararexactfreeze. Roda aprox40->34 confirmada visualmente; minimizar resultou pausa0/FOV40. Piloto permanece opt-in,10unidades/FOV15..110.
- Filtro saveMark boot antesTLSopt integrou35b810(table12d2060) comhash226+ctor160; somente desenhoUnitSaveMark,maquina/estado/saveAPI intactos. Rodada photo-save-mark-final saiu ateperfil,teclasconcluidas:PNG17924662591048052-1 azulvisivel,17924662611107410-2 oculto,17924662673342772-5 restaurado,conferidos. Log16matches exatos. NotificacaoMailIcon separada nao estavaativa; nao declarar cobertura dorelatoseforoutroicone.
- Roteiro finalassumiuEabririasave,mas capturasmostramListenpertoTV; F10extra terminouativo(paused1). Nao foi falhadesaida: ordemdeautomacao inadequadaparaesseestado. Minimizacaoexplicita dajanela exata liberou pausa0/FOV40 no binariofinal. Nao afirmar teste dialogosave ainda. Fonte leave/falhaentrada agora restauraHUD automaticamente; novo build/testesPASS.
- Testar modo foto.cmd e checkboxlauncher agora ativam scenePreview+filtros e documentamF10,WASD,Space/Ctrl,mouse/JKIL,roda,Shift,F12. C# compilou/previewconferido; CheckOnlyPASS. Pacotes externos permanecem antigos. Documentacao atualizada com limites/provas/pendencias.
- SaveMark prova final: maquina continua visivel e interativa. Corrigida automacaoE: parinstantaneodown/up pode desaparecer antespollpad; hold200ms abriu descricao nativa da maquina (save-open-20261020.png). Spacehold200ms cancelou,leituraend-state pausado0/FOV40. Nenhum save confirmado/sobrescrito. Nao declarar dialogosave completo por teste dedescricao.
- Binario finalSHA256135757c5eeb8f9a4733113573bad4142c818be3e52df9538203a43a9dafa03cd. Buildfull,PHOTO INPUT ePHONE MEMBERSHIP PASS. Testar modo foto.cmdCheckOnlyPASS; C#compilou,telaControlsconferida. Pacotesexternosnao atualizados. Terminarrota de teste pelo doneflag e confirmar exit0. Pendencias:confirmar seiconeDavy eSaveMark ouMailIcon, notificacaoativa/cutscenes/transicoes e outrosPCs. Nenhum ganhoFPS reivindicado nesta etapa.
- Encerramento confirmado: photo-save-mark-final-20261020 exit0,nenhum processo kyoei restante. Trabalho salvo em commit funcional; repositorio limpo apos registro. Nao houve desligamento do PC.
- Retorno explicito de Davy em20/10/2026: pediu SALVAR que camera fica se movendo sozinha no modo foto (bug real relatado,causa nao investigada), e que limite de afastamento perto protagonista e chato; quer camera bem livre. Prioridadeproxima: reproduzir/parar deriva com entrada neutra e revisar limite10unidades/culling/streaming paraafastamentoamplo.
- Wishlist registrada a pedido: iluminacao,foco/profundidade de campo,protagonista invisivel,filtros,exposicao,contraste,clima,hora do dia e outros ajustes fotograficos. Desejos nao equivalem a implementacao ou viabilidade comprovada. Opcoes restritas ao modo/restauraveis,logica/saves preservados. Atualizados MODO-FOTO e PLANO. Nesta etapa somente registro; nenhum codigo/binario alterado ou teste de jogo aberto.
- Davy informou3% semanal e pediu continuar vendo. Rodada curta focada na deriva,semabrirjogo: PhotoControls nao tinha deadzone; valores127/129 perto centro128 viravam movimento/rotacao permanentes. Teste novo com127/128/129/120/136 falhou antes emlinha67,confirmando caminho reproduzivel.
- Correcao: zona morta16 emcadaeixo do pad na CAMERA FOTOGRAFICA,reamplitude suave fora dela,extremos preservados. Mouse e controles normais intactos. Buildkyoei+testPhotoPASS; teste inclui neutralidade e extremos/mouse/neutralizacao nativa. Executavel atual recompilado. Causa do relato deDavy ainda precisa confirmacao em hardware/partida; nao declarar eliminacao de toda deriva. Limite10unidades e wishlist continuam pendentes. Documentacao atualizada.
- Davy pediu continuar: removido limite radialfixo10unidades da camera fotografica; movimento so rejeita coordenadas nao finitas. Log de saida agora informa distancia da entrada para provar afastamento. BuildkyoeiPASS. Teste privado/mudo photo-free-range-20261020 em andamento: S12s/subida3s/giro/F12/retorno; aguardar imagens/log e exit0 antes declarar validado. Sourcescene alterado, ainda nao commitado. Controle normal/save sem alteracao.
- photo-free-range-20261020 terminouexit0 aposdoneflag. Primeiraautomacao perdeuprimeiroplanoantesF10; repeticao detectoucursoralteradodurantevalidacao e NAOclicou. Nenhum teste de distancia efetivamente executado,nao afirmarimagemlongedistanciaconferida. Protecoes defoco preservadas,semnovatentativa de forcarentrada. BuildPASS,limiteretirado como pedido,documentadoexperimental/validacaopendente. Sem jogo restante. Proxima prova: deixarjanelaativaeafastar>10unidades,conferirrender/culling/retorno. ListaefeitoscontinuaemMODO-FOTO.

- Retorno de Davy em20/10: camera estavel e afastamento funcionando bem em
  partida. Confirmacao humana registrada; nao substitui cobertura de outras
  cenas/culling/streaming. GPU RTX2060 observada47%,5648MiB,99,61W,74C em
  amostra nvidia-smi da placa inteira; counters porPID nao forneceram leitura.
  Nao atribuir todo uso/barulho ao jogo nem diagnosticar origem do ruido.
- Continuacao autorizada: F11 candidato para ocultar somente desenho do
  UnitPlayer pelo hook35b810 existente; assinatura ctor/thunk antesTLS e
  restauracao em saida/perdafoco. Teste callback real passou com NPCs
  preservados/defaultvisivel/restore/assinatura indisponivel. Prova visual
  de corpo/acessorios/sombra/reflexos em partida ainda pendente.
- Novo pedido: interface propria e comandos de controle bem definidos,
  referencias Spider-Man/Insomniac,TLOU2,DeathStranding2,GhostofTsushima.
  Integrada barra hostImGui com comandos/estado, foraPNG; F9/West alterna
  ajuda. View/Share entra/sai,sticks mover/olhar,gatilhos altura,L1/R1 rapidez,
  dpadzoom,SouthPNG,Northplayer. UnitPhoto passou altura/velocidade/deadzone/
  zoom/neutralizacao e foco; ImGui layout720/1080 teclado/controle passou
  apos acomodar primeiro frame de autosize. Controle fisico/partida pendentes.
- Davy acrescentou distancia focal da lente a wishlist. FOV existente nao
  implica milimetros calibrados; definir equivalencia de sensor antes de
  exibir valores. Foco/DOF/clima/hora/iluminacao/filtros ainda sem implementacao.
- Jogo de Davy nao foi fechado nem alterado pelo agente. Processo deixou de
  aparecer antes do link normal; build atualizado sem abrir outro jogo.
- Fechamento desta etapa: buildkyoei final exit0; PHOTO INPUT, PHOTO PLAYER e
  OVERLAY PASS; launcher1080CheckOnlyPASS; diffcheckPASS. Nenhum jogo de
  teste aberto nesta rodada. Proxima abertura pelo cmd recebe candidato;
  nao declarar validacao visual/F11/controle fisico concluida. Pacotes
  externos permanecem antigos, sem alegar ganho de desempenho.

- Davy autorizou continuar sem acompanhar testes, pediu aproximar dos modos
  foto citados (goldstandard) e descobrir pelo menu de pausa. Nao solicitar
  validacao humana para cada etapa; testes separados/mudos permanecem regra.
- Implementado candidato de lente virtual24mmaltura (FOV15..110), roll,
  velocidade0,25..5x e painel selecionavel por setas/dpad; R/R3 restaura lente,
  roll, grade e velocidade. Nao alegar lente fisicamente calibrada do PS4.
- GradeSDR: exposicao-3..3EV, contraste0..2, saturacao0..2, Original/PB/Sepia/
  Frio/Quente. Passagem hostVulkan amostra apenas frame de apresentacao; sem
  modificar imagem guest. PNG recebe gradeCPU linear equivalente. Neutro
  preserva bytes; foraFoto caminho normal. Shaders proprios GLSL compilados
  por glslc do VulkanSDK (novorequisito Windows), SPIRV derivados em build.
- Build inicial corrigiu palavra GLSL reservada filter e construtoresVulkan
  desabilitados (usar designatedinitializers). Build final passou; testes
  INPUT,PLAYER,SETTINGS,OVERLAY passaram. Nao rodar duas builds simultaneas:
  tentativa de segundo target durante primeira causou recompactionpermission;
  regeneracao sequencial seguinte resolveu, sem apagar arquivos do usuario.
- Rodada privada/muda photo-adjust-20261020 saiu0. Exposure0,5EV e PB vistos
  em host e PNG: comparacao top700linhas (foraajuda), erro medio0,0611/0,0356
  por canal8bit, p99=1 emambos.6PNGs1920x1080. Inicio foi cedo enquanto mapa
  ainda carregava; primeira captura tem culling/loading incompleto, NAOusar
  base/reset como prova de identidade. Repetir apos PROFILE READY.
- Prova real rejeitou F11 original: painel mudava mas personagem permanecia.
  Causa: cena usa UnitMrPlayer2 table12b3140, derivadoUnitPlayer (ctor8f7bc0
  chama9fccf0), nao tabela base12cba00. Lido em grupo1 da cena privada. Novo
  filtro reconhece essa classe com hash96bytes22b1184494d9a34e; teste estatico
  garante tambem classe derivada/assinatura indisponivel. Prova visual repetida
  em photo-adjust-final-20261020 apos recompilar; ainda em andamento.
- Pesquisa menu: UnitHudPause descriptor153d6d0/ctorae3660/table12e1520,
  rotinasae2e90/ae2f40/ae2f90/ae3220. Objeto existe em grupo12 mesmo fora do
  menu; nao usar so existencia/pausa global para exibir opcao. Ainda sem
  entrada implementada; preservar menus/dialogos e posse de pausa original.
- photo-adjust-final e photo-grade-final encerraram0, sem fechar processo de
  Davy. Lente39mm/FOV34,2 e roll10graus conferidos, R voltou lente33mm/FOV40/
  roll0. Segunda rodada reset erro medio0,0000233 por canal vsbase. Contraste
  passou a curva logaritmica emtorno18% para preservar sombras, teste dedicado.
- Terceira rodada grade-final repetiu preview/PNG com contraste suave e
  filtros; detalhes em relatorio privado. Uma tentativa de screenshot falhou
  porque destino ja existia (protecao correta); repetida com nomes novos e
  foco resetado por minimizar/restaurar somente janela privada verificada.
- F11 continua reprovado visualmente:16matches exatos UnitMrPlayer2 omitidos
  no35b810 e corpo ainda visivel. Rejeitada hipotese de bastar derivacao;
  desenho real usa outro componente. Nao declarar invisibilidade pronta.
- Investigacao seguinte achou binding setVisible682a50: resolve personagem e
  chama276ac0 sobre componente+6c0. Setter67bytes controla visibilidade+48 via
  virtuais88/90; corpo do protagonista privado tem componente table1254138,
  visibilidade1. Novo candidato deve preservar/restaurar esse estado no thread
  guest, validar dono atual e nunca alterar flag de NPC generico.

- Davy autorizou trabalhar ate5% RESTANTES DA JANELA DE5HORAS, depois salvar,
  encerrar testes e desligar imediatamente. Confirmou explicitamente que nao
  e limite semanal. Meta ativa criada; telemetria desta thread token_count
  possui primary window300min. Leitura55% depois54/48/45/41restantes. Check
  privado build/overnight/check_photo_usage.py exporta apenas metadados de
  quota, sem credenciais/mensagens; nao usar usage-current antigo como atual.
- Skill OpenAI Docs lida para monitoramento; documentacao oficial app-server
  consultada, sem instalar integracao nem ler autenticacao. Snapshot local
  recente da propria thread foi suficiente para acompanhar esta janela.
- Novos pedidos: lente AJUSTAVEL com instrucao explicita na HUD; filtros
  criativos de todo genero kaiju, liveaction/anime, nao so1954. Prioridade
  menuPAUSA caiu para DEPOIS de foco/DOF, luzes posicionaveis estiloSpider-Man,
  clima e horario. Atualizado plano pratico; nao aguardar Davy olhando teste.
- Candidato de visibilidade276ac0 REPROVOU: componente1254138 identificado
  por RTTI como GxModObjectHit, binding682a50 era de UnitMrE009, nao jogador.
  Flag voltou ao estado original no teste privado e rodada visibility saiu0.
  Codigo/teste/APIbridge removidos do build, copias privadas preservadas como
  evidencia rejeitada. Nunca promover hipotese de offset por coincidencia de
  layout; confirmar tipo/reflexao completo antes de nova chamada. F11 antigo
  permanece experimental e sem invisibilidade comprovada; UI explicita isso.
- Kaiju1954/TokusatsuRetro/KaijuNoturno/KaijuAnime, grao/vinheta independentes
  integrados. Gr?o deterministico por pixel nativo, mesma funcaohashCPU/GLSL;
  preset1954 adiciona marcas/riscos discretos. PNG conserva alfa e RGBA/BGRA.
  Testes de repetibilidade,cinza,alfa,limites/controles e layout720/1080 PASS.
- photo-kaiju-20261020 aposPROFILE READY, mudo/perfilprivado,8PNGs1920x1080:
  lente33->39mmvirtual ajustada viaUI; 1954/PBgrao, retro, noturno, anime e
  anime+grao conferidos. Crop estatico foraHUD/TV: preview/PNG medias0,0301/
  0,0277/0,0305/0,0108/0,0202 e p99=1. ForaFoto a cena muda normalmente, nao
  usar capturas separadas como prova de identidade. R/F10 restauram. Exit0.
- Revisao artistica: presetAnime tem faixas de cor fortes demais; suavizar
  na proxima etapa. Pesquisa nativaDOF confirmou GxUnitFilterDepthOfField
  table1252008/update252960, GxSysFilter singleton13425a8, enable+a0 e36bytes
  de parametros+b0..d0. Ainda pesquisa, nenhuma alteracao de foco integrada.

- Continuacao: foco nativo prototipado sob KYOEI_PHOTO_DOF=1, sem ativar no
  atalho. Unidade252960 ausente na fase; hook2458a0 de SysFilter executa,
  mas NAO produz desfoque visivel. RTTI confirmou SysFilter148f7f0 derivado
  de1341320/table127bb30. Crop original/near/far/off/reset: media0/p99=0.
  Leitura de memoria confirmou enable1 e foco/abertura/FOV; falta consumo real
  na renderizacao. HUD explicita experimental. Ambas rodadas privadas mudas
  1080p encerradas0, saves particulares intactos. Nao declarar DOF pronto.
- Lente agora segue convencao optica nativa35mm:17.5/tan(FOV/2). Substitui
  sensorvirtual24mm para coerencia comDOF; passos2mm/instrucaoHUD mantidos.
- Anime suavizado com saturacao e paleta sombra/highlight, sem posterizacao
  por canal. Screenshot1080p conferido; teste de gradiente preserva tons.
- Compilacao completa e testes input/settings/ImGui/overrideDOF passaram.
  Override valida singleton/tipo/camera, preserva exposicao e restaura saida;
  nao toca fonte da camera. Prototipo opt-in, jogo normal nao alterado.
- Luzes: GxUnitLightPoint1254ed0 presente drawgroup7, ctor2ac650. Ainda sem
  controle de fonte; clima/horario/menu permanecem pendentes na ordem pedida.
- Quota propria acompanhada29->25->24->23->21->20->19->17->16->14 restantes
  da janela300min. Continuar ate5 e desligar conforme autorizacao anterior.

- Anime suave preview/PNG: crop estatico1250x650, media0,02711 e p99=1.
  Rodada photo-dof-render saiu0; nenhum teste/jogo do agente permanece aberto.
  ControlesDOF experimentais ficam ocultos se o hook opt-in nao estiver ativo,
  evitando oferecer opcao sem efeito no atalho normal. RegressaoUI cobre
  painel normal/prototipo, teclado/controle,720p/1080p.

- Avanco salvo no commit3605033: lente/estiloanime refinados e prototipo de
  foco protegido. Controles de foco ocultos na execucao normal. Quatro testes
  finais PASS; buildkyoei completo PASS. Nenhum teste do agente aberto.
- Mapeamento inicial de luzes/ambiente documentado emFOTO-LUZES-CLIMA.md:
  point624bytes/spot656bytes; accessorRGBself+230 e update2ac830 de alcance.
  Virtual359f00 e culling, nao emissao; ainda falta coleta/posse emGxSysLight.
  UnitSkySphere localizado, mas nao comprova ciclo dinamico de clima/horario.
- Consumo DOF permanece sem prova; pesquisa encontrou tarefas246350/2485f0/
  246b70 e consumidores indiretos, sem promover novo hook nao validado.
  Quota propria chegou9% restantes da janela5horas; semanal nao foi usada.

- Pesquisa adicional de luzes: ctorGxSysLight290910->3439f0 define singleton
  1354420. Registro343c20 faz lock321250/lista2ff480/unlock3211e0. Proximo
  requisito e provar remocao simetrica e destruicao da fonte propria; nunca
  recriar singleton existente. Acrescentado documento de pesquisa.
- Workingtree de save_mark so tinha normalizacaoLF/CRLF, diff vazio; indice
  atualizado sem mudar conteudo. Arvore limpa apos commits, fora docs novos.

- Antes do limite: fontes/lente/paleta e pesquisa em commits, build atual em
  build/kyoei.exe. Workingtree conferida limpa, nenhum kyoei.exe em execucao.
  Pesquisa de luz revelou vtable0/8 compartilhadas209b60/209b70; liberacao
  nao comprova retirada de listas/jobs, pendencia explicitada na documentacao.

- LIMITE ATINGIDO: telemetria propria2026-10-20T06:06:16.955Z marcou5%
  restantes da janela300min, verificada06:06:43UTC. Trabalho interrompido
  conforme Davy; tudo salvo, testes encerrados0, nenhum kyoei.exe ativo.
  Desligamento imediato solicitado apos este checkpoint. Retomar foco visual,
  depois luzes/clima/horario e finalmente menu de pausa.

- Retomada20/10 por Davy: concluir modo foto e depois desempenho/meta60fps,
  incluindo opcoes graficas paraPCs mais fracos. Registrado noPLANO, mantendo
  padraoPS4 fiel e ajustes opcionais, sem prometer60fps em qualquer maquina.
- Falha do executor isolado helper_unknown_error contornada com execucao
  revisada fora do isolamento; leitura/codigo/build locais voltaram a funcionar.
- DOF: caminho efetivo identificado.2464f0 copia+790 para ring;247310..24749b
  chama API optica1fa70->57780->57820. Campos legados+a0/+b0 do prototipo antigo
  nao eram consumidos. Bloco+850 investigado e rejeitado: era motion blur.
- Override opt-in transferido para copia por viewport principal, assinatura
  completa validada. Preserva escala optica e restaura fonte imediatamente.
  Build completo PASS; testes DOF/input/settings/player/phone PASS.
- Prova real privada/muda transfer-20261020: foco0,74/f1 gerou desfoque visivel
  nohost ePNG1080p. Crop estatico media10,4892/canal vsoriginal; foco33,64/off/
  reset diferenca0 no crop. PNG reset identico byteabyte aooriginal. Primeira
  prova positiva de desfoque, ainda opt-in; atalho normal preservado.
- Prova dedicada de abertura ainda nao concluida: selecao herdada fez roteiro
  atuar em cores; correcao interrompida por perda de foco, sem continuar inputs.
  Nao declarar essas capturas evidencia de abertura. Segunda cena pendente.

- F11 destravado: UnitMrPlayer2+2410..2490 aponta17UnitCharPartsBase table1284d10,
  com draw proprio74ca00->35b810. Filtro exige pertença ao jogador vivo,
  nao apenas classe. Assinaturas completas construtor/thunk conferidas; teste
  cobre NPC de mesma classe, troca de dono, ausencia de manager e ciclo PASS.
- game-parts1080p privado/mudo confirmou corpo/cabelo/roupa somem/voltamF11,
  NPC e maquina preservados. Davy liberou janela por alguns minutos para
  testes visuais. Variantes/roupas/acessorios/sombras/reflexos pendentes.
- Cadeia antiga rotulada RENDER BODY (+1620/+50/+d0/table1270a88) tambem era
  colisao; rejeitada sem integrar alteracoes. Pecas provadas por slotsdiretas.
- Prova real f1/f32 executou, mas manteve desfoque excessivo: controle ainda
  incorreto. Achado seguinte: seletor optico0 e desfoque constante; modo2
  calcula optica. Apertura e distancia estavam invertidas no novo bloco.
  Preparar correcao e repetir prova antes de ativarDOF noatalho.

- Pedido adicional de Davy20/10: padrao americano no controle (X confirma,
  bola volta), incluindo icones. Preferencia de sistema ja anunciava cross;
  jogo mantem mascaras japonesas. Adaptacao propria SDL troca somente SOUTH/
  EAST antes do consumo nativo; teclado/mouse e snapshot Godzilla preservados.
- DOF corrigido: modo optico2, abertura+7d8 e distancia+7e0. Build/teste PASS.
  Partida privada/muda american-physical carregou; teste anterior physical
  caiu antes da foto no carregamento (execute0x1), nao reproduzido nesta.
- Prova real1080p: mesmo foco0,74, f1 produz desfoque; f32 nitido. Crop820x500:
  diferenca media9,70067 vsoriginal em f1;0 em f32. R restaura PNG inteiro
  identico SHA25664788a6915d81f8850f40983016e167770e0fd14951d91633365a356ba72a987.
  Segunda sequencia comparou foco0,74/33,64 comf1; alteracao visivel confirmada.
  Evidencias privadas transfer-20261020/physical-report.json e capturas/PNGs.
  Atalho Testar modo foto passa a expor foco/abertura experimentais; execucao
  normal continua sem override. Outra cena e variantes ainda pendentes.

- Controles americanos implementados no pad gerado, dicas dos dialogos e
  GPU packKYPROMP2: reconhecimento original separado da imagem americana.
  Gerados45prompts em ja/en; pares nativos, HUD, conversas, esquiva e troca
  de objetos adaptados. Teclado/mouse preservados. Launcher local exigepack2;
  gerador portatil tambem produzpack2. Distribuicao Friends antiga nao alterada.
- Build completo e teste dialogos com mapeamento SDL gerado PASS; leitor real
  dos dois pacotes PASS. Partida privada mostrou X no marcador e dicas novas.
  Contraste de glifos compostos corrigido apos folha visual. Falta nova partida
  com pacote final e controle fisico. Quatro diagramas operation_normal* ainda
  precisam adaptar chamadas/textos; pendencia emCONTROLES-AMERICANOS.md.
- Prova optica salva no969427c. Partida privada37776 fechada normalmente;
  janela liberada. Nao executar novos testes visuais sem necessidade.

- Davy apontou defeito visual especificamente em salvar/conversar. Causa:
  apagar para alpha0 e redesenhar linhas deixava fundo vazado/residuos; a
  recompressao integral tambem alterava hastes. Corrigido com mascara suavizada
  dos glifos nativos, fundo opaco original e preservacao dos blocos de mip nao
  alterados. Teste dedicado com dump proprio PASS: sete interiores opacos,
  pares nativos exatos e pixels das seis hastes inteiramente identicos.
- Pacotes finais ja/en regenerados45entradas, leitor real PASS. Partida muda/
  privada buttons-native-glyph confirmou X limpo no marcador de salvar; captura
  native-glyph-loaded.png. Entrada de teclado foi somente ao PID26884 validado.
  Diagrama completo e teste com controle fisico seguem pendentes; nao declarar
  modo foto/desempenho ou todas as texturas concluidos por este avanco.

- Nova orientacao de Davy: Claude fica com texturas traduzidas/diagramas;
  Codex registra instrucoes e segue em modo foto/desempenho/opcoes graficas,
  incluindo4K/120fps como capacidades a investigar, sem garantir desempenho.
  Repasse local em docs/INSTRUCOES-TEXTURAS-CLAUDE.md: mapeamento, diagramas,
  preservacao dos marcadores/mips, reconstruirpack da base exata e evitar
  duplo swap/custo de reconhecimento de diagramas grandes por upload.

- Modo foto: direcional repete350/80ms por tempo host, sem rajada aposstutter;
  bola/B fecha, gate neutraliza comandos ate soltar botoes/sticks/gatilhos.
  Feedback de captura so confirma aposgravacao real; falhas tambem visiveis,
  aviso5s independenteF9 e excluidoPNG. Build completo/entrada/layout PASS.
- Prova privada/muda ux-20261020: arquivo artificial no destino photos gerou
  aviso de falha; removido somente esse arquivo-prova. Duas capturas1080p
  gravadas e aviso de sucesso mostrado comF9 oculto. PNGs identicos apesar
  dos nomes diferentes no aviso host. Nenhuma foto/save normal alterado.
  Bola/repeticao/releasegate testados offline, controle fisico pendente.
- Investigacao: GetFrameLimit existe no shim, sem consumidor no driver atual;
  vblank60 participa do relogio/eventos convidados. Nao oferecer120 como mero
  aumento desse numero. FSR1 existe como ampliacao host, preserva superficie
  convidada; ainda falta prova real antes de promover como opcao grafica.

- Prova GPU semcampanha: scripts/test_graphics.ps1 verificou1920x1080 nativo,
  framebuffer3840x2160 e quatro cores lidasGPU, com FSR0 eFSR1 realmente ativo.
  Evidencia privada build/research/graphics/4k-{default,fsr1}-20261020. Nao e
  prova de4K interno nem de taxa de quadros/simulacao da campanha.
- Davy reforcou prioridade no modo foto; DLSS/desempenho/opcoes graficas ficam
  para a etapa posterior. Menu grafico em rascunho privado, nao publicado nem
  habilitado nos lancadores. Retomar luzes posicionaveis/clima/horario/pausa.

- Luzes: observado singleton realvtable1254b48 (GxSysLight);1259ba0 ebase.
  Lista privada92nos com point/spot e fontes ligadas a unidades. TiposGI do
  RTTI nao provam uso na cena. Node setter1e9290 apenasguarda ponteiro; nao
  presumir retencao. Destructor2f5d50 ret nao comprova finalizacao da unidade.
- Observeropt-in343b10 validado por assinatura inteira e testeexecutavel do
  relay5bytes/flags; preserva resultados. Partida muda antes/duranteF10
  mostroucaller35f6df/cap4/count4. Sem nenhuma modificacao em luz/lista.
  Primeira filtragem so aceitavabase e foi corrigida para a instancia real.
- Atualizacao290b60->290b80 gera cache16bytes em+58/count+60/cap+64; consumidor
  otimizado290cf0 tem assinatura maior. Proxima investigacao: cache e luz
  propria na pausa. Novas fontes/clima/horario ainda nao implementados.

- Cache de luzes: observacao opt-in290b80 validada por assinatura completa;
  trampolim6bytes testado com prologo/pilha reais em funcao sintetica.
  Teste privado/mudo game-cache.log mostrou8amostras durante a pausa F10,
  caller345322, count29/cap64, e saida normal do modo foto. Teste unitario
  PASS preservando lista, ponteiros e contadores. Nenhuma fonte adicionada
  ainda; normal continua sem hooks de luz. Foco/F11 mantem validacao da cena
  ja documentada, com outras cenas/roupas ainda pendentes.

- Pedido de Davy em 20/10/2026: ao mudar a resolucao, vida e stamina
  diminuem, mas o minimapa continua grande. Investigar a escala do minimapa
  para reduzir a ocupacao da tela, preservando legibilidade, icones e
  alinhamento. Relato do usuario; causa ainda nao confirmada no codigo.
  Etapa limitada pelo pedido de gastar menos de 5% do limite; nao ha medidor
  dessa cota disponivel nesta sessao. Diario lido, mas execucoes seguintes
  falharam na inicializacao da ferramenta (setup refresh had errors).
  Nenhuma alteracao de runtime ou teste visual realizado nesta etapa.
  Retomar comparando vida/stamina/minimapa na mesma cena em resolucoes
  diferentes e localizar seus caminhos de escala antes de alterar.
  Incluir nos proximos planos junto da finalizacao do modo foto, melhorias
  de desempenho, implementacao da traducao e mudancas de controle.

- Retomada20/10 (continua ai): terminal voltou a funcionar com PowerShell
  explicito e login=false; falha de setup recorrente exigiu execucao revisada.
- Investigacao offline da escala do minimapa: Width/Height alteram janela SDL;
  FitHud mede textos, nao escala o conjunto. Inicializador a599c0 mapeado:
  recursos normal/wide em +10/+20 e objetos +18/+28/+30 da estrutura +40.
  Atualizacao a5a560 e seletor a5fc20 usam modo derivado de13bda60; origem e
  relacao com resolucao ainda nao provadas. Nao forcar modo nem floats.
- Notas em docs/re/MINIMAPA-ESCALA.md; relatorio privado de referencias com
  hash do ELF proprio confirmado. Nenhuma partida iniciada, nenhum processo
  kyoei observado, nenhum runtime alterado. Prova720p/1080p ainda pendente.
  Preservadas alteracoes preexistentes no launcher/build_launcher e diario.

- Davy reafirmou prioridade no modo foto; investigacao do minimapa suspensa.
- Luzes: observer estendido ao seletor final290cf0, assinatura integral1457bytes
  validada. Encaminha nove argumentos SysV, preserva retorno/ordem e registra
  point/spot selecionados com limite de amostras. Normal sem hooks preservado.
- Teste nativo PASS, inclusive trampolim6bytes com tres argumentos na pilha e
  fontes/cache intocados. Build completo privado vinculado com sucesso em
  build/research/photo-mode/kyoei-photo-lights.exe. Link do kyoei.exe normal
  encontrou arquivo ocupado: PID24736 usa claude-test; partida preservada.
- Nenhuma partida nova iniciada. Falta prova do seletor em cena e luz propria;
  nao declarar iluminacao extra implementada. Retomar modo foto por essa prova,
  depois clima/horario e pausa. Launcher e texturas preexistentes preservados.

- Continuacao modo foto: prototipo source-proof cria fonte point pela fabrica
  nativa, com assinaturas completas e posicao derivada da camera fotografica.
  RGB/posicao fixos para prova; ainda sem controles HUD ou prova de emissao.
- Uma instancia reutilizavel por processo, desativada ao sair da foto; memoria
  retida para caches/jobs, sem alegar destructor resolvido. Troca do sistema
  encerra a prova. Observe e lancadores normais continuam sem fonte adicional.
- Testes com spies de fabrica/update PASS para criacao unica, camera ausente,
  saida/reentrada, fontes originais intactas e troca do sistema. Compilacao e
  link completo privado PASS. Sessao muda light-source-20261020 preparada com
  saves de teste separados. PID24736/claude-test permanece aberto/preservado;
  nenhum jogo iniciado. Proxima prova: fonte selecionada e luz visivel emF10.

- Davy autorizou testar luzes; clima/horario sao pesquisa de viabilidade, sem
  forcar. Se inviaveis, finalizar modo foto util, integrar menu e seguir.
- Prova real muda native-source-1: luz nativa propria ilumina cenario/corpo e
  PNG1080p; selecionada no renderer. Criacao1, dois ciclos ligar/desligar,
  saida0. RGB da parede sobe de34/36/35 para118/42/41 e retorna33/34/34.
- Controles integrados no painel: ligar/desligar, sete cores, intensidade e
  X/altura/Z; desligada por padrao. Prova controls-1: branca/vermelha, potencia3,
  X+2/-2 e resetR; fonte desligou antes de sairF10. PNGs/painel privados.
- Build completo/testes luzes/settings/input/layout720p1080p PASS. Atalho
  Testar modo foto habilita preview; normal/distribuicao antiga preservados.
  PIDs37992/28176 encerrados normalmente0. Segunda cena/controle fisico pendem.
- Ceu ba89d0 expoe cores dia/noite e sol/lua; instancia ativa e tempo consumidor
  ainda precisam prova. Nao confundir horario do HUD com iluminacao da fase.

- Continuacao20/10: Davy pediu2ou3luzes; ampliado para3slots independentes.
  Depois pediu posicionamento controlavel, direcao, tipo geral/direcionada e
  HUD mais clara/bonita, integrada a estetica do jogo. Referencias visuais
  Ghost of Tsushima/Spider-Man2/Death Stranding2 recebidas e vistas; imagens
  guardadas somente em build/research/photo-mode/hud-references.
- Tres pontuais comprovadas juntas em triple-1: objetos distintos, selecao
  native selected_photo_source=3 e tresPNGs. Houve interferencia de entrada/
  foco, confirmada por Davy: interrompidos roteiros; luzes desligaram na perda
  de foco. Nao usar sequencias interrompidas como prova de direcao ou cone.
- Spot nativo integrado experimentalmente: fabrica2ac8f0/vtable1254fc8,
  update2acad0, direcao vetorial+250 e cone em radianos+260/+264, conforme
  setters/consumidores auditados. Selecao real2pontuais+1spot comprovada no
  triple-2; comparacao visual isolada de direcao/abertura AINDA pendente.
- Controle direto teclado/analogicos implementado para luz selecionada,
  camera neutralizada; testes offline PASS. Sequencia visual interrompida e
  retomada ficou sem estado controlado; nao declarar prova visual desse modo.
- HUD reorganizada em4categorias Camera/Imagem/Foco/Luzes, painel lateral
  escuro/translucido com verdeagua e faixa inferior. Tab/1-4 ouL1/R1 trocam;
  cima/baixo selecionam, esq/dir ajustam. L3/Shift agora aceleram. Memoria da
  selecao por categoria e parametros de spot ocultos na luz geral.
- Build/testes luzes/settings/input/layout720p1080p PASS. Ultimo ajuste somente
  visual (sublinhado e setas no valor) ainda aguarda rebuild. Proxima etapa:
  screenshot HUD nova e prova deterministica spot/direcao/cone/controle direto.

- Fechamento da rodada HUD/luzes20/10: build completo atualizado e quatro testes
  lights/settings/input/overlay PASS; diff sem erros de whitespace.
- HUD nova vista no jogo: painel lateral translucido verdeagua, quatro abas,
  linha destacada com setas, rodape de comandos; PNG continua sem interface.
- Spot COMPROVADO visualmente em hud-studio-20261020/photos capturas6a9:
  yaw-90 aponta para parede (camera olha-X); cone60 ilumina area ampla,
  cone15 cria circulo estreito; yaw-180 retira o facho da parede. A comparacao
  anterior olhava para outro eixo, nao era defeito do shader. Direcao e cone
  nativos funcionam sem alterar luzes originais.
- Controle direto: nova tentativa parou ao perder foco apos HOLD_D, sem
  continuar comandos. Prova visual completa ainda pendente; testes offline
  preservados. Nao declarar essa sequencia como validacao concluida.
- Documentacao e atalho atualizados com categorias e tres luzes. Launcher e
  build_launcher de trabalho paralelo preservados. Clima/horario e menu de
  pausa permanecem pendentes; nao declarar modo foto inteiro finalizado.

- Rodada autonoma solicitada: parar/salvar ao restar5% da janela300min.
  Saldo recuperado dos eventos token_count/rate_limits locais, sem credenciais;
  inicio28% restantes, ultima leitura23%. Nao depender de aviso manual de Davy.
- studio-2 PID29748, teste mudo/user privado: direct-clean concluiu todos os
  passos (WASD/D e apontamentoJ); painel mostra offsetZ-0.59 e camera fixa.
  Prova de deslocamento direto em PNG; controlador fisico/segunda cena pendem.
- Melhorado inicio da spot: direcao passa a ser relativa a camera ao ligar,
  ancora preservada ao reenquadrar. Teste cobre camera voltada-X e giro90,
  sem arrastar fonte quando a camera gira. Build completo/lights PASS.

- Composicao: guias tercos/aurea/centro fora do PNG, com painel oculto ou
  visivel, acompanhando area fotografica. Enquadramentos original/cinema2.39/
  panoramico21:9/quadrado/retrato4:5/classico4:3 com barras no shader e CPU.
  Sem esticar imagem, mudar resolucao interna ou alterar simulacao.
- Build completo e testes settings/input/overlay/lights PASS. Teste ImGui cobre
  guias sem painel nos seis enquadramentos720p/1080p e limites geom?tricos;
  CPU cobre RGBA/BGRA/alpha/centro preservado/reset. Prova studio-3 PID28168:
  PNGcinema138px superiores, quadrado420px laterais, retrato528px laterais.
  Guias nao entram no PNG. PNGs e capturas permanecem privados.
- Spot relativa a camera comprovada visualmente ao ligar; controle diretoJ
  gira facho para lateral (HUD-30graus) mantendo camera. studio-2 saiu com
  camera_distance_from_entry=0.00 e fonte desativada.
- Pesquisa clima/hora: sistema astronomico ativo, mas nenhum SkySphere entre
  referencias2313listas examinadas; fatoresclima/passo zerados. Identificadores
  PARAM_WEATHER sao constantes Lua, nao prova de clima visual. Nao declarar
  impossibilidade nem expor controles sem consumidor efetivo. Detalhes em
  docs/re/FOTO-LUZES-CLIMA.md; sem escritas de ambiente na partida.
- Referencias oficiais estudadas e matriz de viabilidade em FOTO-REFERENCIAS.
  Pendencia nova reproduzida: dicas do jogo vazam no PNG. UnitHudTips e
  UnitHudLoadingTips identificados em listas; desenho/propriedade das fontes
  ainda nao provados. Preservar textos de cenario ao resolver. Menu nativo,
  segunda cena e controle fisico continuam pendentes.

- Investigacao adicional das dicas: UnitHudLoadingTips possui UnitFontEx2
  (12e1d78, drawae6820) com flags1b/5b; UnitHudTips possui UnitDummyText
  (12e1ec0, draw2e56e0) flags19. Ambos walkers2f01f0 ja bloqueados; ha
  desenho independente dos filhos. ae6820 continua apos chamar base2e56e0;
  filtrar apenas sprite324720 nao garante ocultacao. Sem novo hook ainda.
  Provas e caminho de retomada registrados em docs/re/FOTO-DICAS.md.

- Encerramento preparado junto ao limite solicitado: studio-3 saiu do modo
  foto com camera_distance_from_entry=0.00 e luz desativada; PID28168 fechado
  normalmente. studio-2/PID29748 tambem encerrado antes. Nenhum processo do
  agente deixado aberto. Build atual em build/kyoei.exe; atalho Testar modo foto
  utiliza os recursos novos. Alteracoes paralelas launcher/build_launcher
  preservadas fora dos commits desta rodada.
- Retomada: FOTO-DICAS.md (fontes independentes), depois segunda cena/pad e
  menu nativo; clima precisa provar recurso/consumidor real (FOTO-LUZES-CLIMA).
  Alternativas viaveis catalogadas em FOTO-REFERENCIAS; nao declarar recurso
  nativo impossivel so porque nao ha prova na cena201.

- Retomada21/10: Davy pediu continuar SEM abrir o jogo. Nenhuma partida aberta.
  Terminal sandbox falhou no setup; leituras/compilacoes locais funcionaram
  com PowerShell explicito/login=false em execucao revisada.
- Dicas: donos localizados estaticamente no ELF proprio/hash conferido.
  LoadingTips singleton152cbf0; Tips comuns via dono14976d0/vtable12d7708,
  campo78 aponta para raiz12d8e58. Criacao/destruicao/publicacao mapeadas em
  FOTO-DICAS.md; campo40 e recurso de layout, nao pai.
- Preparado filtro estrutural puro para fontes das duas raizes, sem instalar
  hooks novos. Reutiliza walker limitado do MailIcon; valida donos/tipos e
  rechecks. Testes sinteticos tips e phone PASS com Clang/avisos como erros;
  ambos tambem compilados/executados pelos alvos CMake. Diff whitespace PASS.
  Ponteiros de cena nao sao retidos; concorrencia/lifetime ainda pede prova
  antes da integracao. Nao declarar dicas removidas do PNG.
- Preservadas alteracoes preexistentes launcher/build_launcher/stage_cache e
  prioridade em CMakeLists. Proximo passo: lifetime/gates das duas fontes,
  depois prova visual somente quando Davy autorizar abrir o jogo.

- Continuacao21/10 solicitada por Davy: mantida proibicao de abrir o jogo.
  Gates de fontes tips preparados em src/kyoei_photo_tips.c, somente opt-in
  KYOEI_PHOTO_TIPS=1; nenhum atalho habilitado. Duas assinaturas integrais
  conferidas no ELF proprio; preparo dos dois gates antes de qualquer escrita.
  Desenho original fora da foto/falha de leitura; leitura segura via API do
  Windows, sem cache de objetos ou escritas em fontes/listas. Snapshot/lifetime
  concorrente ainda sem prova visual; recurso permanece experimental.
- Testes nativos gate/membership/phone PASS. Prova de instalacao em copia de
  memoria das duas funcoes PASS: assinatura adulterada rejeitada, relays/RX e
  reconfigure; nenhum codigo convidado executado. Relay sintetico real PASS;
  dependencia gz_memory_clamp ausente no teste antigo foi corrigida.
- Build completo do port vinculado com sucesso, sem executar kyoei.exe.
  Continuada pesquisa estatica do menu de pausa; entrada nativa ainda pendente.
  Trabalho preexistente launcher/stage_cache/prioridade/analyze_stutter_trace
  permanece separado. Nao declarar dicas removidas do PNG sem teste em cena.

- Pesquisa menu21/10: separados UnitHudPause(global153d780), UnitMenuManager
  (singleton1543dd0) e UnitMainMenu(manager+A10); tipos/tabelas/fabricas e
  assinaturas documentados em docs/re/FOTO-MENU-PAUSA.md. Mascara317c/e0 tem
  outros consumidores, nao zerar globalmente ao transferir pausa para foto.
- Menu principal possui seis entradas/curvas e limites0..5, arrays/cursor e
  tratamento especial do indice5. b303d0 faz transicao circular de categorias,
  nao e callback generico para um ID novo. Layout proprio inspecionado/hash
  registrado, sem modificar nem versionar dados. Falta provar transferencia
  de posse/animacoes antes de acrescentar linha. Nenhum hook de menu instalado.
- Elo nativo confirmado depois: update do menu chama6f4600 em b2d566 com
  paused1 e em b2e6d7 com paused0, ambos usando exclusoes menu_manager+B00.
  Repetir paused1 enquanto ja pausado retorna cedo e nao reaplica exclusoes;
  nao basta adotar pausa do menu como se fosse a pausa fotografica/exclusao12.
  F10 atual recusa pausa preexistente; protecao preservada. Proxima pesquisa:
  listaB00 e momento de fechamento/animacao. Jogo permaneceu fechado.

- Retomada21/10 apos o Windows Terminal fechar sozinho (falha de heap do
  proprio Terminal, nao do port). Commitado o trabalho de desempenho pendente:
  processo com prioridade acima do normal (KYOEI_PRIORITY=normal desliga),
  drenagem da gravacao Vulkan fora do submit_mutex (trace 304) e cache de
  estagios com 1024 entradas e indice hash. Build completo OK; testes nativos
  PASS (pc_names/png/vmem_reference exigem argumentos/endereco proprio, sem
  relacao). Ganho de FPS ainda nao medido em jogo.

- Trava30 (21/10): KYOEI_FRAME_CAP=30/20 opt-in apresenta a cada2/3 vblanks,
  vblank/tempo convidado intactos (jogo usa passo medido, modo0, minimo15).
  ABBA framecap-20261021 sem/30/30/sem, mudo, 20s parado+giro L, exit0:
  33,53/26,55/26,25/28,75FPS; >50ms 36/55/56/47; >100ms 3/6/8/6; p99
  83/100/101/101ms. Trava30 PIOROU: quadro que perde o slot33ms espera66ms.
  Esta maquina nao sustenta30 firmes no giro; travar nao estabiliza. Padrao
  continua sem trava. Foco perdido nas rodadas3/4 (29/29). GZ_VBLANK_HZ ja
  existe para testar120 depois. Proximo: perfil CPU do giro.

- Estudo offline21/10 (Davy em aula, jogo nao aberto): o Kyoei compila o
  video_core do shadPS4 comum (KYOEI_SHAD). O nucleo otimizado do bbport
  (draw pipe em2 estagios, constant ring, memos de textura/render state,
  janela de faltas, copias paralelas; Bloodborne 65->145FPS no doc
  parallel_gpu.md) esta em third_party/bloodborne_pc mas nao e compilado e e
  so Linux/AMD. gow3_pc-windows-v2.4.1 (GPL-2.0) traz o mesmo nucleo ja no
  Windows (+DLSS). Custo de trocar: 226 remendos CMake, ~125 em video_core
  (texture_cache28, liverpool19, pipeline_cache15, scheduler11, tile8,
  rasterizer8), a refazer sobre o nucleo novo. Alternativa: portar os ganhos
  um a um para a copia atual. Decisao de caminho pendente com Davy.

- Nucleo GoW3 (ramo nucleo-gow3, 21/10, Davy aprovou incluir): copia fixa
  third_party/gow3-gpu = gow3_pc tag windows-v2.4.1 (1436e7e), gpu/ do tag
  oficial (a release nao trazia shaders/opcodes.inc); manifesto
  docs/gow3-gpu.json. FSR-Vulkan (MIT, c64f093) em build/fsr-vulkan-src.
  gpu-gow3/CMakeLists.txt compila video_core+recompilador+common+shim+
  gnmdriver/videoout/avplayer com clang do Kyoei: 0 erros. Adaptacoes
  verificadas (gow3_adapt) e compat/: pthread.h vazio, cheat_hook minimo,
  ntapi.h renomeando enum, iterador std::array. Proximo: cola gz_gpu_* (bridge
  Godzilla + partes do gow3gpu.cpp; libs.h do Godzilla para resolver por
  nome), Ajm/AudioOut/Pad/janela, ligar kyoei.exe opt-in, depois refazer os
  ~125 remendos de video_core do Kyoei. Jogo nao aberto (Davy em aula).
- Biblioteca gow3_core completa (21/10): cola gpu-gow3/kyoei_gow3_bridge.cpp
  (gz_gpu_* do Godzilla + copias paralelas, NoteBackingWrite, servico kernel,
  recuperacao de falha da preparacao especulativa via gow3_setjmp/longjmp no
  gz_gpu_handle_fault), stubs do menu GoW3, configuracoes lidas de GZ_*
  (compat/core/emulator_settings.h), libs.h/sdl_window/threads do Godzilla,
  Ajm/AudioOut/pad/janela do Godzilla, Sirit do nucleo e FSR-Vulkan compilados.
  Teste de ligacao: so restam simbolos gz_* do runtime; 0 duplicados.
  Proximo: alvo opt-in no CMake principal (KYOEI_GPU_CORE=gow3) ligando
  kyoei.exe a essa biblioteca, sem os remendos do nucleo antigo; depois
  primeira partida muda quando Davy liberar.
- kyoei-gow3.exe ligado (21/10 21:50): alvo opt-in no CMake principal
  (KYOEI_GOW3_CORE, padrao ON) com todo o lado C do Kyoei + gow3_core; o
  kyoei.exe normal nao muda. Faltavam so 5 ganchos do nucleo antigo
  (autotestes e flush do cache ao sair): stubs que registram "not available".
  Ainda NAO portados os recursos do Kyoei que viviam nos objetos kyoei_videoout
  (troca de texturas/botoes traduzidos, overlay de dialogos/IME, pos do modo
  foto, HUD fit, frame stats, correcoes de detiler/readback). Ainda nao
  executado: primeira partida muda depende de Davy (estava em aula).
- PRIMEIRA PARTIDA no nucleo GoW3 (21/10 noite, mudo, rota framecap com giro L,
  GOW3_FRAME_STATS=1): logos, titulo, save e cidade (stage201) renderizados
  corretamente, ingles do runtime ok, exit0. Parado aquecendo: 51,4-55,4 FPS
  (nucleo antigo, mesma cena: 37-41). Janela medida (reset+giro): 44,2/35,0/
  38,6 FPS por janela de 5s (antigo na mesma rota: 33,5 e 28,7 de media).
  Picos do giro 50-84ms com 70-113MB de upload de imagens por quadro (texturas
  novas entrando), 0 compilacoes. Log INFO "Making range resident" em excesso
  (logging do Godzilla usa INFO por padrao): reduzir antes de medir de novo.
  Janela abre 1280x720 (tamanho do launcher nao ligado); controle fisico
  ignorado pelos scripts de teste de proposito.
- Ajustes nucleo GoW3 (21/10 noite, mudo, rota com X+Enter e giro L):
  GOW3_PIPELINE_BINARIES=0 liga pre-aquecimento em segundo plano: compilacoes
  no giro 3,3s/5s -> 0. GOW3_GC_BUDGET_MB=5000 para de descartar texturas
  (orcamento padrao 1,9GB na RTX2060). Parado 55-58 FPS, giro 39-41 (60Hz FIFO).
  Diagnostico KYOEI_UPLOAD_REPORT=1 (gpu-gow3/kyoei_upload_report.h): 7 alvos
  de render 1920x1080/1920x540/960x540 recarregados todo quadro (~2,7GB/s,
  flags GpuModified|GpuDirty), por escritas de buffer do shader 0xb9263d29
  (~7 por quadro na base das imagens). Provavel limpeza/preparo por buffer;
  recarga necessaria para correcao, nao alterada.
  Sem trava (GZ_VBLANK_HZ=0, Mailbox): parado 59,5-59,9 FPS (mediana 14,4-14,8ms),
  giro 45,0-49,1. Estagio B (gravacao de draws) 80-86% ocupado = limite atual
  neste Xeon; estagio A espera 25-39%.
- ABBA LTO (21/10 noite, sem trava/Mailbox): copia falhou na rodada2 (rodou sem
  LTO). Parado ~60 em todas (limite do monitor 60Hz; Mailbox indisponivel ->
  FIFO). Giro: sem LTO 43,7/46,6, 46,0/48,3, 43,6/47,5; com LTO 45,4/46,4.
  Sem diferenca mensuravel; LTO fica ligado (padrao do GoW3). Limpeza disco:
  apagados runtime-preview-en-15/-en-04/natural-test (substituidos pela en-16 e
  pacote natural) e build/gow3 de sonda; pacote Friends-Test apagado com
  autorizacao de Davy (jogo eram hardlinks: quase nao liberou). G: 38->55GB livres.
- 22/10 madrugada: getenv("GOW3_RENDER_RES") por draw em FixedRenderSession (via
  TemporalUpscaler::OnFrameStart) era ~10% do DrawRec no amostrador (copias
  sample_threads_*-gow3 em build/research/gow3-first-20261021). Lido uma vez:
  ABBA giro 46,7 -> 48,1 FPS, DrawRec 81-86% -> 74-81%. Commit 8ebc3a8.
  ABBA GOW3_DEFERRED_READBACK=1 inconclusivo: controles iguais variaram 40-42 ->
  31-32 FPS no giro; OneDrive com ~17 mil s de CPU e 22% de CPU ocioso no PC.
  Repetir com OneDrive pausado. STALE_READBACK (leitura 1 quadro atrasada)
  nao ligado: muda comportamento, exige comparacao de fidelidade.
- ABBA leitura adiada repetido com OneDrive fechado (22/10): TODAS as janelas
  ~60 FPS, parado e no giro (limite do monitor 60Hz, FIFO), pior quadro ~31ms;
  DrawRec ~91%. Captura final confirma giro. Sem diferenca da leitura adiada:
  fica desligada. Rodadas anteriores tinham OneDrive consumindo CPU.
- 22/10: prioridade HIGHEST para GpuCommandProcessor/DrawRec/VkRecorder testada
  em ABBA com OneDrive aberto: giro 45,3/47,3 (normal) vs 48,4/46,2 e 45,4/46,9
  (alta): sem efeito, revertida. CORRECAO: janelas de "60 FPS no giro" das
  rodadas anteriores nao sao confiaveis: a rota manda teclas por PostMessage a
  janela sem foco e o giro nem sempre acontece (ultima rodada nem terminou).
  Numero confiavel atual: parado ~60 (teto do motor), giro 45-48. Proximo:
  rota que confirme o giro (ex.: comparar capturas antes/depois ou log da camera).
- 22/10: giro por dentro do jogo (KYOEI_BENCH_TURN_FILE: analogico direito
  enquanto o arquivo existe; adaptacao do pad.cpp do Godzilla no gpu-gow3) para
  nao depender de foco. 3 repeticoes: rodada1 media "60" mas a captura mostra a
  cena de abertura (rota nao carregou o save) = invalida; rodadas 2/3 na cidade
  girando: 42,8/44,0 e 40,6/46,2 FPS. Referencia confiavel: parado ~60 (teto do
  motor), giro 43-46. Toda medicao exige conferir a captura final na cidade.
- 22/10 diagnosticos: untrack de imagens grandes raro (1-2/5s): o "vai e vem"
  de protecao e de texturas pequenas atualizadas pelo jogo, legitimo. No giro,
  GOW3_PERF_DIAG: 527 imagens criadas/5s (texturas novas entrando, "nothing
  cached"); cada registro sobe registry_generation global e invalida o memo de
  conjuntos de texturas (misses por geracao ~47k/5s no giro, 6k parado).
  Refinar invalidacao por regiao renderia ~2% (estimativa pelo doc do bbport).
  Diagnostico de chamadores de UntrackImage (KYOEI_UPLOAD_REPORT=1) mantido
  desligado por padrao.

## RETOMADA (22/10 04:2x, PC desligado a pedido de Davy: 5% da cota)
Ramo nucleo-gow3. kyoei.exe normal intacto. Ultimo passo EM ANDAMENTO (nao liga):
- kyoei-gow3 passou a usar kyoei_pad.cpp/kyoei_window.cpp do Kyoei
  (KYOEI_GOW3_PAD/WINDOW no CMake principal; include ../src no gpu-gow3).
  Faltam KyoeiOverlay::HandleEvent/UpdateTextInput: proximo passo e compilar
  src/kyoei_overlay_gpu.cpp (+ imgui vulkan backend, kyoei_overlay_barriers)
  no gow3_core e fazer os stubs Gow3Overlay::Init/Render chamarem
  KyoeiOverlay::Initialize/Render (mesma assinatura; o presenter GoW3 chama
  Render com a imagem em ColorAttachmentOptimal; Gow3Overlay::Visible deve
  devolver true quando o overlay Kyoei tiver algo a desenhar). Conferir
  BeforeDraw/AfterDraw do overlay antigo (presenter Godzilla) vs barreiras GoW3.
- Depois: texturas/botoes traduzidos (patches KYOEI_TEXTURE_CACHE), modo foto
  (src/kyoei_photo_post.cpp), HUD fit, tamanho da janela do launcher.
- Desempenho atual confiavel (rota com giro interno KYOEI_BENCH_TURN_FILE e
  captura conferida): parado ~60 (teto do motor), giro 43-46 (antigo 28-33).
  Opcoes usadas: GOW3_GC_BUDGET_MB=5000 GOW3_PIPELINE_BINARIES=0 GZ_GPU_LOG=warning.
- Pedidos novos de Davy na memoria (ui-wishlist): launcher inspirado nos outros
  ports, popups salvar/carregar/teclado e HUD do modo foto no estilo do jogo
  (Claude Design), "Sair para o desktop" voltar ao menu com som, opcao de modo foto.
- Alteracoes preexistentes nao relacionadas mantidas sem commit:
  src/kyoei_launcher.cs, tools/build_launcher.ps1 (so quebras de linha).

- 22/10 tarde: overlay Kyoei ligado ao presenter GoW3 (stubs Gow3Overlay ->
  KyoeiOverlay; driver.cpp redesenha com kyoei_overlay_needs_redraw). kyoei-gow3
  liga e roda (commit 53e5cf8). Teste mudo com as opcoes do launcher na pasta
  portatil (copia dos saves): janela 1600x900 e icone ok, titulo/menu em
  ingles, "Return to Desktop" aparece. Rota por teclas ainda cai em New Game
  (Down no menu nao pegou: conferir tempo/entrada). Falta: lista de saves no
  overlay, pos do modo foto (RenderPost/Capture), texturas traduzidas
  (KYOEI_TEXTURE_CACHE), HUD fit, demais remendos do nucleo antigo.
- Davy perguntou por "A City Sinking Beneath Giant Shadows": japones e
  巨影ニ沈ム都市 (livro do Otsuka/titulo do Stage 17), nao o titulo 巨影都市.
  Sugerido "City Sunk in Shadow" (eco do titulo); aguardando aprovacao.
- 22/10 noite: botoes ao vivo (prompts.kypack) no nucleo GoW3: gpu-gow3/
  kyoei_gow3_prompts.h + adaptacao de texture_cache.cpp/.h/image.h (flag
  KyoeiPromptImage tira imagens casadas dos atalhos de imagem limpa). Casam
  48x88 (E no "Save"), 276x320 (status) e 1920x1080 da pausa (setas do teclado).
  Exclui render target/GpuModified/Aliased. Mensagens do sistema de save/erro no
  idioma do launcher + NODATA (Load Game sem saves dava erro 0x80B8000F, tambem
  no nucleo antigo). Lista de saves e carga ok no GoW3.
- DEFEITO ABERTO nucleo GoW3: fundo do menu de pausa vira preto e depois lixo
  colorido (velho: cidade desfocada). Independe dos botoes (testado com pack
  desligado). Ao pausar, imagem 1920x1080 fmt97 em 0x1067ee0000 sobe 1x como
  "CPU-written": o jogo copia o quadro pela memoria sem readback do alvo GPU.
  GOW3_READBACK_LINEAR=1 e GOW3_READBACKS=2 nao resolvem. Proximo: achar o
  caminho da copia (DMA/CPU) e garantir leitura do alvo antes.
- Pendente sem regressao: rodape "設定の変更" das Opcoes em japones tambem no
  nucleo antigo (nao esta no pack de prompts).
- Investigacao fundo da pausa (GoW3), 22-23/10. Fatos: ao pausar surge imagem
  1920x1080 fmt97 (16 bytes/2px, HDR) em endereco variavel (0x1067e4..,
  0x1068040000), enviada 1x da memoria como "CPU-written"; nenhum alvo de
  render, escrita de GPU (watch), copia DMA >=256KB ou imagem GPU sobreposta
  nesse endereco. Logo o jogo a preenche pela CPU a partir de algo que no
  nucleo antigo esta certo na memoria. Descartados (sem efeito): pack de botoes,
  GOW3_READBACK_LINEAR=1, GOW3_READBACKS=2, GOW3_COPY_GPU_BUFFERS=1,
  GOW3_DIRECT_MEMORY_ACCESS=1, KYOEI_SYNC_READS=1 (novo, opt-in), upscaler off,
  todas as otimizacoes GoW3 off (GZ_DISABLED_OPTIMIZATIONS=0x607ffff400fffff).
  Nucleo antigo com GZ_READBACK_LINEAR=0 continua certo. Proximo: achar o codigo
  do jogo que escreve a imagem (PC da falta de escrita) e de onde ele le.
  Diagnosticos KYOEI_UPLOAD_REPORT=1 ampliados: copias DMA, leituras CPU,
  alvos de render, uploads sobrepostos, KYOEI_UPLOAD_WATCH=<hex>.
- Achado: o nucleo GoW3 liga FSR 3.1 por padrao ("Upscaler: FSR 3.1 ... (on)").
  Contra a regra de fidelidade; desligar (GOW3_UPSCALER=off) antes de usar.
- Davy viu "Return to Desktop" aparecer no titulo antes das outras opcoes no
  nucleo GoW3: corrigir o momento de exibicao.
- 23/10 continuacao pausa GoW3: imagem do fundo (1920x1080 fmt97, endereco
  variavel ~0x1067e..0x1068f..) so e usada como textura; nenhum alvo/storage/
  escrita de buffer GPU/DMA na faixa; armadilha de escrita CPU
  (KYOEI_PROTECT_WATCH + KYOEI_PROTECT_DELAY) armada na cidade nao viu escrita
  nenhuma ao pausar. Marcar destino do Resolve como GPU (KYOEI_RESOLVE_WRITTEN=1,
  opt-in) nao resolveu. Hipotese restante: o conteudo no nucleo antigo vem de
  imagem existente achada por FindImage com regras diferentes (mesmo endereco,
  outro tamanho/formato) ou de caminho de GPU nao instrumentado. Proximo passo
  sugerido: captura do quadro da pausa nos dois nucleos (RenderDoc/descritores)
  em vez de mais sondas cegas.
- Davy: "Return to Desktop" voltou a responder ao mouse no nucleo GoW3 (no
  antigo nao respondia) e aparece antes das outras opcoes.

### 22/10/2026 — mudo dos testes e FSR desligado no motor GoW3
- Testes estavam com som: `tools/mute_test_audio.ps1` rodava antes do jogo abrir a sessão de áudio e silenciava 0 sessões sem erro. Agora espera até 60 s a sessão aparecer; se não conseguir, falha (e o roteiro de teste fecha o jogo). Conferido: 1 sessão silenciada.
- `kyoei-gow3`: o upscaler temporal do GoW3 (FSR 3.1, ligado por padrão) fica desligado (`GOW3_UPSCALER=none` se não definido), pela regra de fidelidade ao PS4. Log sem linha "Upscaler"; cidade normal.
- RESOLVIDO fundo da pausa no `kyoei-gow3`: causa era `stale_readback` do GoW3 (ligado por padrão), que entrega à CPU os dados de GPU do quadro anterior. O jogo copia a cidade uma vez para o fundo da pausa e recebia preto/lixo. Agora `GOW3_STALE_READBACK=0` por padrão (deferred_readback e async_shaders seguem ligados; desligar só deferred também resolvia). Conferido: fundo desfocado como no núcleo antigo, volta à cidade normal, e as TVs da rua passaram a mostrar o vídeo (antes só o logo). Diagnósticos KYOEI_* da investigação seguem desligados por padrão; avaliar remoção na limpeza.
- "Return to Desktop" (ambos os núcleos): não responde mais ao mouse, como as opções nativas (pedido antigo de Davy); removidos hover/clique/hit-test. Entrada atrasada em até 2 atualizações do menu (mínimo do alfa recente), porque o núcleo GoW3 grava quadros adiantados e o item aparecia antes das opções do jogo; saída imediata. Sequência de capturas: aparece junto com as opções nativas. test_kyoei_title_exit PASS.
- Medição 22/10 (kyoei-gow3, cidade, ABBA, capturas conferidas): stale_readback desligado parado 54-58 FPS, girando 37-48; ligado parado 54-57 (B1 oscilou 28-56), girando 31-45. Sem perda pela correção da pausa; igual à referência anterior (parado ~60, giro 43-46). GOW3_FRAME_STATS=1 derruba o jogo para ~15 FPS (diagnóstico pesado): não usar para medir. Novo medidor leve: KYOEI_FRAME_STATS=1 no kyoei-gow3 grava "Kyoei frame stats" no run.log a cada 120 quadros (gpu-gow3/kyoei_gow3_flip_rate.h). Nesta máquina havia Wallpaper Engine, OBS e navegadores abertos.
- Modo foto no `kyoei-gow3`: F12 salva PNG nativo 1920x1080 do buffer de exibição do jogo (antes do pós-processo, do overlay e da escala da janela), com o filtro de cor do modo foto quando o painel está aberto (gpu-gow3/kyoei_gow3_photo.h, adaptação de vk_presenter.cpp). Filtro na tela (KyoeiPhoto::RenderPost) sob o overlay; ShutdownPost/Overlay::Shutdown no fim do presenter. Gravação PNG separada em src/kyoei_photo_png.h (usada pelos dois núcleos). Conferido: F10 abre o painel e esconde o HUD; foto 1920x1080 com cores certas (a 1ª tentativa trocava vermelho/azul; agora usa o formato da visão de exibição). Roteiro de teste: F10/F12 em tools/input_native.ps1; com a janela sem foco o F10 às vezes não chega (limite do roteiro). Filtros de cor na foto ainda não conferidos no GoW3.
- Davy trouxe para third_party/ pacotes dos ports de Gravity Rush 2 (gr2_pc 7d90fed) e inFamous (iss_pc 1.0), com melhorias de desempenho (inFamous a 30 FPS numa GTX 1650, segundo Davy). Próximo: estudar o que aplicar ao núcleo GoW3.
- `kyoei-gow3`: compilação assíncrona de shaders desligada por padrão (`GOW3_ASYNC_SHADERS=0` se não definido): ligada, o núcleo pula o draw até o pipeline ficar pronto (efeito novo some por alguns quadros), contra a fidelidade; o port do inFamous também a deixa desligada. ABBA com o PC carregado (GPU 27% ocupada sem o jogo, medições ruidosas): async=0 parado 42-48 / giro 35-40, async=1 parado 10-51 / giro 28-39; desligar não piorou. Repetir com o PC livre.
- Estudo iss_pc 1.0: 12 backports BP01-BP12 (common/backports.h), nenhum no núcleo GoW3. Candidato de velocidade mais simples: BP02 (FindImageFromRange só no balde da 1ª página). Corretivos de shader/barreira BP03/04/05/10/11 exigem comparação visual. Oclusão real (ZPASS_DONE) a avaliar no Kyoei.
- BP02 (shadPS4 145932bf #5240, via inFamous) no `kyoei-gow3`: FindImageFromRange procura só no balde da 1ª página (gpu-gow3/kyoei_gow3_image_lookup.inc incluído na classe pelo texture_cache.h adaptado). Mesmo resultado por construção. ABBA (exe trocado entre rodadas, PC com GPU ~26% ocupada fora do jogo): parado 59-60 nos dois, giro 45-53 nos dois; sem ganho mensurável no Kyoei, sem perda; pausa correta. Mantida por ser equivalente e mais barata em faixas grandes.
- SamplerSetMemo (port do inFamous, iss_pc 1.0) no `kyoei-gow3`: um conjunto de texturas memorizado reaproveita os samplers já resolvidos (1 hash + 1 trava por conjunto em vez de hash, trava e busca por sampler). Novo mecanismo `gow3_patch` (gpu-gow3/CMakeLists.txt): remendos em arquivo em gpu-gow3/patches/ (blocos <<<<<<< ======= >>>>>>>, cada um achado uma vez, CRLF normalizado), para mudanças grandes com ponto e vírgula. Atenção: ao remover um remendo de um cabeçalho, apagar a cópia adaptada velha em build/gpu-gow3/adapted. ABBA girando: sem memo 45,6 / 45,3 FPS de média, com memo 47,4 / 46,1 (+~1,3 FPS, ~3%, no limite do ruído mas na mesma direção nas duas). Parado ~58-59 nos dois. Imagem conferida.
- 23/10 perfil do giro (kyoei-gow3, medidor por amostragem KYOEI_SAMPLE_THREADS, 1 kHz): a placa fica ~50% ocupada girando, então o limite é a CPU, mas nenhuma thread satura. GpuCommandProcessor ~58% ocupada, DrawRec ~57% (40% esperando trabalho), VkRecorder ~57%, guest main ~58% (39% em cond_wait/cond_timedwait do runtime), threads guest 40-50%. Ou seja, as threads trabalham em fila, umas esperando as outras. Overhead do runtime nas threads guest pequeno (NtQueryVirtualMemory ~3,5% somado, memmove 4%).
- Achado no runtime (godzilla-core rt_sync.c/rt_sema.c): prazos de espera usam GetTickCount64 (passos de ~15,6 ms). Teste A/B com timeBeginPeriod(1) (KYOEI_TIMER_1MS, removido): sem ganho (giro ~43 contra ~46 FPS). Não corrigido por ora.

## ESTADO AO ENCERRAR (23/10, pedido de desligamento)
- Feito e testado nesta sessão (núcleo GoW3): fundo da pausa (stale_readback off), FSR off, compilação síncrona de shaders, mudo dos testes corrigido, Return to Desktop sem mouse e sem aparecer antes das opções, modo foto (PNG nativo 1920x1080), BP02, SamplerSetMemo (+~3% no giro), medidor leve KYOEI_FRAME_STATS e amostrador KYOEI_SAMPLE_THREADS.
- Último passo, NÃO testado no jogo: sons nativos no "Return to Desktop" (se0/Cursor_Move ao entrar/sair da linha, se0/Cursor_Set ao confirmar, chamando a função de som do jogo 0x6f1df0 com prólogo e textos verificados; fechamento atrasado 350 ms para o som tocar). Só o teste unitário test_kyoei_title_exit passou; o teste no jogo foi interrompido. Próximo: testar navegação/confirmação no título (log sem "menu sound call differs", sem travar, jogo fecha após confirmar) e Davy ouvir o som.
- Pendente: confirmar com Davy o "voltar pro menu" do pedido do botão; frames_ahead=2 candidato (+~2 FPS, ruído, +1 quadro de latência); travada de ~1 s ao abrir a pausa (jogo despeja 1.300 linhas de log; causa não provada); BP de desenho (03/04/05/10/11) e oclusão real a avaliar com comparação visual.
- Alterações preexistentes mantidas sem commit: src/kyoei_launcher.cs, tools/build_launcher.ps1.

### 23/10/2026 (noite) — sons do Return to Desktop conferidos no jogo
- `kyoei-gow3` em inglês, no mudo, com KYOEI_TITLE_EXIT=1 e a arte do portátil: descer até a 5ª linha mostra "Return to Desktop" com o sublinhado nativo; subir e descer de novo funciona; Enter fecha o jogo (log "Return to Desktop confirmed", código de saída 0), sem "menu sound call differs" nem travar. O som em si (cursor e confirmação) falta Davy ouvir, já que os testes ficam no mudo.
- Sem KYOEI_TITLE_EXIT_ART o item fica selecionável mas invisível; o launcher portátil define as duas variáveis.
- O núcleo GoW3 cria `user/` na pasta atual quando aberto direto do repositório; apagado (cache recriável).
- src/kyoei_launcher.cs e tools/build_launcher.ps1 só diferiam em fim de linha; restaurados.
- Davy esclareceu: o pedido era só o "Return to Desktop" estar no menu título (já está) com o som do jogo. Não existe pedido de "voltar pro menu"; a pendência foi anotada errado e está encerrada.

### 23/10/2026 (noite) — travada ao abrir/fechar a pausa (núcleo GoW3)
- Medida (cidade, KYOEI_FRAME_STATS): ~117-200 ms ao abrir e ao fechar a pausa; a 1ª pausa da sessão chega a ~366 ms. Bem menor que o ~1 s anotado antes.
- Descartados: compilação de shaders (2ª pausa sem compilar trava igual); espera da cópia do fundo da pausa (GOW3_STALE_READBACK=1 trava igual).
- Registro: o jogo escreve ~1.300 linhas ao pausar; cada uma forçava o arquivo ao disco (1.325 linhas: 22 ms contra 4 ms sem forçar). Agora só as linhas do runtime forçam (CMakeLists, adaptação do loader). Ganho ~18 ms por pausa.
- Medidor KYOEI_SAMPLE_THREADS: a chamada que o liga tinha ficado fora do commit 38124a8; recolocada em gz_gpu_start. Novo KYOEI_SAMPLE_SLOW_MS=<ms> (só amostras de quadros mais lentos que isso; precisa KYOEI_FRAME_STATS=1) e KYOEI_SAMPLE_FOCUS=<função> (pilhas completas que passam por ela); pilha de 48 níveis.
- Nos quadros lentos: guest main ~66% esperando; DrawRec ~30% esperando a GPU, ~24% em Presenter::PrepareFrame, ~21% criando memória no driver e ~8% em VirtualProtect (vigias de página). A criação de memória vem de StagingBufferPool::RequestFromRing (DetileImage/ObtainBufferForImage), com blocos novos de 16 MB, e só ocorre nas duas primeiras pausas (os blocos ficam guardados por 50 s): explica o excesso da 1ª pausa. Pré-alocar exigiria ~800 MB; não feito.
- Achado lateral: imagens de 2-16 MB (ex.: 0x1048cb0000, 0x1052610000, 0x104b680000) são recopiadas do buffer para a imagem (GpuDirty, flags 0x4c/0x44) ~8 vezes por quadro o jogo todo, porque shaders escrevem buffers no mesmo endereço (InvalidateMemoryFromGPU em vk_rasterizer.cpp). É trabalho de GPU (~77 MB/quadro); pode pesar no giro da câmera, mas mexer nisso exige cuidado de fidelidade.
- Teste automático: precisa da janela em foco (capture_native -Focus logo após abrir); com a saída redirecionada o passo do mudo falha ou trava. Roteiro em scratchpad (prof.ps1). Uma queda no driver NVIDIA (nvoglv64) ocorreu uma vez durante o carregamento, com o medidor ainda ocioso; dump em build/kyoei-fault-20261023-195004-2600.dmp; não repetiu em 6 carregamentos.

### 23/10/2026 (noite) — giro da câmera no núcleo GoW3
- Cache de pipelines: com binários do driver o GoW3 rodava "sob demanda" e retraduzia ~1.100 shaders por sessão (~600 ao carregar, ~500 no 1º giro). Agora GOW3_PIPELINE_BINARIES=0 por padrão (pré-aquecimento em segundo plano, "Preloaded 737 pipelines"): 2 compilações por sessão; o 1º trecho do giro não cai mais a ~28 FPS. Giro estável igual (~42-47).
- Sem efeito no giro (ABBA): GOW3_FRAMES_AHEAD=2; fence na DrawRec (bit 41 RecorderFences em GZ_DISABLED_OPTIMIZATIONS); GOW3_ASYNC_FENCES=1; vsync=0 (Mailbox, conferido no log "vsync=0").
- Causa do padrão 45 FPS: o próprio jogo espera o vblank (thread de render do jogo ~44% em sceKernelWaitEqueue dentro do envio do quadro, via 0x80044e56f/0x800437988); quadros acima de 16,7 ms viram 33/50 ms (max_ms sempre 33/50), como no PS4. Mesmo com Mailbox fica preso a 60/33. Para ficar liso, cada quadro tem de caber em 16,7 ms.
- Corrente de espera no jogo: laço principal 0x8006e5c41 espera semáforo em 0x800370fc0 (obj+0x8280+0x90, renova a cada 100) pela thread de render; 4 threads de tarefas (fila 0x800321db3) ~45% ocupadas. Estágios do núcleo: GpuCommandProcessor, DrawRec e VkRecorder ~55% cada (parte é espera ativa).
- Medidor: segue a corrente de RBP no código do jogo (sem cair; antes um desempilhamento no código do jogo derrubou o processo), mostra endereços do jogo. Desmontagem: eboot.elf em native-05/out com capstone (pip --user), mapeando pelos segmentos do ELF (boot-linked.bin não tem o mesmo deslocamento).
- Câmera pelo teclado (I/J/K/L) removida nos dois núcleos, a pedido de Davy (só mouse/controle). Conferido: L não gira mais. Testes giram com KYOEI_BENCH_TURN_FILE (funciona sem foco). Falta Davy conferir o mouse.
- KYOEI_FRAME_STATS agora mede a latência envio→flip (submit_to_flip_ms média/máx; to_recorder = do SubmitAndFlip do jogo até o flip chegar à DrawRec; prepare = PrepareFrame; to_flip = espera do vblank). Parado: 24 ms (16 + 0,2 + 8). Girando: 26-31 ms, máx 46-87 (to_recorder 19-23). O jogo tolera ~2 quadros em voo; acima de ~33 ms ele espera e o quadro vira 33/50 ms. Gargalo do giro: tradução/gravação dos comandos do quadro no núcleo (processador de comandos + DrawRec), não a placa.
- Descoberta: leitura adiada de imagens (GOW3_DEFERRED_READBACK) fica DESLIGADA no Kyoei (o núcleo só liga com a variável =1; ninguém a define); cada leitura faz Scheduler::Finish na DrawRec. Ligada: giro 45-48 contra 42-47, dentro do ruído; não adotada. Obs.: com ela desligada, GOW3_STALE_READBACK não tem efeito (precisa da adiada); revisar a anotação de 22/10.
- Travar a 30: GOW3_FPS_LIMIT=30 não limitou (flips do jogo vêm por EOP direto da DrawRec, fora da fila do limitador). Quadros de ~50 ms no giro dariam tranco mesmo a 30.
- Onde o quadro espera no núcleo: o processador de comandos passa ~29% em WaitRegMem do rótulo do VideoOut (liverpool.cpp:1239): antes de desenhar num buffer de tela, espera o flip que o libera, como no PS4 (2 buffers). Depois disso, traduzir e gravar o quadro (processador + DrawRec) leva ~15 ms parado e 17-22 girando, contra 16,7 de orçamento: é esse o custo a reduzir.
- Testados e desfeitos (sem ganho ou com perda): (a) drenagem do fim de rodada que cede a novos envios (to_recorder igual); (b) liberar o rótulo do buffer logo após PrepareFrame copiar a imagem (giro 42-51, mas parado caiu para 54-57: desfeito). Remendos e cópias adaptadas apagados.
- Medidor: KYOEI_SAMPLE_FOCUS mostra arquivo:linha de cada ponto (símbolos), o que revela funções embutidas.

### 23/10/2026 (madrugada) — ícones Xbox e novo plano de interface
- Pedido de Davy: (1) ícones conforme o controle em uso (teclado/mouse, PlayStation, Xbox), 100% usável no controle; (2) modo foto no menu de pausa + atalho no controle; HUD do modo foto e telas de salvar/carregar/teclado refeitas no estilo do jogo; (3) estudar clima e horário no modo foto (desistir se impossível); (4) diagrama do controle (operation_normal*) adaptado a cada controle e traduzido; (5) seguir com o desempenho do giro.
- FEITO (1): tools/make_xbox_icons.py desenha a versão Xbox sobre cada ícone nativo (mantém o corpo; símbolo PS apagado com refill e trocado: ○→A, ✕→B, △→Y, □→X, L1/R1→LB/RB, L2/R2→LT/RT, L3/R3→LS/RS, OPTIONS→☰, touchpad→⧉; direcional e analógicos iguais). Fonte Segoe UI Bold (interface do Xbox) quando instalada. Cores dos botões do conjunto Kenney (CC0). Pacote KYPROMP3 (4 variantes, 48 texturas, ~20 MB); run_native e inspect aceitam KYPROMP3. Janela informa se o último controle é PlayStation (SDL_GetGamepadType PS3/4/5); outros recebem Xbox. KYOEI_CONTROLLER_STYLE=playstation|xbox força. Launcher: opção "Xbox controller" nos avisos de botão. Conferido no jogo (forçado): aviso de salvar com A verde. Falta Davy testar com controle Xbox e PS físicos.
- Diagrama do controle: 4 texturas (operation_normal, _s, _207, _207_s; BC3), legendas ainda em japonês até no pacote inglês. Precisam: tradução, lógica americana (trocar só os textos de ○/✕, as linhas apontam para o botão físico), versão Xbox (desenho de controle Xbox no mesmo estilo) e teclado/mouse, e escolha na leitura do arquivo (não pelo reconhecimento de upload, por custo; ver docs/INSTRUCOES-TEXTURAS-CLAUDE.md).
- 24/10 madrugada — diagramas de comandos (L3/H: grande → pequeno → oculto; HUD, não menu): tools/make_controller_diagram.py redesenha as quatro texturas em inglês com o esquema americano (operation_normal, _s, _207, _207_s). Desenho do controle, contorno e linhas verdes preservados; o texto antigo é removido e o fundo (brilho) reconstruído pela mediana de pixels limpos (nunca do corpo do controle, das linhas e seu halo, nem dos símbolos coloridos). Na lista pequena, os ícones ○/✕ das duas primeiras linhas trocam de lugar; a linha da empilhadeira continua rosada. Termos do jogo: "Giant Shadow Camera", Dash, Evade. Entram no delta de leitura (build_ui_corrections: 66 recursos, 4 diagramas). Conferido no jogo o grande (H). Prévias em G:\kyoei-toshi-work
eview\diagrama*.png. Ressalvas: brilho ao longo da borda do controle mais fraco num trecho do _207; resíduos de 1-2 px junto a algumas curvas das linhas.
- Pendente: versões Xbox (desenhar controle Xbox no mesmo traço) e teclado/mouse do diagrama; o delta atual troca o índice uma vez na abertura, então alternar por controle exige variantes de mesmo tamanho escolhidas na leitura. Também visto na pausa: rótulos japoneses na tela Personagem (帽子, 手持ち, グローブ, メガネ, マスク, アクセサリー) e "L3 閉じる" sob o diagrama.

## Entradas do ramo traducao-natural-en (integrado em 24/10/2026)

### 18/10/2026 — inglês natural (ramo traducao-natural-en)
- Davy aprovou deixar o inglês "naturalzão": escolhas viram a fala direta
  (ação curta entre parênteses só quando precisa), contrações, voz própria
  de cada personagem, sentido fiel ao japonês, nomes do glossário intactos.
- Feito em worktree separada (G:/kyoei-toshi-natural) para não atrapalhar o
  Codex; depois as duas versões serão auditadas lado a lado.
- Prontas: fases 201–208 (um commit por fase). Corrigidos erros de sentido
  (irmão de Saeki, foto "autografada" da Honoka, nome do navio em japonês,
  fala da noiva atribuída ao jogador, "não é uma moto").
- Fonte do jogo não tem ¥: usar "yen". Senha da fase 208 em ASCII.
- Próximo: fases 209–218 (217 não tem falas), Text_Heroine, depois itens,
  dicas de carregamento e notícias. Nada testado no jogo ainda.
### 20/10/2026 — inglês natural: revisão de texto concluída
- Revisados até o fim: fases 209–218, Yuki, Risa, Common/Plan00/Epilogue/
  Mail, nomes de itens (padronizados e iguais nas falas), dicas de
  carregamento, recapitulações, marcadores e objetivos do mapa, registro de
  missões, anúncios, descrições de itens, leituras (Last Sword) e notícias.
- Normalizados em todo o texto: pontuação e letras de largura japonesa
  viraram ASCII, ¥ virou "yen", aspas retas, manchetes com maiúsculas de
  título, citações com aspas duplas, nomes de lugares/lojas/falantes únicos.
- Verificação de pixels com as larguras reais da fonte do jogo (baseada em
  tools/measure_translation_layout.py): nenhum texto com mais linhas que o
  japonês, nenhum glifo ausente. Os \n dos CSV .csh continuam escapados.
- Nada testado no jogo ainda. Próximo: traduzir as texturas
  (tools/localize_textures.py, translations/textures-en.json).
### 20/10/2026 — texturas em inglês completas (168)
- Levantamento das 1.412 candidatas: traduzidas só as que o jogador lê
  (controles, menus, abas, avisos de ação, nomes das fases, dicas de comando).
  Cenário, anúncios do site, fotos de notícias, telas de carregamento e itens
  de leitura (texto já aparece traduzido no leitor) ficam como no original.
- tools/localize_textures.py ganhou o modo "regions": apaga só as letras e
  reconstrói o fundo (degradê, brilho girado, fundo de outra tela do mesmo
  desenho), aceita RGBA8, quebra de linha e tamanho único por tela.
- translations/textures-en.json: 168 texturas, sem nenhum texto japonês.
- Versões sem texto para o português: build/textures-blank (privado, fora do
  repositório), geradas com KYOEI_ERASE_ONLY=1.
- Conferência visual com zoom, lado a lado; galeria privada
  build/texture-review-natural. Nada testado no jogo ainda.
### 20/10/2026 — QA do inglês, padrão americano e plano pós-tradução
- QA automático do texto (13.130 entradas): símbolos japoneses de rótulo
  viram [Photo]/•, colchetes seguros (sem < > que o motor pode ler como
  tag), termos únicos (observation deck, Hananira-cho), verbos de ação
  alinhados (Examine/Talk/Collect). pxcheck: 0 excessos, 0 glifos ausentes.
- Texturas: posição do inglês ancorada na tinta japonesa original (mesma
  borda/centro em relação a ícones); tamanho único por tela; auditoria de
  alinhamento em 511 regiões, desvios restantes são da régua (multilinha).
- Pedido do Codex (docs/INSTRUCOES-TEXTURAS-CLAUDE.md) feito: diagramas de
  controle no padrão americano (X confirma/interage, bola cancela), símbolos
  físicos do controle intactos; telas grandes, listas pequenas e menu
  inicial. Telas "amarrado" e "cofre" tiveram linhas de chamada refeitas
  com linhas nativas de outras telas do mesmo desenho.
- PLANO agendado por Davy, logo após fechar o inglês e antes do PT-BR:
  1. Diálogos/popup de save, load e teclado virtual com símbolos de botão
     (não texto por extenso) e design mais "PlayStation", mesmo estilo.
  2. Detecção automática do controle (PlayStation, Xbox, Nintendo,
     teclado) com escolha manual; Nintendo usa o padrão japonês (A = bola).
  3. Teclado como jogo de PC nativo: teclas personalizáveis (já iniciado,
     setas substituindo L1/R1), e todas as texturas de tutorial/menu
     trocadas conforme o dispositivo, no mesmo estilo visual.
### 20/10/2026 — textos de menu fora das tabelas (HUD e eboot)
- Comparação com o kit de fã (Test7) revelou textos fixos em arquivos .hud e
  no eboot.bin que nossas tabelas não cobriam. Levantamento próprio completo:
  394 propriedades de texto em 77 HUDs (maioria texto-modelo substituído em
  tempo de execução) e strings C do eboot (maioria nomes internos do motor).
- Traduzidos 397 textos visíveis: janelas de salvar/título/próxima fase,
  resultados, loja, troca de moedas, e-mail, brilho, relógio do HUD, nomes dos
  pontos de salvamento, "Start from Stage N", nomes/descrições da Collection,
  categorias de item, nomes de fase da lista. 168 a mais que o kit de fã.
- translations/binary-en.json (sem texto japonês, só hash do original) e
  tools/apply_binary_text.py: troca no lugar, inglês nunca maior que o espaço,
  verificação de hash, tokens e campo de tamanho. Aplicação PASS 397/397.
- Limite de bytes obrigou títulos de fase mais curtos que os das texturas
  nesses espaços. Falta integrar ao portable (delta de recursos + eboot) e
  testar no jogo.

### 24/10/2026 — tradução natural integrada; modo foto no estilo do jogo
- Davy notou narração, dicas e carregamento ainda estranhos: a versão jogada usava runtime-preview-en-16 (15/10). O ramo traducao-natural-en (texto revisado, 168 texturas, 397 textos de HUD/executável) nunca tinha sido integrado. Com autorização de Davy, fundido em nucleo-gow3 (merge 55b6d20; diário corrigido em a456575, as entradas do ramo ficam numa seção própria).
- Prévia nova runtime-preview-en-17 (13.138 textos + 168 texturas, incluindo os 19 diagramas de controle em padrão americano; meu gerador de diagramas apagado por ser substituído). Manifesto docs/m4/runtime-preview.json aponta en-17.
- Textos de HUD (148) entram no delta de leitura (build_ui_corrections, base en-17). Textos do executável (249): tools/build_eboot_text.py mapeia as posições do eboot.bin (SELF) para endereços via eboot.elf; pacote KYTEXT1 sem japonês (endereço, tamanho, FNV do original, inglês); kyoei_ui_text.c aplica com KYOEI_EBOOT_TEXT só se o original confere.
- run_native.ps1 agora define KYOEI_ARCHIVE_DELTA (build/ui-corrections-en.kydelta), KYOEI_EBOOT_TEXT (build/eboot-text-en.kytext) e KYOEI_UI_ART (build/ui-art.kyart); antes o atalho local nem usava o delta. Portátil (build_portable + launcher) gera e passa os mesmos. Pacote de ícones refeito sobre en-17.
- Modo foto: HUD no estilo do menu de Opções do jogo (tools/make_ui_art.py, atlas KYART2 no atlas de fontes do overlay; fonte Segoe UI Semibold + CJK), textos em inglês/português/japonês (src/kyoei_ui_text.h), barra de comandos com ícones do controle em uso. Conferido no jogo (teclado). Aprovado por Davy pela maquete; entrada "Photo Mode" na pausa aprovada; atalho Share/View mantido (o PS4 não entrega Share ao jogo).
- NÃO TESTADO NO JOGO (Davy pediu para não abrir agora): en-17, HUD texts, executável. Depois de conferir, apagar runtime-preview-en-16 (6,3 GB).
- Pendentes desta rodada: controle Nintendo (A à direita confirma, padrão japonês; ícones A/B/X/Y nas posições Nintendo, L/R, ZL/ZR, +/−), entrada "Photo Mode" na pausa, telas de salvar/carregar/teclado no mesmo estilo, clima/horário no modo foto, desempenho do giro.


### 24/10/2026 — retomada da integração Nintendo, sem abrir o jogo
- Retomado o trabalho de Claude a pedido de Davy. Nintendo já estava no commit faa565f; helpers visuais no 69b4fbf. Não foram reimplementados.
- Build do port e dos testes de entrada, prompts, foto, overlay e teclado virtual concluído. Os cinco executáveis de teste passaram; CTest não registra testes nesta configuração, portanto foram executados diretamente.
- Pacotes locais en/ja validados: KYPROMP4, 48 entradas cada; atlas KYART2 com 75 imagens, 16 Nintendo. Atalho verificado com -CheckOnly em inglês e japonês, sem iniciar o jogo.
- Corrigida a herança de variáveis de tradução ao alternar para japonês: atalho limpa EBOOT_TEXT/UI_ART e launcher portátil limpa delta, texto de executável e arquivo de tradução antes de definir o idioma.
- Preservada a alteração já existente em tools/build_portable.py que retira J/I/K/L da descrição da câmera; não incluída no commit desta retomada.
- A confirmação visual e física de Nintendo, en-17 e textos HUD/eboot continua pendente: nenhum jogo aberto. Preservada en-16 até essa conferência. As outras tarefas planejadas (entrada na pausa, telas salvar/carregar/teclado, clima/horário e desempenho do giro) continuam separadas desta finalização da integração.
- Cópia privada build/City-Shrouded-in-Shadow-Portable atualizada com sucesso (sem iniciar): runtime, launcher recompilado, delta com 148 textos HUD, 249 textos do executável, prompts Nintendo en/ja e atlas de 75 imagens. Pacotes comparados byte a byte com os locais. Log: build/finalize-20261024-portable.log. Não foi feito pacote público de distribuição.

### 24/10/2026 — updater dos amigos para o núcleo GoW3
- Davy autorizou preparar updater se a troca de núcleo permitisse. Os targets kyoei/kyoei-gow3 compartilham fontes do carregador, runtime e entrada; o backend é ligado no executável. Update somente do runtime faz sentido, sem substituir arquivos preparados, launcher ou tradução antigos.
- Correção da retomada anterior: o empacotador ainda selecionava build/kyoei.exe (núcleo antigo). Geradores portátil, updater e atualização local agora escolhem build/kyoei-gow3.exe. Portátil pessoal corrigida e hash igual ao runtime do update. Atalho run_native continua apontando para o target antigo; não foi alterado por esta atualização para amigos.
- Gerador aceita --baseline-update para recuperar a allowlist de um ZIP cumulativo verificado, pois Friends-Test foi apagado; --release-notes evita instruções antigas. Preservada fora do commit a edição anterior sobre J/I/K/L em build_portable.py.
- Gerados Windows e Linux/Wine 20261024-GoW3, cerca de 8,5 MB cada. Somente runtime, aplicador e instruções/hashes; sem dados do jogo, saves, en-17 ou novos ícones. Não enviados externamente.
- Seis testes unitários passaram e ZIPs reais passaram em dez cenários (updates 15/16/17/17b/18 × Windows/WSL): check-only, aplicação, reaplicação, backup exato, hash final, save e prefixo preservados. Hash original aceito a partir do ZIP cumulativo; payload original indisponível para reteste.
- Sem abrir o jogo. Compatibilidade jogável da migração com launcher/tradução antigos ainda precisa ser conferida; updater marcado experimental, sem promessa de FPS. Hashes e instruções em docs/ATUALIZACOES-AMIGOS.md; relatório privado build/research/update-validation-20261024-gow3.json.

### 24/10/2026 — patch pequeno da tradução: SALVO E INTERROMPIDO por Davy
- Pedido: criar atualização pequena en-16 -> en-17. Durante o trabalho, Davy esclareceu que a tradução antiga deve ser EXCLUÍDA ao concluir; não manter backup de 6,35 GB. Ao final pediu salvar tudo e interromper para Claude avaliar amanhã se continua ou muda a implementação. Nenhuma autorização para seguir os testes nesta sessão.
- Código novo salvo: tools/translation_patch.py (patch streaming KYTRPAT1: TOC nova + cópia do corpo comum + cauda traduzida); tools/build_translation_update.py (gerador); tools/apply_translation_update.py (Windows/Linux Python 3.11+, hashes, GoW3 obrigatório, staging, transação, recuperação antes de concluir, exclusão da antiga após sucesso); tools/test_translation_update.py. Instruções em docs/ATUALIZACAO-TRADUCAO.md.
- ZIP montado: build/Kyoei-English-Translation-Update-20261024-PENDENTE.zip, 10.225.760 bytes (~10,2 MB), SHA256 7d6da2225e8f56a81516d3cefe87f5afe2e0b04cf61934645785834faceaff19. Patch de arquivo: 8.209.392 bytes; cópia local de 6.344.699.095 bytes da en-16, sem common.psarc no ZIP. Inclui launcher e seis arquivos auxiliares no total; não troca runtime/saves/configurações/japonês. Exige primeiro o updater GoW3 de 24/10. Cerca de 6,42 GB livres temporariamente; após sucesso remove o PSARC anterior. Guarda só registro e backups pequenos dos auxiliares.
- Testes pequenos Windows/Linux passaram durante o desenvolvimento (14 antes do último teste adicionado; suíte atual tem 15). Cobrem arquivo reconstruído exato, corrupção, saída existente, aplicação/reaplicação, falha na troca e recuperação, falta de espaço, lock do launcher, caminhos, exclusão da antiga e retomada da limpeza. Último acréscimo impede __pycache__ no CheckOnly e tem teste CLI próprio.
- Validação REAL NÃO concluída. Script privado build/research/verify_translation_update_20261024.py: usa en-16 real de 6,35 GB com auxiliares antigos sintéticos e hashes correspondentes no manifesto de teste, para Windows/WSL. A última rodada passou pelo CheckOnly real Windows (7 arquivos, 6,42 GB), mas foi interrompida a pedido antes de comprovar instalação. Não afirmar PASS do arquivo real nem prontidão para envio. Log build/research/translation-update-validation-20261024.log; podem existir diretórios descartáveis build/translation-validation-* com staging/transaction.json da execução interrompida. Não são instalação pessoal nem saves reais.
- Os primeiros testes reais pararam por problemas do harness (codificação de stdout) e pelo cache Python gravado no pacote durante CheckOnly; corrigidos. O ZIP foi atualizado com as fontes finais sem regenerar o patch binário já montado. Conferir scripts dentro do ZIP antes de liberar; metadados estão no JSON ao lado.
- Nenhum jogo aberto, nenhuma cópia de amigo alterada ou pacote enviado. A tradução en-17 continua parcial/em conferência; este updater não preenche textos que ainda faltam traduzir. Davy está insatisfeito com isso e quer que Claude reavalie amanhã.
- Preservada sem commit a edição anterior em tools/build_portable.py sobre J/I/K/L. A base local en-16 foi preservada como entrada necessária à geração/validação; o pedido de exclusão foi aplicado à tradução antiga instalada pelo updater, depois do sucesso.

### 24/10/2026 — tradução como delta pequeno (revisão do updater do Codex)
- Revisado o updater en-16 -> en-17 do Codex: seguro, mas exigia Python no Windows, relia 6 GB ~7 vezes e encadeava patches de versão a versão. Com aval de Davy ("resolva"), trocado o modelo: a tradução inteira vira um delta de leitura sobre o common.psarc original do disco.
- tools/build_translation_delta.py: 355 recursos (en-17 + correções de HUD) em languages/en/translation.kydelta, 8.324.179 bytes, releitura verificada. Base confirmada como original (SHA 3a54e70d…).
- kyoei_translation_mount.h: com KYOEI_ARCHIVE_DELTA e sem arquivo traduzido, não para. Launcher usa translation.kydelta se existir (instalações antigas seguem com o psarc). build_portable gera o delta e apaga a cópia de 6 GB da portátil (derivado recriável; en-17 segue em kyoei-toshi-work).
- Prova sem abrir o jogo: harness com a camada de arquivos real do runtime leu os 6.353.023.224 bytes virtuais idênticos ao esperado (SHA 2b47609e…). Launcher --check ok. test_translation_mount ganhou caso do delta.
- Updater novo: build/Kyoei-English-Translation-Update-20261024.zip, 17.790.302 bytes; runtime + launcher + delta + prompts en/ja + texto do eboot + artes. Windows por PowerShell (sem Python), Linux por Python. Exclui a tradução antiga de 6 GB só depois de tudo instalado. test_translation_update passou em Windows e WSL (check-only, aplicação, reaplicação, runtime desconhecido recusado, falha no meio desfeita). Removidos translation_patch.py, o ZIP PENDENTE e o script de validação antigo.
- Pendente: Davy abrir o jogo em inglês pela portátil e conferir textos; depois apagar runtime-preview-en-16. Não enviado a ninguém.
- Conferido no jogo em segundo plano (janela fora da tela, sem foco, mudo, user temporário): log "Kyoei delta: translation.kydelta applied"; abertura e narração em inglês ("Their everyday lives go on, just like always."). Legenda da cena de abertura ("僕は 今…") segue em japonês: lacuna já existente na en-17 (o delta reproduz a en-17 byte a byte), não regressão. Apagada runtime-preview-en-16 (6,3 GB).

### 24/10/2026 — textos que faltavam na abertura (en-18)
- Davy viu a legenda "僕は 今…" em japonês. Auditoria de cobertura (docs/m2/audit.json × translations): texto japonês só existe no common.psarc (stages e movie não têm). Faltavam 3.092 strings: 2.242 de item_data são anotações internas ("仮登録", itens de teste), 1.636 BannedWords (filtro), placeholders de notícias/dev/teste. Visíveis e faltando: abertura/criação do personagem e NPCs da primeira rua (Text_Stage_201, ~100), objetivos iniciais do mapa (9), descrições do menu de Opções (16), memorando do Hebisaki (reading_material r37/c4).
- 131 traduções novas em translations/localization-en-additions-20261024-01.json (estilo natural; revisão humana pendente). Créditos (staffroll, 320 nomes reais em kanji) mantidos em japonês: romanizar nomes sem a leitura oficial seria inventar.
- runtime-preview-en-18: 13.269 textos + 168 texturas; só os 4 recursos alterados diferem da en-17. Portátil e build/Kyoei-English-Translation-Update-20261024.zip refeitos (17.789.943 bytes, SHA 8c141dc6…); test_translation_update passou em Windows/WSL.
- Conferido em segundo plano/mudo: escolha "Right now, I'm..." e "Leaving the office after work"/... em inglês. Apagada en-17 (substituída pela en-18).
- Correção da florista (a491fdb, 21/10) já estava na en-17 e segue na en-18; amigos com en-16 ainda veem o "...".
- Davy achou "Right now, I'm..." artificial (já vinha da pilot-intro antiga, usada até a en-16). Reescritas 22 falas da abertura: "Play as a man/woman", "So here I am...", "Just got off work"/"Just got out of class"/"Out on the town, totally pumped!"/"Not sure why I even came here". en-18 refeita, portátil e ZIP refeitos (17.789.709 bytes, SHA 0b6b1b2f…), testes Windows/WSL ok, conferido no jogo em segundo plano.
- Davy: a escolha já aprovada era "I'm a man." / "I'm a woman." (pilot-intro). Restaurada; antes de reescrever falas, conferir versões anteriores aprovadas (pilot-*.json e histórico). ZIP refeito (17.789.722 bytes, SHA d5798363…), testes ok.
- Pedido de Davy: tudo natural. Reescritas as falas novas da primeira rua/objetivos/opções (karaage como no resto do jogo; "someone important to me" e "girlfriend" alinhados ao texto já revisado). Varredura de padrões literais no texto todo ("Is that so", "Thank you very much", "It is said that", "That is", "It seems"...): 17 trocas; mantida a fala formal da entidade alienígena nos créditos (voz do personagem). ZIP refeito (17.789.858 bytes, SHA 9d58131c…), testes Windows/WSL ok.

### 24/10/2026 — telas de salvar/carregar/nome no estilo do jogo; narração
- kyoei_overlay_model.cpp: lista de saves, confirmações/progresso e entrada de nome desenhadas com as peças do menu de Opções (barra de título, marcador, linhas, moldura, barra de comandos com ícones do controle em uso ou teclas); mouse por áreas invisíveis (hover seleciona, clique confirma, roda rola). Textos en/pt/ja. Teclado virtual 10x4 no mesmo estilo. Prévia offline (rasterizador de ImDrawData, privado) e conferido no jogo em segundo plano: tela de nome com teclas Enter/Esc/Backspace/setas. test_kyoei_overlay e test_kyoei_ime_overlay passam.
- Créditos (staffroll) ficam em japonês: não há créditos oficiais em inglês (jogo só saiu no Japão); romanizar nomes reais seria chute.
- Davy: narração do carregamento estranha e "(Rain...)" parecia legenda de som. Reescritas as telas de introdução (loading_image_and_text r0-r5) e "(It's raining...)" (pensamento do protagonista).
- Modo foto: clima/horário reais descartados (ver docs/re/FOTO-LUZES-CLIMA.md, decisão final); adicionados filtros Golden Hour/Blue Hour/Overcast (só cor; CPU e shader iguais). test_kyoei_photo_settings passa.
- Giro da câmera: roteiro de medição em segundo plano (janela fora da tela, mudo, KYOEI_BENCH_TURN_FILE, rua do início após a criação do personagem) — base 47-56 FPS girando, picos 34 ms. Gargalo segue sendo CPU do núcleo (processador de comandos + DrawRec, ver 23/10); a recópia GpuDirty é trabalho de GPU, que está ~50% livre, então não é o limite. Sem mudança segura nesta rodada.
- Controle Nintendo: código e testes prontos; falta conferir com controle físico.
- Portátil pessoal atualizada com o runtime novo (telas no estilo do jogo, filtros) e a en-18.
- Launcher: opção 3840 × 2160 (saída 4K; resolução interna do jogo continua a do PS4, 1080p, pela regra de fidelidade). Conferido em segundo plano: janela 3840x2160 abre e roda.

### 24/10/2026 — novo plano (pedido de Davy)
1. Ícones de botão próprios de cada plataforma (Xbox, Nintendo, teclado/mouse), não o desenho do PS com o símbolo trocado (ex.: View/Menu do Xbox, sem formato de touchpad).
2. Texturas de controle (diagramas e tutoriais) em versões Xbox, Nintendo e teclado/mouse, trocando conforme o dispositivo.
3. Desempenho do giro: trabalho longo no núcleo (tradução de comandos).
4. Opções gráficas de PC: regra de fidelidade relaxada (AGENTS.md): resolução interna maior/menor, upscaler, presets.
- Ícones Xbox/Nintendo redesenhados do zero (tools/make_pad_icons.py) para os 19 ícones de fonte (font/icon/IC_*): botões redondos (Xbox colorido, Nintendo branco), bumpers, gatilhos, sticks, Menu ☰/View ⧉ e +/− redondos, D-pad cruz inteira. Mesma caixa ocupada e sombra suave dos nativos. Texturas com contexto (balões de fala, NEXT, pular cena) seguem com a troca de símbolo. Conferido no jogo forçando Xbox (pausa com LB/RB). Diagramas de comandos ainda PS (próximo trabalho de arte; o gerador antigo está no histórico, 026211f).
- Opções gráficas: o núcleo GoW3 já tem FSR 3.1/FSR 4/DLSS, presos às constantes do God of War. Captura de quadro do Kyoei funcionando (patch frame_capture_whole_frame_cpp) e cena/profundidade/projeção/vista localizadas: docs/re/UPSCALER-KYOEI.md. Próximo: ligar o gancho de câmera do Kyoei e validar os vetores de movimento.
- FSR 3 rodando no Kyoei em modo experimental (docs/re/UPSCALER-KYOEI.md): AA bom parado e cenário certo girando, mas protagonista com blocos e FPS cai de ~50 para ~14 (custo de CPU). Achado: o jogo é limitado pela CPU, então resolução menor/upscaler não aumentam FPS aqui; servem para qualidade. Desempenho de verdade = acelerar a tradução de comandos.

### 25/10/2026 — esvaziamentos do draw pipe medidos
- Giro em segundo plano, mudo, GOW3_FRAME_STATS=1: só o esvaziamento do fim da submissão espera (~1 por quadro, 18-25% da thread de comandos). Os dispatches de compute não esvaziam. Hipótese de vários esvaziamentos por quadro descartada; sem mudança de código. Próximo alvo: custo por draw da gravação (DrawRec). Detalhes em docs/DESEMPENHO.md.
- Davy notou o jogo a ~30 FPS. Regressão desde 22/10 (53e5cf8): depois do primeiro diálogo do Kyoei, o driver redesenhava o último quadro em todo vblank livre (revisão do overlay nunca confirmada com ele invisível). Achada por busca binária (worktree G:/kyoei-bisect); corrigida em kyoei_gow3_stubs.cpp: parado ~33 -> 57-59, girando ~31 -> 41-50. test_kyoei_overlay passa; diálogo do save some normalmente. O GOW3_FRAMES_AHEAD=2 testado antes era efeito da regressão (sem ganho após a correção): revertido para 1.
- Davy: no aviso "★Save" o círculo do botão parece solto da placa branca. Comparado com o japonês (HUD_FIT desligado): no original a placa passa sob o círculo, sem vão, e é mais baixa que ele; em inglês o HUD_FIT deixa placa/texto maiores e aparece um vão. Defeito nosso, a corrigir. Também: com o padrão americano o ícone deveria ser X, aparece a bola (conferir).
- Borda do "★Save": os texels transparentes do anel/haste (texture/text marker/talk_icon_01..06) são pretos (0,0,0,0); a filtragem os mistura no anel branco e aparece um contorno cinza tracejado sobre a placa (igual no japonês original, a 1080p). build_prompt_pack.py agora preenche esses texels com a cor vizinha nas variantes teclado/americana/Xbox/Nintendo (a original fica intacta: é o reconhecimento por pixels). Simulação da filtragem: contorno some. No jogo, "Kyoei prompt matched: 48x88" com o pacote novo; test_american_icons PASS. Davy confirmou: X interage e bola volta em todos os controles, exceto Nintendo (japonês) — já é o comportamento do pacote (PlayStation usa a variante americana). Ele viu a bola provavelmente por pacote antigo na portátil: refeita.
- Ajuste de largura das placas (KYOEI_HUD_FIT) NUNCA funcionava no núcleo GoW3: o objeto de texto do marcador fica na pilha da thread do jogo (0xFC...), fora da tabela de regiões do runtime, e readable() recusava 100% das chamadas. Agora cai para VirtualQuery com cache da última região por thread. "★" aceito como um caractere de largura cheia. No jogo: "★Save" passou de escala 4,92 (nativa) para 3,78 (placa do tamanho do texto); com o pacote novo, X azul e anel sem contorno sobre a placa. FPS igual. test_kyoei_hud_trace PASS.
- Touchpad: o port nunca enviava toques ao jogo (touchData vazio), nem com controle PlayStation; "Slide Touchpad" (arrastar o minimapa; mover o braço no capítulo 204) não funcionava. src/kyoei_touch.h preenche os dedos reais do touchpad (DualShock/DualSense via SDL) e cria a alternativa para os outros: segurar View/−/M e mover o analógico direito ou o mouse arrasta (câmera parada); toque rápido continua ligando/desligando o mapa (enviado ao soltar). Também: o botão View do Xbox (− do Nintendo) não estava mapeado; agora é o botão do touchpad, como o launcher dizia. Conferido em segundo plano/mudo com gatilhos de teste KYOEI_BENCH_SWIPE_FILE e KYOEI_BENCH_DRAG_FILE: minimapa arrasta, câmera parada, toque rápido em M alterna. Falta Davy testar com controle físico (dedo real no touchpad e View do Xbox).
- Diagramas de comandos para Xbox: tools/make_pad_diagrams.py redesenha os 9 diagramas grandes (controle Xbox no estilo nativo, nomes dos botões, linhas até o botão certo) com auditoria automática de layout (cruzamentos, linhas sobre texto/botões, textos encostados/sobre o desenho): 0 problemas nos 9. As 9 listas pequenas trocam só os símbolos pelos ícones de fonte do Xbox (modelos rotulados à mão de 3 listas; L3/R3 pelo texto da linha). Davy aprovou o visual do controle; pediu auditoria de linhas/espaçamento (feita).
- Diagramas de comandos por dispositivo, prontos e no jogo: tools/make_pad_diagrams.py gera os 18 (9 telas + 9 listas) para Xbox, Nintendo (controle Pro: X em cima, A direita, B embaixo, Y esquerda; −/+, Home, Capture) e teclado/mouse (teclas usadas destacadas, setas, mouse). Auditoria automática de layout: 0 problemas nos 54. Duas organizações: a nativa (colunas do PS) e, quando ela não fecha, colunas pela posição do botão (sempre no teclado). Colunas de baixo com botões sob os próprios textos passam para a direita deles (Nintendo). "Slide Touchpad" vira "View/−/M + Right Stick/Mouse" (alternativa real em src/kyoei_touch.h); no cofre (204) sai, pois repete o analógico esquerdo.
- build_prompt_pack.py --diagrams (só no pacote inglês) inclui as 18 texturas: pacote de 66 texturas, 177 MB. Runtime: limite do pacote 32 -> 256 MB; carregamento com std::call_once (a leitura longa deixava outras threads marcarem texturas como "sem par" para sempre: numa rodada nada foi reconhecido). Conferido em segundo plano/mudo com KYOEI_CONTROLLER_STYLE xbox/nintendo e KYOEI_PROMPT_MODE keyboard: tela grande (H), lista pequena e marcador do save trocam em todos. test_kyoei_live_prompts PASS.
- Pendência vista no teste: dica "L3 閉じる" no rodapé da tela de comandos segue em japonês e com o ícone do PS.
- "L3 閉じる" do rodapé da tela de comandos: o rótulo do HUD é "[i16] 閉じる" (ícone + palavra), não o "閉じる" exato que build_ui_corrections já traduzia; acrescentado "[i16] Close". Falta refazer a portátil para entrar.
- ACHADO, a investigar: no núcleo GoW3 os ícones de fonte ([iN] nos textos: rodapé, tutoriais "press [i4]") não são reconhecidos pelo pacote de botões (nenhum "prompt matched" para font/icon/IC_*, nem no pacote antigo); seguem com o desenho PS em Xbox/Nintendo/teclado. O registro de 24/10 conferiu Xbox na pausa (LB/RB), que usa texturas avulsas. Próximo: ver como o núcleo GoW3 sobe os IC_* (63x50 RGBA8, 6 mips) e por que o reconhecimento falha.
- RESOLVIDO ícones nos textos ([iN]): a fonte desenha os ícones a partir da página font/utf8all_icon_0.tex (512x512 RGBA), não dos arquivos font/icon/IC_* (por isso nunca eram reconhecidos). build_prompt_pack.py localiza cada IC_* na página (posições exatas, 21 de 21) e monta a página por dispositivo com os ícones já desenhados (teclado, americano, Xbox, Nintendo). No jogo, modo teclado: o rodapé da tela de comandos mostra H no lugar de L3 ("Kyoei prompt matched: 512x512").
- Pacote de botões comprimido (KYPROMZ1: zlib, carregado na memória uma vez): 177 MB -> 9,5 MB (en), 1 MB (ja). inspect_prompt_pack lê os dois formatos.
- Opções gráficas de PC no launcher (botão GRAPHICS): predefinição PS4 original/Enhanced/Custom, filtro anisotrópico (desligado, 2x-16x), nitidez AMD RCAS (desligada/baixa/média/alta), ampliação acima de 1080p (bilinear/AMD FSR 1). O launcher grava data/graphics.ini e passa GOW3_CONFIG. Descoberto: o núcleo aplicava RCAS sempre que a imagem enchia a janela (1080p): agora desligado por padrão (visual do PS4). FSR 1, que o núcleo GoW3 desativava, reativado só no Kyoei (gow3_adapt). Log mostra as opções efetivas ("GPU (GoW3 core): vsync=… anisotropy=… sharpening=… fsr1=…"). Conferido: padrão 0/0/0; Enhanced 16/50/FSR1 em 2560x1440. Resolução interna acima de 1080p não oferecida: exigiria corrigir o tamanho de render do próprio jogo (o núcleo só sabe fazer isso no God of War); o jogo é limitado pela CPU, então resolução menor não daria FPS.
- Launcher: controles sem "I J K L"; linha "Drag the minimap"; "SAVE & CLOSE" mostrava "SAVE  CLOSE" (& virava atalho).

### 26/10/2026 — limpeza: só o núcleo GoW3
- Com aval de Davy, removido o núcleo gráfico antigo (Godzilla/shadPS4 pré-compilado): ~1000 linhas de adaptações no CMakeLists (vídeo, Liverpool, caches, apresentador, objeto kyoei_videoout), 41+5 arquivos de src/ só dele (readback cooperativo, detilers, cache de estágios, gravação Vulkan, rastros de raster/travadas, estatísticas antigas) e 11 testes C++ e 11 testes/ferramentas Python correspondentes. Ficam o runtime C do Godzilla (third_party, intacto), o controle e a janela adaptados.
- O alvo kyoei agora liga o gow3_core: o executável é build/kyoei.exe (kyoei-gow3.exe não existe mais); build_portable, build_runtime_update, update_portable_runtime, capture_native, input_native e mute_test_audio atualizados. F9 (rastro de raster) saiu da janela.
- Conferido: compilação completa sem erros, todos os testes C/C++ e Python passam, partida em segundo plano normal.
- Limpeza 2: saíram os diagnósticos de investigações encerradas no núcleo GoW3 (relatório de uploads KYOEI_UPLOAD_REPORT/WATCH/OVERLAPS e KYOEI_IMAGE_WATCH, experimentos do fundo da pausa KYOEI_SYNC_READS/RESOLVE_WRITTEN, vigias de memória KYOEI_FAULT_REPORT/PROTECT_WATCH/PROTECT_DELAY), os autotestes e o desligamento de cache do núcleo antigo no carregador, e o stub de gravação Vulkan. Ficam o amostrador de threads, o registro do HUD, os gatilhos de teste (giro, toque, arrasto) e a câmera do upscaler (FSR 3, para retomar). Compilação, testes e partida em segundo plano ok.
- Limpeza 3: README.md para o GitHub (o que é, estrutura, como compilar com as cópias fixas de third_party, como usar com o próprio dump, aviso legal). Saíram 18 documentos de sessões/planos cumpridos e estudos do renderizador antigo (seguem no histórico do git), a nota CONTINUAR-CODEX.md, os atalhos e scripts do "orbe" (rastro do núcleo antigo), "Testar desempenho" (readback agrupado do núcleo antigo), test_graphics.ps1 (modo de teste GPU indisponível no GoW3), a automação noturna de localização (agendava tarefas e desligava o PC) e as ferramentas de cota do Codex. run_native.ps1: sem opções do núcleo antigo e aceitando o pacote de botões comprimido (KYPROMZ1, que ele recusava). run_native -CheckOnly ok.
- PT-BR iniciado: tools/translate_batch.py (show/apply/publish/status) sobre a cópia de trabalho G:/kyoei-toshi-work/translation/pt-work (tem o japonês; fora do git). 2.961 entradas sem inglês marcadas como internas. Lote 0056 (Text_Stage_201): 104 de ~300 aplicadas; a segunda metade foi traduzida mas não salva (falha da checagem de segurança da ferramenta) — refazer. Pendente M3: a fonte do intermission (utf8all_IM_*) não tem 52 caracteres acentuados do português. Portátil refeita com o executável único (ok, ainda não aberta).

### 26/10/2026 — desempenho, sessão Codex (em andamento)
- Lidos briefing, regras e histórico; criado ramo `desempenho-codex` de `nucleo-gow3` (`3d4c516`). Sem alterações em traduções, snapshots ou projeto do Godzilla. Referência privada `build/research/desempenho-codex-20261026/kyoei-reference.exe`, SHA256 `e4d6b5df075d394b93e42524315a2435e0363816c5973c641e499407e1dd43a4`.
- Davy fechou Wallpaper Engine e qBittorrent. Testes 1080p, Fifo, SDL dummy (mudo), `KYOEI_BENCH_STATS=1`, sem `GOW3_FRAME_STATS`. Primeira rodada aqueceu shaders; segunda foi descartada por uma leitura PowerShell de telemetria que ficou presa consumindo CPU (encerrada). Referência limpa `codex-sparse-abba-a1`: giro 41,61 FPS (últimas três janelas dentro do intervalo medido), máximo 66,32 ms, sem compilações durante a coleta. Resultado isolado, não prova de ganho.
- Candidata em validação: lista dos blocos de registradores alterados, mantida junto ao bitset, para evitar varredura a cada desenho e na captura do delta da preparação. Teste novo compara 2.000 replays com a cópia completa; 31 executáveis de testes passam (33 chamadas, incluindo modos e fixtures).
- A primeira compilação incremental misturou layouts: `vk_draw_prep.cpp.obj` e `vk_pipeline_cache.cpp.obj` continuaram datados de 22/10, ignorando o novo cabeçalho adaptado `regs.h`; falha em SearchBinaryInfo antes de abrir arquivos do jogo. Integração agora muda o comando de compilação quando o conjunto dos cabeçalhos adaptados muda, invalidando os objetos anteriores. Recompilação completa do núcleo e ABBA pendentes; candidata ainda não aprovada.
- Davy autorizou salvar e desligar normalmente o Windows ao restarem cerca de 7% da cota de 5 horas. Monitor privado `build/research/desempenho-codex-20261026/quota_watch.py`, vinculado somente ao rollout desta conversa (`01a17689-fad2-7222-84ce-5bb562d095f4`), lê telemetria real e recente, grava checkpoint ZIP verificado e diário antes do desligamento, sem `/f`. Checkpoint de ensaio validado. Para cancelar: criar `stop-quota-watch` na mesma pasta. Não estimar cota pelo relógio nem ler a de outra conversa.
- Após a recompilação completa, candidata abriu e terminou normalmente. ABBA curto (três últimas linhas do log, como no briefing): antiga 40,68; nova 43,11; nova 37,37; antiga 38,10 FPS. Agregado 39,34 → 40,03 (+1,75%), menor que a variação; **não é ganho comprovado**. Detalhes e critério alternativo por bytes em `docs/DESEMPENHO.md`. Coleta longa em andamento.
- Verificação no jogo antiga/nova (`GOW3_PIPE_VERIFY=16`) sem divergências de registros ou tabelas e saída 0 nas duas. Capturas após R, comparadas: nenhum defeito novo encontrado; desenho azul junto ao save também existe na referência e varia com ângulo/animação. Arquivos de próxima candidata, ainda sem integração/compilação: `gpu-gow3/kyoei_register_writes.h` e `src/test_kyoei_register_writes.cpp` (evitar marcar valores idênticos como alterados; não mexe no checksum dos pacotes).
- ABBA longo terminou: antiga 38,55; nova 34,17; nova 31,96; antiga 31,71 FPS. Referência também piorou entre pontas; lista descartada por não provar benefício, sem atribuir toda a queda à mudança. Guardada no checkpoint privado `checkpoint-20261026-024142.zip`; fontes, cabeçalho gerado e teste obsoleto removidos. Sondagem `GOW3_DRAW_PIPE=0`: 34,45 FPS, sem prova de ganho, não adotada.
- Avanço mantido: compilação coerente ao adicionar/remover cabeçalhos adaptados (fingerprint e manifesto com limite de caminho); teste do bloco real CMake passou em remoção/preservação/repetição/recusa de caminho externo. Medidor leve lê `draws_per_frame` do contador existente a cada 120 flips. Núcleo recompilado, 28 alvos atuais de testes passam (30 chamadas); binários antigos de frame_stats/stutter_trace, já sem fonte/alvo, removidos. Partida `codex-unchanged-abba-a1`, saída 0 e 39,86 FPS no giro; ~1.390 desenhos por quadro parado, 1.917–2.173 no fim do giro. Não é correção dos 60 FPS.
- 02:55: processo `gzport.exe` do Godzilla (teste M7, iniciado em 02:54:51) consumindo ~145% de um núcleo. Essa instância iniciou depois da última coleta Kyoei; não explica sozinha as variações anteriores. Perguntado a Davy se pode pausar os testes do Godzilla para as próximas medidas. Não encerrado nem alterado o outro projeto; compilações/revisão seguem enquanto se aguarda.
- Preparadas três candidatas **independentes**, sem ganho de FPS ainda validado: (1) `kyoei-unchanged.exe`, evita copiar/marcar registradores idênticos, checksum original e fila original; (2) `kyoei-checksum.exe`, xxHash já existente para pacotes de pelo menos quatro palavras, caminho original para os pequenos, identidade do pacote vazio preservada, sem mudar chaves persistidas; (3) `kyoei-completion.exe`, mesma fila de desenhos, espera bloqueante após os 4.096 spins existentes, conclusão publicada só depois do handler; nenhum fence/GpuIdle antecipado.
- Arquivos/hashes em `build/research/desempenho-codex-20261026/artifacts.json`: referência com métrica `d646c69107d3…`, unchanged `38783028d1f7…`, checksum `c2d08bbace71…`, completion `e53446f307f1…`. Cópias com PDB próprio. Referência original `e4d6b5df075d…` também preservada. `build/kyoei.exe` fica selecionado como a referência validada entre os testes.
- Verificação das candidatas: 60 mil escritas/3 mil replays completos para unchanged; 60 mil pacotes com cópia de dados/rebase e caminho pequeno/vazio para checksum; 30 mil handoffs reais entre duas threads, sem liberação de prefixo incompleto, com volta do anel e visibilidade do payload para completion. Os 30 alvos atuais de testes passam (32 invocações com modos/fixtures); teste extra do checksum usa exatamente o cabeçalho da candidata. Núcleos compilados. **Faltam ABBA completo e conferência visual no jogo antes de adotar qualquer uma.**
- Microbenchmark isolado do checksum, em ciclos da própria thread: xxHash perde em duas palavras (18,28 contra 11,51 ciclos), mas ganha de quatro a 64 palavras (14,68–97,54 contra 19,76–325,52). É custo sintético do checksum, não FPS de jogo; por isso o caminho pequeno foi preservado.
- Agendador privado `guarded_abba.py`: aguarda 60 s sem Godzilla, compiladores, Wallpaper Engine ou qBittorrent; fecha **somente** o Kyoei do log específico e invalida a rodada se houver interferência. Monitores de hardware e cota seguem privados. Não enviadas mensagens nem alterados processos/arquivos do Godzilla. Houve uma referência válida na preparação (`codex-unchanged-guard-064738-a1-r1`); tentativas posteriores foram descartadas por compilações. As comparações finais recomeçam sem alterações no meio.
- Corrigida disputa do nosso agendador com a ligação: `codex-build-in-progress` reserva o executável até cópia/hash; `artifacts.json` impede rodar variante errada. Nenhum resultado de FPS com executável incorreto foi aceito. Após retirar o teste de completion do CMake ativo, o manifesto removeu corretamente o cabeçalho gerado `vk_draw_pipe.h`.
- 07:05: Davy pediu interromper/salvar e revogou a interrupção na mensagem seguinte; trabalho retomado. Os monitores e a reserva real de 7% foram reativados. Fila atual: `guarded_abba.py unchanged checksum completion`, log `guarded-abba-resumed.log`. Fontes das candidatas ficam sem commit até a validação; CMake ativo contém apenas o patch unchanged, os outros dois são arquivos preparados/inativos. Commit validado da infraestrutura: `418f59f`.
- 07:52: 12 partidas ABBA válidas concluídas sem outros ports/compiladores. Agregados antiga → candidata: unchanged 47,84 → 47,11 FPS (−1,51%); checksum 47,36 → 47,42 (+0,13%); completion 45,96 → 46,85 (+1,94%). Duas primeiras descartadas e fontes retiradas após checkpoint ZIP verificado `checkpoint-20261026-075326.zip`; executáveis/logs/capturas preservados. Completion precisa repetir: as próprias referências variaram 44,81–47,16. Todas encerraram normalmente, nenhuma mudança adotada como solução dos 60 FPS.
- 07:53: perfil da referência iniciado sem verificador pesado nem GOW3_FRAME_STATS; medição de FPS continua separada. Código ativo retornou à referência (completion ainda é protótipo inativo). Monitores de cota/hardware continuam; cota real em ~40% usado, desligamento de 7% restantes ainda não atingido.
- Perfil normal: PostDraw 18,4% da thread de comandos, incluindo cópia de memória 9,8%; DrainDrawPipe 24,2%, GetGraphicsPipeline 9,6%. Incluem espera entre quadros, não são porcentagens de CPU ativa. Completion conferida no jogo com GOW3_PIPE_VERIFY=16 (`codex-completion-verify-0756`), sem divergências, saída 0 e imagens com R sem defeito novo evidente.
- 08:04: candidata independente vmahint compilada: apenas clamp/cópia tentam o último índice da tabela por thread, conferindo a entrada atual sob a mesma trava; fallback é a busca binária original. Teste do helper real com 3.000 alterações/relocações de tabela e quatro leitores de 100.000 iterações cada passou; todos os 30 alvos atuais passam (32 chamadas). SHA256 `00f9153a0096dd3864291092fc32a69c818e17a46c362d8dffada54b83711441`. Fila ABBA `vmahint completion`, log `guarded-abba-repeat.log`, sem mais compilações no meio. Resultado ainda pendente; fontes experimentais sem commit. Primeiros 12 resultados preservados em `guarded-abba-completed-070758.json`.
- 08:19: ABBA completo vmahint: antiga 45,47; nova 45,37; nova 45,00; antiga 44,44. Agregado 44,95 → 45,19 FPS (+0,53%): descartada por não demonstrar ganho. Evidências em `guarded-abba-completed-080432.json`/`result-vmahint.json`; fontes arquivadas no ZIP `checkpoint-20261026-082026.zip` e retiradas. Agendador encerrado **entre grupos**, antes de iniciar a repetição de completion, para preparar a próxima candidata.
- 08:23: cópia unificada de constantes (`constant-copy`) compilada. Só buffers pequenos sem escrita/alteração da GPU em toda a faixa pedida; limita e copia o prefixo contíguo com uma trava. Buracos/reservas interrompem o prefixo; origem física, buffers maiores, escrita e sobreposição da GPU mantêm as regras anteriores. Fixture inclui `kyoei_memory.c` real e VMEM real: 500 tabelas alteradas, 128 mil consultas aleatórias, fronteiras, remapeamentos físicos, vazio/overflow, imagem nativa e PAGE_NOACCESS idênticos a clamp + copy originais. 30 alvos atuais passam (32 chamadas). SHA256 `5b111a7ba35f67ded1d85184f92357c8a8d938499129ef7b64135d4a42017334`.
- Próxima fila: `guarded_abba.py constant-copy noring prep1 completion`, log `guarded-abba-constants.log`; compilação proibida até terminar. `noring` e `prep1` são opções já existentes, usando **o executável de referência**, respectivamente bit ConstantRing desligado e um worker de preparação. Cada grupo tem referências próprias. Resultado/adoção pendentes. Plano de energia apenas consultado: Equilibrado, sem alteração. Cota renovou naturalmente; monitor segue a telemetria real, não o tempo decorrido.
- 08:40: grupo constant-copy terminou 44,10 / 44,26 / 33,75 / 32,59 FPS (antiga/nova/nova/antiga). Agregado 37,48 → 38,30 (+2,18%) **não é ganho validado**: a referência caiu 26% entre pontas. Carga do host cresceu de ~59% para 73–74%; verificação em 08:40 encontrou Chrome usando ~1,6 núcleos. Não atribuir toda a queda ao Chrome nem à candidata sem isolamento. Imagens conferidas sem defeito novo evidente, ângulos variam. Dados em `guarded-abba-completed-082402.json`/`result-constant-copy.json`.
- 08:42: encerrada apenas nossa tentativa inicial de noring (`codex-noring-guard-082402-a1-r1`), antes de aceitar nova coleta. Perguntado a Davy se consegue pausar tarefas pesadas do Chrome; navegador/outros aplicativos preservados. Guard agora mede o CPU acumulado dos processos Chrome a cada passagem e aguarda 60 s abaixo de 0,8 núcleo, além das exclusões anteriores. Candidatas não aprovadas por esse grupo.
- Observador privado `thread_cpu_watch.py`: somente GetThreadTimes/GetThreadDescription, sem suspender ou alterar prioridade. Primeiro leitor não aceitava o código positivo de sucesso 0x10000000 retornado pelo Windows e mostrou nomes como `unnamed`; corrigido com teste na própria thread e cache dos nomes. Amostras antigas não identificam quais auxiliares usavam CPU. Nova leitura registra também o custo do observador, com nomes consultados no aquecimento.
- 08:54: candidata independente alignment compilada e 30 testes passam (32 chamadas). Só alignas(64): separa cursores A/B do ConstantRing e busy_cycles do contador de pacotes do DrawPipe. Sem mudar protocolo, comandos ou esperas. Hash `5e8faa8040b0cc52dc0aaf7c7d3a13cfbda5925311fa1de18a81411d93a07c33`; sources ativos só usam esses dois patches, não constant-copy/completion. Fila `prep1 noring alignment completion`, log `guarded-abba-threading.log`, aguarda Chrome/compiladores quietos; sem nova compilação durante os grupos. Todas continuam sem adoção de FPS.
- 08:58: gate do Chrome passou a usar média de ~10 s (mínimo 8 s), evitando que um único pico curto reinicie a espera. Primeira fila nem chegou a abrir uma partida; reiniciada com os mesmos quatro grupos em `guarded-abba-threading-v2.log`. Não alterado/fechado Chrome e nenhum aplicativo de terceiros. Nomes do observador consultados só uma vez por thread e depois guardados, reduzindo seu custo.
- Diagnósticos 09:02–09:21 (sem usar FPS): oito workers somaram ~0,54 núcleo, um worker ~0,36; comandos/DrawRec/VkRecorder ficaram em ~0,6–0,7 núcleo cada. Perfil do VkRecorder: ~59% espera, ~16% spin, ~9% cópia de buffers. Contadores existentes mostraram 95–130 mil cópias pequenas/s, 257–377 MB/s de streaming e 78–89 MB/s de reenvios nas regiões reportadas. Dados privados `result-thread-cpu-diagnostic.json`/`result-buffer-stats-diagnostic.json`. Observador de threads desligado antes de voltar ao FPS.
- Corrigida leitura do console privado: PowerShell grava `.console.log` em UTF-16; leitor usava UTF-8 e não encontrava tabelas de contadores. Log leve de FPS é outro arquivo UTF-8 e não foi afetado. Relidos os consoles originais de reference/sparse/completion com a codificação correta: nenhuma divergência do verificador.
- 09:35: primeiro limite de streaming 4 KiB (sobre alignment) compilou, passou nos testes e no jogo com verificador/encerramento 0; imagem após R sem defeito novo evidente. Streaming reportado ~49–56 MB/s, reenvios ~100–107 MB/s; diagnóstico com verificador, não comparar FPS nem taxas como ganho normalizado. Artefato inicial `f15417de...`; fontes arquivadas em `checkpoint-20261026-094959.zip`. ABBA interrompido para preservar a política original de endereços nativos, que não têm o mesmo rastreamento de escritas do heap.
- 09:55: versão revisada `alignment-stream4k-native`, SHA256 `9d8ee1230c784780714b656bab67198701b900e985d8fdf5f9641231f659b234`, mantém 16 KiB fora da faixa gerenciada/para faixas que atravessam seu limite; usa 4 KiB só inteiramente em [0x1000000000, 0xfc00000000). 31 alvos passam (33 chamadas); teste do helper real usa soma de 128 bits como oráculo para fronteiras/overflow e 100 mil casos. Linux conserva 16 KiB. Ainda sem ganho de FPS aprovado.
- Davy confirmou que precisa continuar usando o Chrome. Preferência respeitada: não fechar/silenciar nem bloquear indefinidamente os testes por ele. Novo modo privado `--allow-chrome` registra CPU média/máxima e cauda de ~16 s em cada rodada; demais concorrentes/compiladores ainda invalidam. Comparações serão repetidas quando a variação impedir concluir. Fila atual: `guarded_abba.py --allow-chrome --baseline=alignment alignment-stream4k-native`, log `guarded-abba-stream4k-native.log`; nenhuma nova compilação durante esse grupo. As antigas filas strict Chrome ficam só como evidência de tentativas interrompidas.
- ABBA completo `095604`: alignment 46,69; stream4k-native 45,90; stream4k-native 50,35; alignment 47,26 FPS. Agregados 46,97 → 48,02 (+2,23%), menor que a dispersão das candidatas; **ainda não adotada**. CPU médio do Chrome na cauda: 0,31 / 0,74 / 0,37 / 0,23 núcleo. Resultado/filas arquivados em `result-stream4k-native-first.json` e `guarded-abba-completed-095604.json`.
- 12:40: Davy informou que fechou o Chrome; confirmado nenhum processo/janela Chrome. Preparada fila contínua contra a referência original: `alignment-stream4k-native norecorder alignment prep1 noring completion pipe50 stream-device`, `--allow-chrome`, log `guarded-abba-closed-chrome.log`.
- 12:43: **Davy pediu pausar momentaneamente os testes para usar o Buzz.** Agendador parou e encerrou somente o Kyoei de teste PID 22836; zero processos do nosso jogo em 12:44. Monitores de hardware/CPU também parados; nenhuma rodada da nova fila aceita. Executável/PDB de referência validada restaurados e checkpoint ZIP salvo. Não retomar testes ou carga pesada até Davy liberar. Fontes experimentais seguem sem commit; desempenho de 60 FPS não resolvido. Monitor de cota de 7% continua conforme autorização anterior (última leitura ~54% usado).
- 13:30: Davy liberou após o Buzz. Fila retomada; referência 45,43 FPS e alignment-stream4k-native 47,87, apenas metade do ABBA. qBittorrent reabriu (~0,73 núcleo no intervalo observado); grupo não completado, sem ganho adotado. Evidência parcial em `guarded-abba-completed-133124-partial.json`.
- Nova candidata `alignment-stream4k-native-search`: busca de BinaryInfo filtra a primeira palavra da assinatura com std::find e só então aplica a validação completa original; mantém o caminho direto de salto ao cabeçalho. Busca vetorial limitada a páginas de 4 KiB para não ler uma página posterior antes de achar o cabeçalho. Sem cache de ponteiros ou hashes novos. Teste do helper real contra varredura original: 1.200 buffers aleatórios, falsos prefixos, primeira/última ocorrência e guarda PAGE_NOACCESS após cabeçalho. 32 alvos atuais passam (34 chamadas). SHA256 `59bba820eecbf617f6fa5a730a8d9828fa79121b4aae8738ba92600cabb4b611`.
- `codex-shader-search-verify-1351`: jogo/encerramento 0, sem divergências do verificador, imagem após R sem defeito novo evidente. FPS desse ensaio diagnóstico não conta. Preparada comparação suplementar `--allow-qb` com CPU do qBittorrent registrada, sem encerrar o aplicativo; ainda sem ABBA concluído da busca.
- 13:59: **Davy pediu interromper os testes e voltar mais tarde.** Fila `guarded-abba-shader-search.log` parou durante a primeira referência; apenas nosso Kyoei PID 10968 foi fechado, zero processos próprios confirmados. Monitores de desempenho parados, referência validada restaurada, fontes/documentação/checkpoint salvos. Não retomar testes/carga pesada sem liberação de Davy. Próxima fila preparada: `alignment-stream4k-native-search norecorder alignment-stream4k-native alignment prep1 noring completion pipe50 stream-device`; todas continuam sem aprovação de FPS. Monitor da reserva de 7% mantém a autorização anterior; última leitura ~11% usada na janela renovada.
- 16:08: Davy liberou a continuação. Testes mudos retomados, sem fechar Chrome/qBittorrent; carga dos dois registrada. ABBA `160854` da combinação alignment + cache 4 KiB + busca de assinatura: 38,56 / 33,75 / 39,37 / 41,46 FPS; agregados 39,95 → 36,34 (**−9,04%**). Combinação reprovada; esse grupo não isola a contribuição de cada componente.
- Mesmo grupo, `GOW3_VK_RECORD_THREAD=0`: 42,77 / 35,06 / 38,09 / 42,51 FPS; agregados 42,64 → 36,51 (**−14,37%**). Reprovado; conservar a thread de gravação. Ambos sem compilação de shaders na coleta. Comparação isolada de alignment + cache ainda em curso; nenhuma melhoria de FPS adotada.
- 16:54: ABBA de alignment + cache 4 KiB fechado: 39,47 / 39,93 / 45,19 / 40,52; agregados 39,99 → 42,39 (+6,01%). Sem compilação de shaders. qBittorrent na cauda: 0,85 / 0,48 / 0,17 / 0,20 núcleo; Chrome zerado. Candidatas variam 13%, portanto ainda sem aprovação. Estado de todos os 12 ensaios arquivado em `guarded-abba-completed-160854.json`. Retirados os seis patches experimentais do CMake normal (artefatos/fontes preservados); coleta curta da thread `guest main` iniciada para avaliar o custo de preparar a submissão.
- 17:14: controle sem ConstantRing (`noring`), ABBA `165832`: 40,60 / 36,59 / 38,57 / 40,04 FPS, agregados 40,31 → 37,55 (**−6,85%**). Reprovado. Fila parou entre grupos para compilar uma candidata isolada: flush normal do scheduler a cada 512 draws, somente fora de passes com clear, seguido de barreira completa no novo command buffer. Objetivo: sobrepor execução GPU e tradução CPU, sem adiantar GpuIdle, fence ou readback. Compilação/testes/verificação/ABBA pendentes; os patches anteriores continuam inativos.
- 17:20: `draw-batch512` compilado, SHA256 `8a65f7ea096c0fb9802fe8fb5892dcb41ffde2bcbe4b78be814352b3898da054`; todos os 28 alvos normais passam (30 chamadas). Alvos dos quatro protótipos anteriores retirados do CMake normal; fontes e resultados anteriores preservados nos checkpoints. Validação de sincronização Vulkan comparativa em curso; primeira tentativa da referência encerrou antes de inicializar GPU com mensagens do loader no stderr. Nova tentativa sem `VK_LOADER_DEBUG=layer` abriu normalmente e criou o log próprio da camada. Não usar FPS desses diagnósticos.
- 17:29: referência e `draw-batch512` encerram com 0 na validação Vulkan + PIPE_VERIFY=16, camada confirmada pelo módulo real do processo. Referência já tem VUIDs layout-07988, CopyImage-01548, Draw-07939 e Draw-09600; candidata apresenta os três primeiros, sem tipos novos, sem SYNC-HAZARD e sem divergências do verificador. Isso não prova correção dos defeitos anteriores. Capturas após R comparadas: sem defeito novo evidente; animação/texto variam. Resultado privado `batch-validation-result-1721.json` e `validation-*-1721/messages.log`. ABBA leve `guarded-abba-draw-batch512.log` iniciado; sem aprovação de FPS até fechar.
- 17:45: ABBA `draw-batch512` fechado: 44,08 / 36,77 / 35,95 / 39,25 FPS, agregados 41,53 → 36,35 (**−12,45%**). Reprovado; patch retirado do CMake normal, artefato/código preservados. Controle do mesmo exe com `KYOEI_GPU_BATCH_DRAWS=0` iniciado em seguida. Reutiliza explicitamente a última A (39,25) como primeira A: sequência temporal A1/B1/B2/A2/C1/C2/A3, dois ABBAs sobrepostos, não independentes. Origem marcada em `shared_reference_source`; grupo anterior arquivado em `guarded-abba-completed-172942.json`.
- 17:49: **Davy pediu pausar e avaliar a proximidade.** Fila parou entre rodadas; zero Kyoei aberto, observador de hardware parado, executável/PDB validados restaurados (SHA256 `d646c69107d3b7332e7319ae1f494d5d90de0540dbee8e16686cb663b03192d9`). CMake e fontes versionadas de produção voltaram à base validada; objetos de build ainda serão atualizados pelo Ninja na próxima compilação. Não abrir testes ou compilar até nova liberação.
- Controle `batch0` ficou parcial: referência compartilhada 39,25, mesmo exe com envio extra desligado 46,24 FPS (qBittorrent na cauda 0,18/0,30 núcleo; modo 0 confirmado nos metadados). Não declarar ganho nem ABBA concluído; evidência `guarded-abba-completed-174528-partial.json`. Retomada: terminar controles e conferir uma recompilação integral da base sem protótipos antes de inferir efeitos de código/layout. Nenhuma melhoria de FPS aprovada; 60 FPS estáveis ainda não resolvidos, sem prazo confiável. Regra de salvar/desligar nos ~7% da cota continua autorizada; última leitura 77% usados, janela real de 300 min.
- 18:35: Davy liberou nova continuação. Recompilação integral da base sem protótipos, 28 alvos normais/30 chamadas passam; artefato `baseline-clean`, SHA256 `c95cf34f8ce78f4a671c7eeae4273fbec052aa9bf43a3db68409aa8ddf1ae5f5`. Comparação de todas as seções PE e do arquivo completo: **idêntico à referência após normalizar somente timestamp COFF, timestamp do diretório de debug e GUID/age do PDB**; zero bytes restantes diferentes. Relatórios privados `baseline-section-comparison.json` e `baseline-code-equivalence.json`. Não havia uma alteração de código da base recompilada que explicasse o resultado isolado de batch0.
- `codex-clean-profile-1849`, base normal, sem verificador pesado: coleta de todas as threads guest + comandos/DrawRec (642 amostras por thread, perturbação maior que a coleta só de duas threads). Encontrada a thread guest que chama SubmitGfx: Build consome 9,8% das amostras dela, sobretudo inicialização/alocação de PreparedDraw[]. A coleta anterior só de guest main não cobria essa thread. Preparados, ainda **inativos e sem compilar/testar**, dois protótipos independentes: cópia de constantes sob uma trava por grupo (`kyoei_constant_copy_batch_cpp.patch`, API/fixture com remapeamento concorrente); reutilização limitada a 16 MiB de arrays PreparedDraw somente após o último dono da Submission liberar (`kyoei_prepared_draw_pool_cpp.patch`, helper e teste de posse/concorrência). O segundo preserva o tipo/layout original de Submission e só adapta o .cpp. Priorizar este custo medido na retomada; não misturar as duas candidatas.
- 19:07: **Davy pediu encerrar e pausar para dar aula.** ABBA da preparação desligada interrompido na última referência; apenas nossa execução foi fechada. Parcial: base 36,08; prep0 34,51/27,90 FPS; sem referência final, sem conclusão/adoção. Arquivo `guarded-abba-completed-185434-partial.json`. Observador de hardware parado, zero Kyoei aberto, exe/PDB de referência `d646c691…` restaurados e hash conferido. Fontes versionadas de produção/CMake continuam na base, sem protótipos ativos. Não compilar nem abrir testes até Davy liberar.
- Telemetria do monitor original ficou obsoleta após continuação da sessão; sua regra de frescor impediu usar o número antigo. Confirmado o arquivo filho de nossa thread (parent_thread_id original, mesmo cwd), `rollout-2026-10-26T16-42-44-01a179bc-9467-7c63-a51a-bf3490f0c1ff.jsonl`, com leitura atual de 19% usados em 19:07, janela renovada de 300 min. Monitor privado atualizado para esse arquivo e reiniciado, mantendo a autorização de salvar/desligar perto de 7% restantes.
- 21:41: Davy liberou a retomada. Ativada **somente** a reutilização de arrays PreparedDraw: `kyoei_prepared_draw_pool_cpp.patch` e helper com oito entradas/16 MiB de armazenamento livre, devolução apenas no último release da Submission, estados recolocados em Pending antes da publicação. Tipo/layout de Submission e protocolo de fences permanecem iguais. Teste do helper de produção cobre último dono, destruição do pool com submissão viva, limite de memória, overflow e 8.000 trocas em quatro threads; 29 alvos atuais passam (31 chamadas). Artefato `prepared-pool`, SHA256 `9c0ee2b154409e0c87af88f7d56fef35f80b95365e0b841f92d0e3676d7fea82`.
- `codex-prepared-pool-verify-2146`: saída 0, GOW3_PIPE_VERIFY=16 sem divergências, capturas após R comparadas com a base sem defeito novo evidente. ABBA normal contra `baseline-clean` iniciado em 21:50, log `guarded-abba-prepared-pool.log`; adoção de FPS ainda pendente. Nenhuma compilação durante o grupo. Monitor de cota recebeu leitura fresca de 28% usados em 21:50.
- Grupo `215019` completo: 36,44 / 32,93 / 41,21 / 35,81 FPS; agregado 36,12 → 36,61 (+1,34%). **Não aprovado**: grande dispersão e foco inconsistente (perdas 28/28, 29/29, 0/29, 9/28), além de CPU do host elevada (picos próximos de 100%; Brave/Plex/webcam em uso). Não atribuir a variação ao cache nem aos aplicativos isoladamente. Helper/patch/testes do pool preservados, mas retirados da compilação normal. Dados em `guarded-abba-completed-215019.json`.
- Encontrado `Common::AccurateSleep` usando CreateWaitableTimer comum na apresentação/vblank. Protótipo isolado `kyoei_accurate_timer_cpp.patch` usa timer de alta resolução com reset manual e fallback original em Windows sem suporte. Nada de alteração de fence/GpuIdle, simulação, resolução ou opções gráficas. Compilação e 28 alvos normais/30 chamadas passam; artefato `accurate-timer`, SHA256 `1ce7a68f27646fb83aa1d29d0e308fcf90bb58633d31b6f712356809369711ba`.
- Programa privado `probe_accurate_timer.py` extrai as funções original/adaptada reais e testa APC interruptível/não interruptível, prazo zero, reset manual e ausência de vazamento de handles: PASS. Janela invisível, sem timeBeginPeriod; médias para pedidos 1/4/8/16 ms: original 15,31/15,56/15,43/28,13; candidata 1,60/4,38/8,36/16,32 ms. São tempos de espera do teste nativo, **não FPS**. Log/JSON em `timer-precision-probe.*`.
- `codex-accurate-timer-verify-2217`: saída 0, verificador sem divergências e imagem após R sem defeito novo evidente. ABBA normal iniciado em 22:25 (`guarded-abba-accurate-timer.log`); ainda sem resultado adotado. Novo wrapper transmite progresso/FPS/foco diretamente, evitando consultas de terminal durante as janelas medidas. Avaliar consistência de foco antes de atribuir efeito. Monitor de cota continua ativo com dados frescos desta continuação.
- Grupo `222557` interrompido: 41,59 / 45,66 / 40,45 FPS, sem A2; foco 29/29, 0/29, 0/29. Parcial inválida para atribuir ganho, preservada em `guarded-abba-completed-222557-partial-focus-mixed.json`. Executável/PDB de referência restaurados. Criada cópia privada `route-background.ps1`: mesmos save, resolução, entradas e tempos da rota, mas sem SetForegroundWindow, ShowWindow com SW_SHOWNOACTIVATE e hints SDL de ativação desligados apenas no ambiente do teste. Nova comparação `guarded-abba-accurate-timer-background.log` iniciada em 22:41; exige todas as verificações de foco em segundo plano nas quatro rodadas.
- Correção do monitor de cota: o arquivo filho usado desde 19:09 era da revisão automática, não uma continuação principal. Os valores frescos de então pertenciam ao mesmo limite, mas ficaram obsoletos. Monitor voltou ao rollout original desta conversa, que continua recebendo telemetria: 59% usados em 22:42. Agora valida o ID principal e recusa qualquer sessão filha. Mantidas a regra de frescor, o checkpoint e o desligamento normal previamente autorizados.
- Grupo `224141` também parcial e inválido: 42,77 / 46,05 / 44,80 FPS, foco 0/29, 0/29, 29/29; dicas SDL + SW_SHOWNOACTIVATE não impediram ativação inicial. Evidência arquivada. Rota privada `route-noactivate.ps1` aplica WS_EX_NOACTIVATE só à janela de teste, minimiza/restaura uma vez antes de carregar/aquecer e registra handles/zeros em cada consulta. Novo ABBA `225419` iniciado em 22:54. A1: 41,89 FPS, 29/29 em segundo plano, handles reais diferentes e não nulos em todas as consultas; aguardando B1/B2/A2. Não alterar o atalho/janela normal.
- Grupo `225419`: A1 41,89, B1 46,25, B2 41,94 FPS; B2 foco misturado 11/29. A2 encerrada antes da coleta, só nossa instância. Achado erro da rota: PowerShell considera `[bool]([IntPtr]::Zero)` verdadeiro, portanto a checagem original de MainWindowHandle tentava ShowWindow/estilo no handle zero antes da janela existir. Correção em `route-noactivate-v2.ps1`: comparação explícita com IntPtr.Zero, leitura de volta/validação de WS_EX_NOACTIVATE, registro de estilos e handles em todas as consultas. Grupo `230754` iniciado 23:07; mensagem de abertura confirma hwnd real 2691158, estilo 134217984 (0x08000100), foco em outra janela. SHA256 da rota `7feb7f9cde137652387f8f5a78d04658e4796de7dffc50c0562d7d0398bcd785`. Quatro rodadas pendentes; nenhuma conclusão de FPS do timer ainda.
- 23:23: ABBA `230754` completo e foco uniforme: 48,11 / 42,44 / 39,78 / 38,23 FPS, agregado 42,60 → 41,06 (**−3,61%**). Todas as quatro rodadas 29/29 em segundo plano, handles não nulos, estilo 0x08000100 constante, zero compilações na coleta. Host CPU durante giro: 57,6 / 62,4 / 67,7 / 75,1%; GPU 1860 MHz constante, 77–79 °C. Forte queda também na referência; não atribuir causalmente toda a queda ao timer. **Sem benefício de FPS comprovado: patch retirado do CMake normal**, helper/patch/probe/artefato preservados. Resultado `result-codex-accurate-timer-guard-230754-a1-r1.json` e contexto `conditions-...json`. Exe/PDB normal restaurados (`d646c691…`), hash conferido. Repetir agora prepared-pool contra baseline-clean usando a rota corrigida; a comparação anterior dele tinha foco inconsistente.
- 23:39: ABBA do pool `232359` completo: 37,77 / 43,81 / 47,57 / 33,85 FPS, agregados 35,70 → 45,62 (+27,77%). Foco uniforme 29/29, handles/estilo válidos. **Não atribuir ainda todo o ganho ao pool**: CPU total no giro 74,8 / 69,1 / 58,4 / 75,2%, maior nas referências. Binário/PDB antigo restaurado pelo guard. `probe_prepared_pool.py` usa PreparedDraw real (1.016 bytes), três submissões simultaneamente retidas e 1.000 alocações por bloco ABBA: para 2.048 desenhos original 1,065/1,105 ms e pool 0,039/0,036 ms; ciclos da própria thread confirmam redução (2,75–2,85 milhões → 91,7–100,9 mil). Isso mede a alocação/inicialização, **não FPS**. Repetição invertida pool/base/base/pool `234049` iniciada em 23:40; guard agora registra CPU por processo (Kyoei, Brave, Plex/QtWebEngine, webcam, OBS, Chrome, qBittorrent etc.) sem fechá-los nem alterar prioridades. Produção segue sem protótipos ativos até concluir.
- 23:36: telemetria da conversa principal renovou a janela de 300 minutos (0% usados; 2% em 23:39, novo reset 1793086554). Monitor continua condicionado a 93% usados na janela real, sem estimar pelo relógio.
- 23:49–23:54: compilações externas apareceram entre B1/B2 da confirmação; guard aguardou, preservando as duas rodadas completas (pool 41,38; base 37,69 FPS). Aproveitada a pausa para reativar somente a integração do pool e seu teste no CMake e recompilar com dois jobs. 29 alvos/31 chamadas passam. Artefato `prepared-pool-rebuilt`, SHA256 `8e75be8d084f3c84157593bd5cdf1b8936b800bcad589cfadb8ea3ab4ffdd34f`: **idêntico ao artefato prepared-pool em todos os bytes após normalizar somente timestamps COFF/debug e identidade PDB**, zero bytes diferentes. Relatório privado `prepared-pool-rebuilt-equivalence.json`. Exe/PDB de referência restaurados antes de remover marcador de compilação; guard retomou B2 às 23:55. CMake está com a candidata pronta, ainda sem commit/adoção de FPS até terminar a confirmação.

### 27/10/2026 — reutilização de preparação mantida; 60 FPS ainda pendentes
- Confirmação invertida `234049`, pool/base/base/pool: 41,38 / 37,69 / 42,61 / 42,19 FPS. Normalizando para base → pool: **40,00 → 41,78 (+4,45%)**, quatro rodadas 29/29 em segundo plano, handles/estilo válidos. Guard aguardou compilações entre as duas rodadas centrais; nenhuma durante a coleta. CPU qBittorrent no giro 0,280 / 0,229 / 0,227 / 0,142 núcleo; demais aplicativos acompanhados abaixo de 0,01. CPU do jogo 6,34 / 6,29 / 6,05 / 5,96 núcleos, host 70,8 / 69,2 / 62,1 / 63,0%. Grupo anterior marcou +27,77% com carga menor nas candidatas; **não divulgar 27,77% como ganho exclusivo do código**. Mesmo +4,45% é uma estimativa sob variação, não promessa estável.
- **Mantida a reutilização** pela redução direta comprovada no tipo real (1,085 → 0,037 ms para 2.048 desenhos), testes de posse/concorrência, imagem e dois grupos com agregado favorável, sem antecipar fences/GpuIdle. Retorno do array somente no último dono, cache livre limitado a 16 MiB/oito entradas, payload só legível depois de Ready. Testes 29 alvos/31 chamadas passam; jogo/verificador/capturas de `codex-prepared-pool-verify-2146` sem divergências/defeito novo. Executável recompilado é equivalente integralmente ao testado, excluídos só metadados de link/PDB. Escolhido `prepared-pool-rebuilt` (`8e75be8d…`) como novo padrão validado; guard passa a restaurá-lo entre testes. **Ainda não resolveu os 60 FPS no giro.** Próxima candidata independente: constantes agrupadas sob uma consulta de mapeamento por grupo; fontes/testes preparados, integração/compilação ainda pendentes.
- 00:04: avanço salvo em `8dac8cf`. Iniciada compilação da candidata **constant-batch** sobre o pool validado: helper C no runtime adaptado e patch somente no CollectRingBindings. Alocações/consultas GPU fora da trava, uma trava de leitura do mapeamento por grupo, cópia do prefixo válido, ordem das bindings preservada e fallback para os demais casos. Acrescentada proteção contra soma de endereço com overflow antes das consultas GPU. Fixture estendida compara 128 mil leituras e grupos aleatórios/nested e 2.000 grupos de 16 leituras durante remapeamento concorrente. Integração compilando; estes testes estendidos e FPS ainda pendentes. O helper/patch anterior de cópia simples continua inativo; o timer continua inativo. Marcador impede novos jogos durante a compilação e o script restaura `prepared-pool-rebuilt` após preservar a candidata.
- Constant-batch compilado, SHA256 `a8b1c727d33f8f901b71b15910c031cd62c83008f442d7cb0f28d11101cbce9d`; 30 alvos/32 chamadas passam. Acrescentado e executado caso de grupo misto com fontes nativas, guest sem mapeamento, overflow, tamanho zero e páginas PAGE_NOACCESS. Verificações de jogo `codex-constant-batch-verify-0010` e `...-verify-1080-0015`: saída 0, PIPE_VERIFY=16 sem divergências, capturas após R sem defeito novo evidente, animação/vídeos/texto variam. Não usar seus FPS.
- Correção de protocolo: capturas mostraram que a rota com minimização/estilo tinha cliente **1920×1050**, embora ambiente/criação pedissem 1080. Os grupos recentes do timer/pool com essa rota continuam comparando o mesmo tamanho entre A/B, mas **seus FPS não devem ser rotulados como saída 1080p exata**. O código/imagem normal do pool já havia sido verificado em janela 1080 e sua redução de alocação continua válida. `route-noactivate-1080.ps1` com SetWindowPos comum também continuou em 1050. Probe SDL independente, fora da tela e sem dispositivo Vulkan (`probe_window_size.py`): criado/restaurado 1920×1080; SetWindowPos comum reduz para 1050; com SWP_NOSENDCHANGING fica 1920×1080. `route-noactivate-exact1080.ps1` usa esse ajuste antes do aquecimento e registra dimensões em todas as consultas; medidor recusa qualquer amostra fora de 1920×1080. ABBA constant-batch contra pool validado `002311` iniciado em 00:23; primeira tentativa encerrada antes de medir por compilação externa, depois teste gzport do Godzilla. Guard aguarda sem tocar no outro projeto.
- Probe de cópia real sem jogo/FPS (`constant-batch-probe.*`), 16 buffers por grupo em ABBA: guest 512 B ~1,23 → 0,51 µs/grupo, guest 2 KiB ~3,29 → 1,62 µs, guest 8 KiB ~8,25 → 7,39 µs; nativo ~1.500 → 760 µs e misto ~760 → 385 µs. Ciclos da própria thread também caem. É custo isolado da API, não FPS; benchmark executado enquanto medições de jogo aguardavam o outro port. Nenhuma adoção da nova cópia ainda.
- Preparada candidata separada `constant-batch-native`, SHA256 `1b8a1b73f79613a6b8b0b9830d3ed416f2bab6c7291bbc1174bc755ed502351b`: reutiliza somente a descrição de uma região nativa dentro da chamada de grupo, nunca os dados nem informação entre quadros. Teste de fronteiras alterna regiões acessíveis/PAGE_NOACCESS, ordem de fontes e cópias parciais; 30 alvos/32 chamadas passam. Probe separado `constant-batch-native-probe.*`: 16 fontes na mesma região nativa ~1.500 → 48–57 µs/grupo; misto ~750 → 47–56 µs. Custo isolado, **não FPS**, jogo/imagem/ABBA dessa variante ainda pendentes. A primeira candidata `a8b1c727…` foi preservada e continua sendo a que está na fila de medição.
- Davy pediu priorizar este projeto e autorizou explicitamente: **“se o godzilla abrir te autorizo fechar”**. Monitor privado `close_godzilla_watch.py` mira somente o caminho verificado `G:/godzilla-port-codex/build/gzport.exe`, pede WM_CLOSE e encerra esse mesmo processo se não responder em 15 s. Não altera arquivos ou compiladores do outro projeto. Eventos registrados em `godzilla-close-history.jsonl`; guard recusa qualquer sobreposição, mesmo se a abertura/fechamento ocorrer entre duas consultas. Permissão vale para priorizar os testes atuais do Kyoei; parar o monitor se o trabalho for pausado.
- Correção da resposta sobre FPS: a tecla L não gira mais a câmera no pad do Kyoei; o gatilho interno era removido antes dos dois segundos finais/captura. Assim, as três últimas janelas da rota antiga **podiam incluir trecho parado**. Números anteriores não provam giro puro; o pool continua mantido pela redução direta de alocação e testes, com ganho de FPS exato ainda pendente de confirmação limpa. `route-pure-turn-1080.ps1` mantém o gatilho até o jogo encerrar e exige cinco janelas após ativá-lo, descartando margem de duas e garantindo pelo menos as últimas três inteiras em giro. Dimensões físicas conferidas em todas as consultas; medidor recusa qualquer falha desses requisitos. Grupo anterior `005241` ficou parcial (36,73 / 43,02; restante interrompido) e foi arquivado como `...partial-tail-not-guaranteed.json`; não concluir. Novo ABBA puro iniciado em 01:02 (`guarded-abba-constant-batch-pure1080.log`).
- **Novo limite pedido por Davy em 27/10: salvar e desligar com 10% restantes da cota real de 5 horas**, substituindo os 7% anteriores. Monitor alterado para disparar aos 90% usados, mantendo validação da conversa principal, telemetria recente, checkpoint verificado e desligamento normal sem `/f`. O pedido não pausa o trabalho agora.


### 27/10/2026 01:33 — reserva de cota da sessão Codex
- Telemetria real de 5 horas: 9% restantes. Davy autorizou salvar e desligar o PC.
- Checkpoint privado em build/research/desempenho-codex-20261026/checkpoint-*.zip; estado exato do ramo, diff e arquivos de código/documentação preservados. Desligamento normal do Windows solicitado sem forçar aplicativos.

### 27/10/2026 — consulta de andamento
- Davy perguntou pelo estado do projeto. Diário consultado: reutilização de PreparedDraw mantida; 60 FPS estáveis no giro ainda não comprovados. Candidatas de cópia agrupada de constantes continuam dependendo de confirmação limpa no jogo; não apresentar probes isolados ou medições antigas com trecho parado como ganho de FPS.
- Nesta consulta não houve compilação nem execução do jogo. O terminal apresentou falhas intermitentes de inicialização; leitura do diário concluída, mas estado atual do Git e resultados privados finais da fila não confirmados. Resposta baseada no último registro, com checkpoint das 01:33 preservado.

### 27/10/2026 — retomada após o desligamento
- Davy liberou a continuação. Confirmado que o grupo puro `010202` terminou antes do desligamento: pool 42,19 / constant-batch 43,81 / constant-batch 42,35 / pool 42,27 FPS, todas as rodadas integralmente em segundo plano e com cliente 1920×1080. Diferença pequena (~2%); ainda sem adoção isolada da primeira candidata. Relatórios privados já salvos em `result-codex-constant-batch-guard-010202-a1-r1.json` e `conditions-...json`.
- O desligamento deixou `constant-batch-native` no caminho de execução; restaurada primeiro a referência validada `prepared-pool-rebuilt`, com hashes dos três artefatos conferidos. Iniciada verificação da variante nativa, muda, em segundo plano, antes de medir seu desempenho.
- Monitor da reserva de 10% atualizado para a conversa principal atual (`01a17e34-305a-79c0-ac25-ccd0975c8b58`, cwd conferido, sem sessão pai; leitura fresca 3% usados). Reiniciados observadores privados de hardware e do Godzilla conforme autorização anterior. Guard passou a comparar eventos Godzilla por relógio civil, pois timestamps monotônicos do boot anterior não são comparáveis após reiniciar o PC. Nenhum arquivo do projeto Godzilla alterado.
- Grupo nativo `134434` concluído em 14:06: pool 47,79 / candidata 47,16 / candidata 47,68 / pool 46,55 FPS; agregado **47,16 → 47,42 (+0,55%)**, insuficiente para comprovar ganho. Foco uniforme, cliente 1920×1080 e zero compilações de shaders nos intervalos medidos. Compilações externas interromperam duas tentativas, descartadas e repetidas. Wallpaper Engine preservado e sua carga registrada (~0,044–0,080 núcleo), assim como qBittorrent (~0,096–0,170). Máximos ~49–51 ms em ambas, sem melhora demonstrada. Executável/PDB de referência restaurados ao terminar a comparação; integração experimental ainda depende da decisão final.
- Verificações `codex-constant-native-verify-resume-1341` e controle `codex-pool-capture-control-1342`: saída 0, PIPE_VERIFY=16 sem divergências. Captura PrintWindow ficou preta nos dois; não declarar PASS visual. Iniciada foto nativa da candidata; próximo diagnóstico inclui perfil da CPU com giro mantido durante a coleta, sem usar seus FPS.
- Fotos F12 nativas de `codex-native-photo-1407` e `codex-pool-turn-profile-1411` conferidas: cenário, personagem e HUD comparáveis, sem defeito novo evidente; vídeo/animação/texto variam. Foto exige foco breve depois da coleta; o auxiliar devolveu o estilo NOACTIVATE, mas SetForegroundWindow da janela anterior foi recusado pelo Windows; ao encerrar o teste o jogo saiu normalmente. Não foi alteração de foco nas quatro medições de FPS, concluídas antes. Erro transitório WinError 32 ao restaurar o exe depois da primeira foto: processo já terminou, próxima cópia da base passou; acrescentado retry limitado a 10 s apenas para esse erro e confirmação de hash no diagnóstico privado.
- **Cópia agrupada retirada do CMake normal**: +0,55% não comprova melhora, máximos equivalentes. Fontes/testes/probes e artefatos experimentais preservados. Recompilação sem o experimento, `pool-after-batch-retired`, SHA256 `f56bf5fb3cf52add0a19744cdd06e3effcff00550ae091243318f1a011d09231`: 29 alvos/31 chamadas passam; arquivo inteiro idêntico ao pool validado após normalizar exclusivamente timestamps COFF/debug e GUID/age PDB, zero bytes restantes diferentes. `CMakeLists.txt` e `gpu-gow3/CMakeLists.txt` voltaram ao HEAD validado.
- Perfil `codex-pool-turn-profile-1411` manteve giro na coleta, mas incluía PIPE_VERIFY=16: suas amostras em cópia/comparação do arquivo completo de registradores são custo do diagnóstico e não justificam otimização do modo normal. Repetição limpa `codex-pool-normal-turn-profile-1420` iniciada com verificador desligado, só três threads do núcleo, giro mantido por mais 60 s e janela de amostragem posterior ao carregamento/aquecimento. Seus FPS também não devem ser usados como benchmark normal.
- Perfil normal `...-1420` concluiu com saída 0 (9.339 amostras por thread). Encontrada conversão implícita em GetDrawOffsets: recebe `optional<FetchShaderData>`, mas GraphicsPipeline::GetFetchShader retorna referência a `optional<const FetchShaderData>`. A conversão faz cópia profunda do vetor de atributos a cada draw; construtor aparece em 1,9% das amostras da DrawRec, mesmo sem upscaler. Duas chamadas atuais conferidas. Nova adaptação `kyoei_draw_offsets_reference_cpp.patch` só faz o parâmetro corresponder ao tipo const já mantido pela pipeline; sem alterar cálculos ou vida dos dados.
- Probe privado com função original/adaptada extraídas e tipos reais Regs/Info/FetchShaderData: **12.000 resultados idênticos**, incluindo optional vazio/presente, vetor vazio/não vazio, offsets de registrador e UserData com/sem snapshot. Alocações durante as chamadas: **10.268 → 0**. Fontes, hashes, log e JSON `draw-offsets-probe.*`; não é FPS. Compilação, testes e verificação no jogo da candidata isolada `draw-offsets-reference` iniciados; FPS/adoção pendentes. Cópia agrupada permanece inativa.

### 27/10/2026 — executável para Davy testar; testes em segundo plano encerrados
- Davy revogou explicitamente o fechamento do Godzilla e pediu parar de abrir o Kyoei em segundo plano, pois as janelas apareciam na área de trabalho. Pediu o executável. Novas regras registradas em AGENTS.md; monitor de fechamento encerrado, arquivo de parada preservado, script privado bloqueado contra reativação e retirado da inicialização da retomada. **Não fechar Godzilla e não reiniciar testes do Kyoei em segundo plano.**
- Encerrado somente o teste próprio `codex-offsets-reference-verify-1425` (PID 19444). O jogo chegou à cena/giro e saiu com código 0; sem divergências do PIPE_VERIFY no console. Captura final interrompida pelo pedido; o harness encerrou com erro esperado por já não existir o processo. Não declarar protocolo completo de imagem nem FPS da nova candidata.
- `draw-offsets-reference` compilado: SHA256 `d74ad4c329af46a1ae73d163b9ce1f38f26e2e17a8c99d63e382e5d7cbc66b15`; 29 alvos/31 chamadas passam, além do probe de 12.000 casos idênticos e zero alocações novas na chamada. Mantida a correção pequena de tipo; ganho de FPS ainda não medido. 60 FPS estáveis continuam pendentes.
- Runtime da portátil pessoal atualizado com essa compilação usando `tools/update_portable_runtime.py`; executável para abrir: `build/City-Shrouded-in-Shadow-Portable/City Shrouded in Shadow.exe`. Launcher --check retornou 0 sem abrir o jogo. Saves, configurações, tradução e áudio do uso normal preservados. Davy fará o teste manual; não abrir o jogo para ele nesta entrega. Referência anterior do pool segue preservada nos artefatos privados para comparação/retorno, sem sobrescrever saves.
- Runtime da portátil após a limpeza habitual de metadados PE: SHA256 `713331fb8f60ba19821dd34ddf0daf9810483599eb5a55049437f96aa8ee3e66`, registrado no manifesto da portátil. Não confundir com o hash do executável de desenvolvimento acima.
- Observadores de hardware e de cota encerrados ao terminar esta entrega manual. A regra anterior de reserva de 10% não foi revogada para trabalho futuro; não houve desligamento nesta entrega.

### 27/10/2026 — balanço solicitado por Davy
- Davy perguntou se houve progresso nos últimos dias. Balanço: a correção de 25/10 recuperou a regressão (~33 → 57–59 FPS parado nos testes registrados). Em 26–27/10 ficaram a reutilização de PreparedDraw e a eliminação da cópia implícita de FetchShaderData; reduções de trabalho comprovadas, mas sem novo ganho significativo de FPS demonstrado no giro. Experimentos descartados e correções dos próprios protocolos consumiram parte relevante do tempo. Não apresentar atividade de diagnóstico como solução dos 60 FPS.
- Apenas consulta do histórico e registro; nenhum jogo ou monitor reiniciado.

### 29/10/2026 — updater dos amigos (jogo + tradução)
- Gerado `build/Kyoei-Update-Jogo-e-Traducao-20261029.zip` (SHA256 `162179dc…`) a partir da portátil pessoal: runtime `713331fb…`, launcher e tradução de 26/10. Gerador ganhou `--translation-update` para aceitar quem aplicou a tradução de 24/10 (runtime `50af4ebb…`, antes recusado). test_translation_update PASS Windows/Linux. Jogo não aberto.

### 29/10/2026 — piloto pt-BR jogável
- Fonte principal (utf8all) tem todos os acentos; a do intermission (utf8all_IM) não tem ÀÁÉÓÚàáâãçéêíóôõú• (M3 pendente para essa tela).
- Traduzidos os 733 textos da primeira fase (Text_Stage_201 inteiro, NPCs, Text_Common, TextData, hud_data, operation_text), no estilo natural do inglês. Rascunho `translations/localization-pt-BR.json`, revisão humana pendente. Largura estática: as 25 linhas mais largas que o p99 do inglês (1358) foram encurtadas/quebradas; nenhuma restante acima disso.
- `tools/merge_language_fallback.py`: textos sem pt-BR usam o inglês aprovado. Prévia `runtime-preview-pt-01` (13.269 textos, 168 texturas inglesas reaproveitadas) e `languages/pt-BR/translation.kydelta` na portátil. Prévia antiga runtime-preview-02 (6 GB) apagada.
- Runtime: pt-BR liga os mesmos recursos do inglês (ajuste do HUD, nomes latinos, textos do executável em inglês, títulos do teclado de nome em português). Launcher mostra "Português (Brasil) · prévia" só se o delta existir; usa botões/textos do executável em inglês. build_portable gera o delta pt-BR quando há prévia.
- Testes names/ime_overlay/overlay/text_wrap/hud_trace PASS; pc_names falha igual sem as mudanças (pré-existente). Launcher --check 0. Jogo não aberto: Davy testa.
- Andamento pt-BR (29/10, mesma sessão): fases 201–207 completas (diálogos, NPCs, itens, notícias), 4.485 textos em `translations/localization-pt-BR.json`. Fluxo: `translate_batch.py show/apply` no workspace privado; linhas que cruzam lotes vão por um roteador temporário no scratchpad. Nomes do glossário (Ultraman, Alien Zarab) mantidos oficiais. Glossário informal usado: Labor, Ingram, Anjo, Café da Casa, Ovo Cozido, Água do Sopé do Monte Hakusan, Bentô Cerejeiras em Flor, Sanduíche Torre Iberis, Ministro do Karaage, A Saga da Última Espada, Cidade Absolutamente Segura, Notícias/DICAS da Sombra Gigante, energia (ST). Entradas sem inglês aprovado ficam anotadas e não traduzidas.
- Pausa pedida por Davy: fases 201–208 com diálogos completos; falta a notícia da 208 (`translate-pt-BR-0065`, 128 textos) e daí em diante 209–218, Text_Heroine, itens, missões, mapas etc. Retomar por `translate_batch.py status`. O jogo ainda não foi reconstruído com esta versão: rodar merge_language_fallback + build_runtime_translation (--language pt-BR --textures-en) + delta para a portátil antes do próximo teste.

### 29/10/2026 — pt-BR completo e prévia reconstruída
- Tradução pt-BR concluída: 16.136/16.136 textos do workspace preenchidos (todas as fases 201–218, Text_Heroine, itens, missões, objetivos, leituras, e-mails, dicas, créditos e monólogo final). `translations/localization-pt-BR.json`: 15.484 entradas; o restante são notas sem tradução (textos de teste do desenvolvedor, notas internas entre `<>`, nomes próprios de pessoas nos créditos sem leitura confirmada). Revisão do Davy jogando pendente.
- `translate_batch.py apply` mantém o `
` literal quando o original CSV o guarda assim. `merge_language_fallback.py` agora inclui textos pt-BR que o inglês nunca cobriu (≈2.300, quase todos colunas de item_data) em vez de recusar.
- Prévia `runtime-preview-pt-02` (15.578 textos, 168 texturas inglesas de textures-en-natural), roundtrip verificado; `docs/m4/runtime-preview.json` aponta para ela (entrada en inalterada). Delta `languages/pt-BR/translation.kydelta` da portátil trocado (8.328.579 bytes, SHA256 64a9b4a5…, verificado). Launcher --check 0. Prévia pt-01 (6 GB) apagada. Jogo não aberto: Davy testa.
- Pendências: fonte do intermission (utf8all_IM) sem acentos (M3); texturas ainda em inglês; nomes de pessoas nos créditos seguem em japonês no pt-BR (no inglês também estavam vazios).

### 29/10/2026 — 94 textos restantes em pt-BR, portátil e updater
- Comparação en × pt: no inglês não falta nada visível (as ~2.300 entradas só-pt são linhas reservadas "仮登録" de item_data e textos de teste). No pt faltavam 94 que apareciam em inglês: abertura da 201 (70), objetivos do parque em map_goal_text (9) e descrições do menu de opções (15). Traduzidos. Corrigido Text_Stage_207 p0/k11 (texto de teste que tinha recebido frase errada) para "(Teste)...1".
- Prévia `runtime-preview-pt-03` (pt-02 apagada); delta pt-BR da portátil 8.328.331 bytes, SHA256 2ed89aa0…, verificado; launcher --check 0.
- Updater `build/Kyoei-Update-Jogo-e-Traducao-PTBR-20261029.zip` (34.331.307 bytes, SHA256 5f8e0888…): agora leva também `languages/pt-BR/translation.kydelta`; aceita quem tem a GoW3 de 24/10, a tradução de 24/10 e o updater anterior de 29/10 (apagado, substituído). test_translation_update PASS Windows/Linux. Jogo não aberto: Davy testa.

### 02/11/2026 — preparação da 1.0 e nova reserva de cota
- Davy retirou desempenho e campanha completa dos bloqueios para a 1.0 e autorizou revisão geral da tradução, diagramação/narração dos carregamentos e pop-ups de personagens (exemplo: florista), além de limpeza e preparação pública.
- Revisadas 138 telas de narração em português (409 entradas, 401 alterações); inglês também revisado e rediagramado (385 entradas alteradas em oito arquivos). Quebras dos carregamentos podem mudar sem alterar comandos de nome, botão, estilo ou formatação; regressão protege a exceção limitada a esses recursos.
- Ajuste de nomes: consumidor NPC 0xaea710 tem corte próprio em >=9 caracteres, diferente do marcador de comandos. Adaptação por assinatura FNV 0x428352b501c61c8a conserva a placa de oito células, restaura nome inteiro e ajusta só a escala horizontal do texto. Medição dos comandos/minimapa aceita letras Latin-1 com acentos. Testes sintéticos e instalação nos bytes do eboot próprio passam; jogo não aberto, visual pendente.
- Fonte IM: 26 glifos do próprio dump transplantados para espaço vazio da terceira página R8, preservando todas as 6.976 entradas anteriores e seus pixels. Tabela e atlas conferidos; folha de glifos vista offline. Dois testes sintéticos protegem preservação, idempotência e recusa de sobrescrever pixels.
- Criadas traduções dos 397 textos fixos e rótulos das 168 imagens da interface. Imagens pt-BR geradas; três diagramas receberam rótulos menores para evitar letra excessivamente reduzida. Prévia de controles vista sobre fundo escuro. Pacote por dispositivo pt-BR ainda em construção; não anunciar conclusão visual nem publicação.
- Novo gerador tools/build_localized_delta.py valida textos/controles/nomes/hashes e gera patch diretamente, sem outra cópia temporária de 6 GB. Inglês: 13.269 entradas, 8.323.217 bytes, SHA256 9514c281385def605f54f4acb2d51cd149e5fdd6a7bea89ca992e946f421fbf8. Português: 15.578 entradas, 8.390.838 bytes, SHA256 acd9357b7b8591a8b700b06a22aed90995ef760a4de3b07e743a5f6053017885. Deltas derivados são privados; não são o pacote público do GitHub.
- Removidos 21 protótipos inativos de src/ e gpu-gow3/ somente após conferir ausência no CMake e cópias byte a byte no checkpoint-20261027-143553.zip. Prova em build/retired-prototype-audit.json. Saves locais user/ preservados e adicionados ao .gitignore.
- Nova instrução explícita: salvar e desligar quando a cota real de 5 horas OU semanal ficar ABAIXO DE 7%, substituindo 10%. tools/watch_quota.py ativo em segundo plano sem janela; conversa principal e cwd conferidos, telemetria fresca e reset futuro obrigatórios. Três testes passam; 7% exatos não disparam. Leitura inicial: 62%/18% restantes. Processo registrado em build/quota-watch-20261102/watch.pid. Não usa tempo decorrido como cota e não força outros aplicativos.
- Trabalho de tradução/empacotamento continua. Portátil pessoal ainda não atualizada nesta etapa; não abrir jogo automaticamente nem fechar Godzilla.

### 02/11/2026 — candidata privada instalada para conferência curta
- Incluídas as 29 dicas de carregamento na revisão pt-BR; inglês também rediagramado. Fonte IM ampliada para 31 glifos, incluindo maiúsculas e ü, preservando as 6.976 entradas anteriores. HUD aceita também os ordinais º/ª.
- Diagramas: corrigida transferência de rótulo entre colunas (dois testes de regressão), reduzido cruzamento de linhas no teclado e permitido deslocar colunas inferiores. Se as linhas não podem ficar limpas, usa legenda com nomes das teclas/botões. Seis variantes Xbox/Nintendo de moto/carro/cavalo reconstruídas e auditadas sem achados; demais variantes preservadas. Pacote pt-BR: 67 texturas, 9.630.247 bytes, SHA256 8cfd37f8dbdcbc77fe4242817438dc6b2367a9b7b3a44760bf8413f387a6631b.
- Patches finais: inglês 8.323.260 bytes, SHA256 02445892fd160f587c54184ceef2071d4d4c09f4bd8ac797d162e708c86faf64; pt-BR 8.391.330 bytes, SHA256 ea0e874597f93f40c115dd9a077f3a419da5a2ec0c5217906503ca2f3d8d6e41. docs/m4/runtime-preview.json registra os deltas novos separadamente dos previews completos antigos. O empacotador prioriza os deltas verificados.
- Runtime e launcher compilados. Testes de HUD/layout/instalação no eboot, nomes, word wrap, IME, fontes, rótulos, PSARC/leitura virtual e imagens passam. Aplicador testado em instalações sintéticas Windows/Linux, incluindo rollback e preservação dos dados. Não contar a chamada inexistente test_psarc_delta.py (zero testes); cobertura PSARC veio dos nove testes reais e do novo test_delta_source.py.
- Gerado build/Kyoei-1.0-RC1-Atualizacao-20261102.zip, 43.680.611 bytes, SHA256 e0a132a9f70e85f6581636b594004a45851aa4c2e8026e547259d403ee9037f3. É atualização PRIVADA para instalações preparadas, não o ZIP público do GitHub; contém derivados locais do dump, sem PSARC, executável PS4 ou saves.
- Aplicado à portátil pessoal fechada: sete arquivos alterados, 2.082 arquivos protegidos de saves/configurações com hashes iguais; anteriores guardados em data/update-backups/localization-reviewed-20261102-rc1-20261102-023802. Runtime portátil SHA256 62464787af9764bac1c1dbeeda72afabe40c5e8587203f72bd7d1e3707ea201c. Launcher --check 0. Nenhuma partida aberta. Davy informado e convidado a conferir florista, carregamentos, acentos e controles, sem campanha completa.
- Publicação 1.0 ainda pendente: conferência visual da candidata, revisão restante e preparação reproduzível/instalação pública sem dependências dos caminhos locais. Conexão GitHub confirmou a conta DavyBirth; pesquisa não encontrou repositório acessível com kyoei. Nenhum push/publicação feito.
- Monitor de cota segue ativo: última leitura registrada nesta etapa 40% na janela de 5 horas e 15% semanal, limiar estrito abaixo de 7% em qualquer uma. Não fechar Godzilla nem abrir Kyoei automaticamente.


### 02/11/2026 ? balan?o e limpeza do disco solicitados por Davy
- Balan?o da ?ltima sess?o: candidata RC1 instalada, revis?o dos carregamentos en/pt-BR, fonte IM com acentos, nomes longos e interface/controles pt-BR. Confer?ncia visual por Davy e publica??o continuam pendentes. Os ?ltimos registros est?o datados de 02/11, embora Davy tenha se referido a ontem.
- Espa?o livre no G: passou de aproximadamente 5,64 GiB para 26,39 GiB (cerca de 20,75 GiB recuperados). Removidas as instala??es derivadas build/public-application-check, public-setup-check, public-setup-check-v2 e localization-rc1-payload; manifestos preservados em build/cleanup-20261102. Pastas continham hardlinks, portanto a soma dos tamanhos aparentes n?o representa a recupera??o f?sica.
- Removido somente o common.psarc da pr?via obsoleta runtime-preview-pt-03 no workspace privado; manifesto preservado, substituto runtime-preview-pt-04 e refer?ncia inglesa en-18 mantidos. Tradu??es, ferramentas e fontes de reconstru??o preservadas. Prot?tipos/checkpoints e trabalho ?nico n?o foram removidos.
- Confirmada aus?ncia de processos de jogo/prepara??o usando os candidatos; caminhos absolutos e aus?ncia de reparse points conferidos antes das remo??es recursivas. Dump original e port?til mant?m os 87 arquivos esperados presentes com tamanho correto. Runtime e deltas en/pt-BR continuam com os hashes RC1 registrados acima; launcher --check retornou 0. Saves e configura??es da port?til n?o foram alvo da limpeza. Prova privada em build/cleanup-20261102/verification.json.
- Nenhum jogo aberto, nenhum processo do Godzilla encerrado. A telemetria encontrada em build/quota-watch-20261102/current.json era antiga e marcada fresh=false; n?o foi usada para inferir cota atual nem disparar desligamento.

### 02/11/2026 — revisão independente encaminhada ao Claude
- A pedido explícito de Davy, iniciado Claude Code local para revisar a candidata 1.0, incluindo florista/nomes longos, carregamentos, acentos, imagens e controles; tradução, preparo público, privacidade e instalação também no escopo.
- Roteiro em docs/REVISAO-CLAUDE-1.0.md. Processo oculto com somente Read/Glob/Grep, sem shell nem edição; sem abrir jogo, alterar portátil ou fechar Godzilla. Imagens existentes podem ser inspecionadas offline; comprovação ao vivo deve ser marcada pendente com roteiro para abertura manual por Davy.
- Processo e saída em build/claude-review-20261102/process.json, relatorio.md e stderr.log. Revisão encaminhada; não tratar o início do processo como revisão concluída. Relatório poderá ser parcial se houver falha de autenticação/cota ou limitação das ferramentas.
- Davy esclareceu que deseja iniciar o Claude manualmente. Encerrada somente a execução de revisão iniciada pelo agente (PID registrado em build/claude-review-20261102/process.json); roteiro preservado para abrir na sessão manual. Não há relatório concluído dessa execução.

### 02/11/2026 — revisão independente do Claude concluída (sessão manual)
- Relatório em build/claude-review-20261102/relatorio.md. Nada de código, tradução ou portátil alterado; jogo não aberto.
- Bloqueios apontados: preparador público depende de third_party/godzilla-core não versionado; falha no bootstrap deixa ~6 GB em build/setup-* e trava nova tentativa; languages/ e setup-manifest.json não ignorados no Git; diagrama PS pt-BR com textos sobrepostos (L2/L1, R2) na prévia; placa de NPC sem escala mínima (nomes de 27–35 letras a ~25%); stage_205_news_data sem pt-BR.
- Pendente de Davy: roteiro visual de 6 passos no relatório.

### 02/11/2026 — correções da revisão (RC2, Claude)
- Tradução pt-BR: notícia do estágio 205 traduzida; fala da florista neutra ("Tome cuidado, viu?"). Varredura de gênero do protagonista (pode ser homem ou mulher): 40 falas/escolhas compartilhadas neutralizadas (ex.: "Bateu uma irritação", "Não, valeu", "Olá! Fique à vontade!", "A curiosidade fala mais alto"), mais 6 trechos de narração/missões ("Em choque", "Lutando contra o vento"...). Falas que já têm versão masculina/feminina no jogo foram mantidas.
- Carregamentos: 52 quebras pt e 41 en reequilibradas (sem palavra sozinha na última linha), dentro da largura já usada; pausas "Ah..." preservadas. Nenhuma quebra com hífen nos dois idiomas.
- Placa de NPC: escala mínima 0,6 (src/kyoei_hud_trace.c); nomes longos podem passar um pouco da placa em vez de ficarem ilegíveis. Teste 29 novo; visual pendente.
- Diagramas: rótulos de câmera/Sombra Gigante encurtados (fim das sobreposições L2/L1/R2 no PS); "Virar: esquerda / direita"; "or"→"ou" nas 7 listas pequenas (região só pt-BR, filtro por idioma em texture_language/make_pad_diagrams); "Botão −/+" e "Botão Exibir" (Nintendo/Xbox); margem maior entre legendas e desenho do teclado. Tela de ajuda do título: "Anal. esq. +", "Aperte ○" no lugar de "Press ○ Button". Todos os 18 diagramas PS e as folhas Xbox/Nintendo/teclado vistos offline.
- Botão trocado (○→✕) no cursor de próxima fala: o detector pegava o brilho verde e pintava um disco duro; agora só remove o símbolo e preenche suavemente (refill). test_american_icons PASS.
- Preparo público: third_party/godzilla-core/scripts + LICENSE versionados (hashes iguais ao snapshot); bootstrap limpa build/setup-* em qualquer falha e desfaz a instalação parcial (teste novo); mensagens pt/en; .gitignore cobre languages/, setup-manifest.json, support/. Launcher: textos em pt, sem "prévia", erro do preparo visível, confere scripts do core.
- X/O: hoje o X confirma em todos os idiomas, inclusive japonês (rt_services param 1000 fixo = cross; arte americana sempre). Não alterado: decisão de Davy e teste no jogo necessários.
- Deltas RC2: en 8.323.252 bytes (13.269), pt-BR 8.355.696 bytes (15.578), roundtrip verificado. Jogo não aberto.

### 02/11/2026 — releitura completa pt-BR e en (Claude)
- Lidos todos os textos únicos dos dois idiomas, na ordem do jogo (falas, notícias, itens, missões, carregamentos, romances, créditos).
- pt-BR: 198 correções (208 trechos no arquivo). Toro volta a ser macho, como no original e no inglês. Mais falas compartilhadas sem gênero ("Por vocês", "com as mãos amarradas", "O elevador parou comigo dentro", "achou que você era o pai ou a mãe dela"...). Nomes padronizados: "Jardim do Céu da Meia-Noite", "O Céu de Nós Dois", "Edifício Sukima Nº 2", "HP".
- en: 48 correções. "For you ladies" → "For you two" (Otsuka), "Attaboy" neutro, "your girlfriend" → "Yuki" na fala da Risa, "young man" → "young person" na notícia do anel. Títulos "Street Style Snapshot" uniformes, maiúsculas em carregamentos, "Granzellla" → "Granzella", nomes de itens iguais aos da lista (Captain's Spare Cap, Red Pencil Tucked Stylishly Behind the Ear).
- Defeito achado nos dois idiomas: uma notícia (Toro e Kuro, estágio 212) mostrava o código japonês cru "＜主人公＞"; trocado por "A certain young person" / "Uma certa pessoa jovem".
- Conferências: marcadores do jogo iguais aos de antes (os avisos restantes já existiam: nomes do protagonista); larguras dos carregamentos iguais (pt 2 títulos de 61 já existentes, en 0); testes de tradução, diagramas e preparo OK.
- Deltas refeitos em build/rc2-work: en 13.269 e pt-BR 15.578 entradas, roundtrip verificado. Jogo não aberto.
- Pedido de Davy: X/O muda só no japonês. Parâmetro 1000 do sistema agora devolve círculo (0) quando GZ_LANGUAGE=0 (japonês) e X (1) nos outros idiomas; adaptação no CMakeLists (cópia fixa do Godzilla intacta). Runtime recompilado e instalado; jogo não aberto.
- Prévia de diagramação fora do jogo: tools/preview_text_layout.py (largura das letras da fonte do jogo + quebra entre palavras do port; caixa estimada = maior linha japonesa do mesmo tipo). Saída privada build/text-preview/preview.html. Marcados: en 122, pt-BR 160 (estimativa; muitos podem ser falsos alarmes). Candidatos reais a conferir: dicas de carregamento (LoadingTips) com 4 linhas onde o japonês usa 3, epílogo e algumas falas longas dos estágios 213/215. Pendente: abrir o jogo (Davy liberou, mudo) para calibrar o tamanho real das caixas.
- Teste no jogo (Davy liberou; mudo, janela 1280×720, build/kyoei.exe na portátil): aviso inicial, abertura, título e duas telas de carregamento do novo jogo em pt-BR corretos, sem cortes. Título "New Game/Load Game/Option" é inglês no próprio jogo (sem entrada de tradução). Calibração: a caixa do texto de carregamento quebra em ~1.000 de avanço (a frase "...época da / primavera."), enquanto a prévia estimou 2.660 pela maior linha japonesa: o japonês também depende da quebra automática, então a regra da prévia não vale para todos os tipos. A largura-limite real fica no estado interno do medidor (0x2e2b10), ainda não mapeado; mapear isso é o próximo passo para uma prévia confiável.
- Larguras reais das caixas: o medidor do jogo (0x2e2b10) compara a posição da letra com params+0x10 (largura em pixels) após escalar por params+8. Registro opcional KYOEI_WRAP_LIMITS / KYOEI_WRAP_LINES em src/kyoei_text_wrap.c (desligado por padrão). Medidas na tela, em unidades da fonte (largura/escala): legendas 1.667 (até 3 linhas no japonês), escolhas 1.484, carregamento 1.147, painel de local 463, local de missão 441 (uma linha, corta sem "…").
- Defeitos achados no jogo e corrigidos: painel do menu cortava "Bairro Sakur/a" (essa caixa redesenha quebrando por letra; agora 45 nomes pt e 46 en de place_text têm quebra entre palavras e alguns foram encurtados); coluna de local das missões cortava "Calçada Chuvosa ao An" (10 locais pt encurtados); 2 fichas do estágio 214 em 4 linhas reescritas para 3 nos dois idiomas. Falas, escolhas e carregamentos vistos estão certos.
- A pasta data/user da portátil não tem saves do jogo, então só o começo foi testado.
- Modo de teste em segundo plano: KYOEI_TEST_INPUT=<arquivo> (src/kyoei_test_input.h, ligado no laço de eventos via CMakeLists). O teste acrescenta linhas "tap <tecla> [ms]" e o port injeta eventos SDL na fila; não precisa de foco. Fotos com tools/capture_native.ps1 sem -Focus. Desligado em todo uso normal. Testado: título, menus, carregar save e jogo com a janela atrás de outras.
- Saves de Davy (G:\kyoei-toshi-work\native-05, fase 1) copiados para G:\kyoei-toshi-work\claude-test3 para testes; originais intocados. Dinheiro fica em SAVEDATA.DAT+0x1088 (64 bits). SYSTEM.DAT: opções em 64..69, volumes em 72/76/80.
- Dicas de carregamento: caixa real 700/0,6 (~1.167), corpo com no máximo 2 linhas no japonês; 9 dicas pt e 5 en rebalanceadas para 2 linhas com quebra entre palavras ("Luzes nas portas" encurtada).
- Começar em fase específica: o jogo tem itens de Coleção "Começar da fase N" (tabela em 0x13006b0 da imagem, com "Bônus de conclusão"), liberados após zerar ou por moedas; o "????" do título ficou bloqueado neste save. Próximo passo: achar a marca de conclusão/moedas ou a função de início de fase.
- Coleção liberada na cópia de teste: SYSTEM.DAT bytes 2090–3092 = 1 (marca dentro desse bloco; bytes 84–2089 mexem em opções e estragam as cores). Na Coleção, o nome da fase 9 passava por cima do contador nos dois idiomas: binary-en agora "Clashing Convictions", rótulo pt "Convicções em choque"; os outros 16 nomes cabem (~300 px). eboot-text en/pt refeito e incluído na instalação (build/install_rc2.py).
- Começar em fase específica ainda não resolvido: SAVEDATA byte 0 não é a fase; moedas não estão em 0x1094–0x10a8. Próximo: achar a função de "Começar da fase N" (tabela 0x13006b0) ou o campo de fase do save.
- Pontos de partida do jogo: tabela de 27 pontos em 0x126c738 da imagem (entrada de 0x30 bytes: número, nome "ID_00nn", fase 201–210, ponto), mais rótulos "Stage N" e "Começar da fase N" em 0x12ffe00 (parece menu de depuração/Coleção). Trocar no save os campos 0x0, 0x76, 0x82, 0x84, 0x1090, 0x10ac/b4/b8 não mudou a fase carregada. Próximo passo: registrar em tempo de execução quem lê essa tabela ao carregar (ou ao começar novo jogo) e forçar o ponto escolhido só em modo de teste.

## 03/11/2026 — Pular para qualquer fase (teste) e módulos de todas as fases
- Atalho só de teste: `KYOEI_TEST_STAGE=<arquivo>`; gravar "209 30" (fase, ponto da tabela 0x126c720) no arquivo. O gancho em Scene.isReady (0x6068a0, chamado enquanto a fase carrega) chama a função de desenvolvedor Scene.setNextStage (0x607d00) depois de 4 s de carregamento (antes disso o carregamento do save sobrescreve). getTime, Pad.IsTrigger e isDbgPadOn não são chamados durante o jogo. Conferido: save da fase 201 → fase 209 (em cima do trem, com Yuki), mudo e em segundo plano.
- **Defeito sério achado**: o port só preparava o StageModule.prx da fase 201; qualquer outra fase parava com "native module not prepared" (inclusive avançando a história normalmente). `tools/prepare_portable_image.py` agora liga os 19 StageModule (201–218); as exportações repetidas entre fases ficam fora da tabela global (cada fase as acha pelos registros E do deferred.tsv), via adaptador conferido do linker fixo. Runtime: até 32 módulos adiados; carregador (adaptador no CMake) aceita 48 módulos e 256 segmentos. Imagem nova: 0x2cd4058 bytes; endereços do eboot e da fase 201 inalterados.
- Instalações já existentes precisam preparar a imagem de novo (runtime/out) para receber as outras fases; incluir isso no pacote 1.0.

### 03/11/2026 — retomada Codex da validação RC2
- Davy pediu concluir a etapa deixada pelo Claude com a partida já aberta. Sessão de teste PID 31684, build/kyoei.exe, perfil separado claude-test3; saves originais preservados. Capturas sem foco em build/review-20261103. Não iniciar Claude automaticamente.
- Nova autorização: reserva de cerca de 5% em qualquer cota, substituindo 7%. tools/watch_quota.py agora inclui 5% exatos; testes de validade/conversa/limiar passam. Monitor oculto desta conversa em build/quota-watch-20261103, sem estimar uso pelo relógio. Primeira leitura válida: 99%/10% restantes (5 horas/semanal). Salvar checkpoint verificado antes de solicitar desligamento normal imediato do Windows.
- Atenção à prova de salto: o log atual registra setNextStage(214, 10), mas o inicializador executado em seguida é stage202, e a captura mostra a conversa no trem. A afirmação anterior de validação da fase 209 não deve ser contada como comprovada sem confirmar o módulo e o cenário reais. Investigação dos argumentos/ordem de carregamento em andamento.
