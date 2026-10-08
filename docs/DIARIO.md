# Diário do projeto

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
