# Diário do projeto

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
