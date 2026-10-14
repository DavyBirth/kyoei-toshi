# Inglês: retomada e preparação da prévia, 14/10/2026

A versão privada atual tem **8.259 de 16.100 entradas preenchidas (51,3%)**,
com **1.337 novas entradas** em nove lotes nesta sessão. Restam 7.841 vazias.
As contagens incluem registros internos e testes: não comprovam a porcentagem
de cenas ou de texto visível do jogo traduzida. Revisão humana permanece pendente.

Os lotes ampliaram a estação, a torre, o navio, as conversas opcionais de Yuki,
o correio e itens. Foram preservados IDs, hashes, japonês, controles, variantes
e inglês recebido. As 38 reparações anteriores de delimitadores continuam sendo
as únicas modificações no inglês do pacote recebido; nenhuma entrada foi apagada.

Os 173 documentos passaram na auditoria. As 8.259 traduções foram reinseridas
e relidas em memória em 117 recursos reais do dump próprio, com preservação dos
textos restantes. Isso não equivale a aprovação editorial ou teste de gameplay.

## Contexto audiovisual e nomes

O japonês do JSON é a fonte textual. SRT/TXT ajuda a localizar candidatos, e
quadros nativos ajudam a confirmar interlocutores, tom e situação. O SRT atual
do Stage 4 degrada após aproximadamente 90 segundos, com repetições e conteúdo
incoerente; não usar esse trecho como evidência de diálogos reais.

Quadros 1280×720 foram vistos aos 1200, 1800 e 2200 segundos. Aos 1800 segundos,
a fala visível corresponde a 0146:p0/k1790: a menina dirige-se a Yuki como irmã.
Os outros quadros mostram o catwalk e a busca pelo extintor, sem comprovar todas
as falas dessas sequências. Registro e hashes ficam na pasta privada, em
reports/day-20261014-visual-evidence.json. Nenhum vídeo novo foi iniciado no jogo.

O próprio texto 0018:p0/k290 fornece a leitura de Kashiwagi em kana. Risa e
Hiroko são transliterações diretas dos nomes em katakana. Essas são leituras
fundamentadas para o rascunho, não uma alegação de localização inglesa oficial.
Outros nomes incertos permanecem pendentes; não substituí-los por pronomes.

A apresentação dos personagens no [PlayStation Blog](https://blog.ja.playstation.com/2017/10/16/20171016-kyoeitoshi/)
também confirma o contexto da menina e de Yuki; não fornece aprovação dos
nomes em caracteres latinos.

## Prévia para o port

Davy pediu avaliar aplicação incremental e autorizou preparar uma prévia se
não exigisse trabalho grande. Não é necessário recompilar o port: importar
o rascunho e reconstruir o common.psarc privado usa as ferramentas existentes.

tools/import_localization_workspace.py valida os documentos privados, confere
fontes, nomes e controles e grava somente uma lista explícita de campos:
id, source_text_sha256, en, notes e editorial_status. O japonês original serve
para validação em memória e não é incluído como campo no patch. O novo arquivo
translations/localization-en-20261014.json contém 8.259 traduções em 117 recursos.

A união com as 130 entradas anteriores passou na validação: **8.389 entradas**,
sem IDs duplicados, com reconstrução/releitura dos recursos. A prévia inglesa
en-04 foi gravada e publicada em pasta nova: 6.348.273.788 bytes, SHA-256
026a094e7ed2fa8f84e19ee9c7d9e3bf4eab5daa11d13f4016c8b022fceba792.
Prova do PSARC: 117 recursos fornecidos/alterados, verified=true; os recursos
foram relidos e o hash da gravação conferido. CheckOnly EN e PT-BR PASS.
Português permanece em 94 entradas. Nenhuma partida foi aberta pelo agente;
gameplay/layout da en-04 ainda dependem de teste visual.

O pacote aumentou 1.748.729 bytes sobre o original de 6.346.525.059 bytes.
O inventário M0 soma 17.798.062.971 bytes em 87 arquivos. Não confundir o
tamanho de uma cópia de teste com o aumento causado pelos textos.

Após validar o substituto, ausência de partida ativa e manifesto en-04,
removido apenas common.psarc inglês en-03, liberando 6.346.561.190 bytes.
Traduções e provas en-03 preservadas; reconstrução permanece possível.
Dump, saves, EN en-04 e PT-BR 02 preservados, conforme regra de limpeza.

## Retomada

working/ e o consolidado cumulativo foram atualizados na pasta privada.
CONTINUE-HERE.md, CHECKPOINT.json e o índice pendente apontam os próximos lotes.
Priorizar 0061 (navio), 0111 (roupas) e lacunas narrativas; confirmar nomes e
ampliar a conferência audiovisual. Inglês incompleto; PT-BR não ampliado.

A passagem posterior para PT-BR deve usar japonês como fonte e inglês revisado
como apoio. Contexto e ferramentas serão reaproveitados; fontes, acentos e
layout precisam de conferência própria.
