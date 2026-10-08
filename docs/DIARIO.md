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
