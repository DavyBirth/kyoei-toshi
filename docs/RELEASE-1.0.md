# Versão 1.0 portátil

Escopo autorizado por Davy em 02/11/2026. Desempenho e conclusão de uma campanha inteira não bloqueiam o lançamento. A versão deve informar suas limitações reais.

- [ ] Revisar a tradução pt-BR e inglesa, preservando sentido, nomes e controles.
- [ ] Corrigir narrações e diagramação dos carregamentos, inclusive títulos longos.
- [ ] Corrigir pop-ups de personagens truncados, incluindo nomes com acentos.
- [ ] Completar os caracteres das fontes usadas e revisar as imagens traduzidas.
- [ ] Atualizar a portátil pessoal para teste manual de Davy, preservando saves.
- [ ] Conferir dependências e compilação a partir de uma pasta limpa.
- [x] Retirar experimentos obsoletos após conferir arquivos de referência e dependências (21 protótipos, cópias idênticas verificadas nos checkpoints).
- [ ] Gerar pacote público sem dump, saves, caches, logs, imagens originais ou caminhos pessoais; a pessoa fornece seu próprio dump.
- [ ] Conferir licenças, créditos e disponibilidade do código das dependências.
- [ ] Escrever instruções, requisitos e problemas conhecidos, verificar hashes e publicar no GitHub.

A portátil pessoal e os pacotes privados de amigos contêm dados do disco. Não são os arquivos para anexar à release pública. Não abrir o jogo automaticamente; Davy faz a conferência visual manual.

Preparação da candidata em 02/11: 138 narrações e 29 dicas de carregamento revisadas; 397 textos fixos e 168 imagens pt-BR traduzidos; 31 glifos adicionados à fonte IM; código do nome completo nas placas NPC conferido contra o eboot próprio. Reconstrução dos recursos e testes offline passam. Os itens visuais acima só serão fechados após conferir no jogo a candidata atualizada.

Conferência curta de Davy: carregar o save perto da florista e verificar o nome inteiro; observar uma tela de carregamento e uma tela de intervalo com acentos; abrir a ajuda de controles no dispositivo usado. Isso não exige concluir uma campanha.

O pacote público também precisa resolver as dependências locais do preparador (dump/firmware do próprio usuário) e da compilação, que ainda apontam para caches e pastas desta máquina. A candidata privada não comprova uma instalação pública a partir de uma pasta limpa.


Atualizacao 03/11: gerador de candidata publica em tools/build_public_candidate.py.
Inclui Python isolado, NumPy/Pillow, fontes OFL, tabelas de traducao e preparadores;
recusa formatos derivados do disco e perfis. ZIP v6 gerado, com avisos AAC/FSR/ImGui
complementados. Instrucao de instalacao em docs/release/INSTALL.txt; auditoria das
fontes e bloqueios de compilacao em docs/release/DEPENDENCIES.md.
O teste completo de preparo a partir do ZIP esta em andamento. Nao marcar pacote
publico/reproducibilidade/publicacao como concluidos antes de terminar a prova.
