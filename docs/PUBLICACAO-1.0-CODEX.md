# Publicação da 1.0 no GitHub (para o Codex)

Pedido de Davy em 03/11/2026: publicar a 1.0 no GitHub, **repositório público direto**. O Codex faz a publicação.

## Pacotes prontos (gerados do commit atual da branch desempenho-codex)
- build/Kyoei-1.0-Windows.zip — SHA256 1272b160c30ebff26a468bcad9b4e9af86134491e69b5630a17e9ed3a67a6adb — 1.801 arquivos, sem dados do jogo nem caminhos pessoais (verificado).
- build/Kyoei-1.0-Source.zip — SHA256 bb41e1f0e164035cfdee46c1abdca4bafb78864408fb3153fbff1166cf81371b — 16.082 arquivos (fontes correspondentes; kit v3 já compilou Release fora do cache em 03/11).
- Arquivos .sha256 ao lado de cada ZIP.

## Passos
1. Conferir que nada do jogo está no histórico do Git (AGENTS.md: nada do jogo no repositório). Publicar a branch principal com o código atual (mesclar desempenho-codex em main).
2. Criar o repositório público, enviar main.
3. Criar a release v1.0 anexando os dois ZIPs e os .sha256. Texto: docs/release/INSTALL.txt (pt/en) + problemas conhecidos.
4. build_public_candidate ainda grava publication_ready=false e build_public_source clean_build_verified=false: atualizar/registrar a prova (kit v3 compilou; este v1.0 difere só no código de hoje) antes de marcar como pronto.

## Pendências conhecidas (não bloqueiam a 1.0, ver INSTALL)
- Nomes de lugar do minimapa cortados na borda (marcador nativo).
- Rodapé de resposta de e-mail (Escolha/Enviar) não conferido contra o japonês.
- Leitor de livros e tela de fim de jogo não alcançados em teste.
- Balões de ação de 12 caracteres: runtime registra "Kyoei label cut (12 chars)" no log se algum corte acontecer.
