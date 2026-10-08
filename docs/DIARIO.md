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
