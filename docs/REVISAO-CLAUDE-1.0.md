# Revisão independente da candidata 1.0

Pedido de Davy em 02/11/2026: conferir tradução, instalação, distribuição e a parte visual, incluindo florista, carregamentos e controles. Produzir primeiro um relatório; não alterar código, traduções ou a portátil nesta revisão.

## Contexto e regras

- Leia AGENTS.md e as últimas sessões de docs/DIARIO.md. Desempenho e campanha completa não bloqueiam a 1.0.
- Preserve dump, saves, configurações, trabalho não commitado e projetos do Godzilla. Não publique, envie arquivos do jogo nem faça limpeza adicional.
- Não abra o jogo automaticamente ou em segundo plano. Faça a inspeção visual offline com imagens/capturas existentes. Para comprovação ao vivo que dependa de abrir o jogo, deixe um roteiro curto para Davy abrir manualmente; não confunda inspeção de código ou imagem gerada com prova no jogo. Testes eventualmente autorizados devem ficar mudos sem afetar outros aplicativos.
- Não reinstale nem reconstrua cópias completas: o G: ficou com aproximadamente 26,4 GiB livres após a limpeza. Use evidências e derivados existentes; não recrie diretórios de teste removidos.
- Cota do Codex: somente telemetria real, recente e da conversa principal pode justificar a regra de desligamento abaixo de 7%. O current.json antigo não é telemetria atual. Não desligue com base em valores antigos ou estimativa de tempo.
- Não execute comandos de shell nesta primeira passagem: use Read, Glob e Grep para código, documentos e imagens. Se precisar de execução ou ferramenta visual indisponível, registre precisamente a limitação e o teste pendente. Não leia credenciais ou conversas particulares.

## Candidata e evidências

- Portátil: build/City-Shrouded-in-Shadow-Portable/City Shrouded in Shadow.exe.
- Runtime: runtime/kyoei.exe dentro da portátil, SHA256 62464787af9764bac1c1dbeeda72afabe40c5e8587203f72bd7d1e3707ea201c.
- Delta inglês: languages/en/translation.kydelta, SHA256 02445892fd160f587c54184ceef2071d4d4c09f4bd8ac797d162e708c86faf64.
- Delta português: languages/pt-BR/translation.kydelta, SHA256 ea0e874597f93f40c115dd9a077f3a419da5a2ec0c5217906503ca2f3d8d6e41.
- build/Kyoei-1.0-RC1-Atualizacao-20261102.zip é atualização PRIVADA, não distribuição pública. Contém derivados locais do dump.
- docs/m4/runtime-preview.json, docs/release/supported-game.json e build/cleanup-20261102/verification.json.
- A última limpeza removeu public-application-check, public-setup-check, public-setup-check-v2, localization-rc1-payload e o arquivo common.psarc da antiga prévia pt-03. Manifestos guardados em build/cleanup-20261102. A portátil atual, prévias en-18/pt-04 e fontes de trabalho continuam preservadas.
- Existem alterações não commitadas em ferramentas de bootstrap/arte e launcher. Revise o estado atual dos arquivos, incluindo tools/bootstrap_portable.py, tools/prepare_portable_image.py e seus testes; não suponha que HEAD contenha tudo.

## O que verificar

1. Tradução en/pt-BR: cobertura, redação natural, narração e dicas de carregamento, glossário, placeholders e comandos, quebras de linha, resíduos de japonês e inglês indevido no pt-BR. Diferencie créditos/nomes e entradas internas de texto realmente visível. Não afirme revisão integral sem inspecionar a totalidade.
2. Florista e outros nomes longos: examine o ajuste específico de NPC e as evidências visuais disponíveis. A placa deve exibir o nome inteiro sem reticências, corte, sobreposição ou redução ilegível. Identifique os arquivos/funções e o que ainda precisa ser observado em execução.
3. Carregamentos: veja as folhas/imagens disponíveis, legibilidade, margens, alinhamento, palavras cortadas, quebras naturais, contraste e acentos da fonte IM. Confira também ordinais º/ª e ü. Distingua atlas correto de renderização correta no jogo.
4. Controles: PS, Xbox, Nintendo e teclado/mouse; legendas, ícones dentro do texto, listas pequenas e diagramas grandes. Procure linhas cruzadas, rótulos trocados, sobreposição, textos minúsculos e incoerência com o mapeamento real, inclusive View/menos/M e arrasto do minimapa. Marque hardware não testado.
5. Instalação pública: caminhos locais, dependências, preparo a partir do próprio dump e módulos, integridade, erros compreensíveis, idioma, preservação de saves/configurações, rollback e testes relevantes. Revisão estática não equivale a instalação limpa executada.
6. Conteúdo público: ausência de executável PS4, PSARC, firmware, saves, configurações pessoais e texto japonês original completo. Separe pacote privado RC1 de futuro pacote público; a publicação ainda não ocorreu. Revise também documentação e exclusões do Git.

## Entrega

Responda em português simples. Para cada achado: gravidade, arquivo/linha ou tela, evidência concreta, reprodução e correção sugerida. Indique exatamente quais imagens abriu. Use estados: confirmado, suspeita ou não verificado. Ao final, liste bloqueios reais da 1.0, itens opcionais e um roteiro visual curto para Davy. Não declare florista/carregamentos/controles aprovados ao vivo sem ter visto o jogo. Seu texto será salvo em build/claude-review-20261102/relatorio.md pelo processo que iniciou esta revisão.
