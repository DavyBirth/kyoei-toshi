# Plano de trabalho offline — próxima manhã

Preparado em 13/10/2026 para executar quando Davy retomar antes da escola.
Este arquivo não agenda execução nem mantém o agente ativo após a sessão.

## Regras da sessão

- Não abrir o jogo, shadPS4 ou qualquer teste que os execute. Não fechar nem
  reiniciar uma partida existente. Não depender de testes de Davy na escola.
- Preservar dump, saves, core fixo e projeto Godzilla. Adaptações em src/ e
  integração de build; dados derivados do jogo permanecem privados fora do Git.
- Ler AGENTS.md, DIARIO.md e RETOMADA.md; conferir Git, processos e espaço.
- A informação antiga de 4% pertence à sessão anterior. Não presumir quota
  renovada sem informação atual. O agente não consegue consultar o percentual.
- Se houver contador confiável ou aviso de Davy, iniciar fechamento em 5% e
  parar perto de 3%. Sem contador, salvar etapas e encerrar com checkpoint;
  não prometer controle exato nem gastar uso apenas para chegar ao limite.
- A janela de cinco horas não garante cinco horas de execução contínua.
  Se o trabalho durar esse período, reservar 20–30 minutos para encerramento.

## Ordem das traduções — orientação explícita de Davy

Fazer uma língua por vez: primeiro inglês, depois português do Brasil.
Revisar e ampliar os rascunhos EN antes de iniciar nova rodada PT-BR. Validar
variáveis, ícones, cores, quebras, nomes personalizados e reinserção do inglês
antes de publicar sua prévia privada. Não gerar os dois pacotes em paralelo.
Registrar o checkpoint EN e suas dúvidas antes de passar ao português.
Revisão humana e confirmação visual continuam pendentes; não exigir que Davy
esteja em casa para concluir o trabalho verificável offline.
Correções compartilhadas de fonte/runtime podem contemplar ambos os idiomas
sem iniciar uma segunda rodada de tradução. Não apagar os rascunhos PT atuais.

## 1. Fontes e minimapa

- Rastrear a fonte do objetivo p0/k36 em common/lua/map_goal_text.luc.
  O texto preserva o à; o atlas principal tem o glifo, variantes têm lacunas.
  Demonstrar qual fonte é usada antes de escolher a correção.
- Documentar tabela, atlas, métricas e páginas. Adicionar glifos somente com
  formato demonstrado; preservar desenhos existentes, ícones e transparência.
  Verificar ida e volta, limites e gerar folha privada para inspeção.
- Investigar âncora, pivô, largura e transformação do marcador p0/k17,
  incluindo bytes versus caracteres. Não encurtar a frase como paliativo.
- Corrigir causas reproduzíveis offline e testar com dados sintéticos.
  Se depender do executável ou da tela, registrar caminho exato e preparar
  diagnóstico opt-in para depois, sem declarar o defeito visual resolvido.

## 2. Entrada de nome e save/load na imagem do jogo

- Separar lógica de IME/save/load da apresentação Win32, preservando ABI,
  resultados, armazenamento, slots e erros. Usar saves sintéticos nos testes.
- Detectar modalidade ativa por eventos reais de teclado/controle, filtrando
  drift. Permitir alternância durante a edição; conexão não prova uso ativo.
- Implementar modelo do teclado virtual: navegação, caixa, espaço, apagar,
  cursor, confirmar/cancelar, limites e Unicode. Círculo confirma, X volta.
- Integrar apresentação ao runtime e renderer existentes. Preservar fallback
  funcional até confirmação visual; não substituir por simulação desconectada.
- Integrar save/load à apresentação mantendo seleção, cancelamento e aviso de
  sobrescrita. Testar estados, confirmação única e ciclo de vida sem jogo.

## 3. Inglês, nomes e depois português

- Localizar nomes padrão e confirmar leituras antes de romanizar. Não traduzir
  significados nem alterar nomes personalizados ou saves existentes.
- Revisar primeiro as 94 entradas EN e ampliar abertura, tutoriais e menus
  com contexto suficiente. Preservar tokens e registrar dúvidas por ID/hash.
- Não tratar constantes internas como falas nem traduzir cenário indiscriminadamente.
- Validar um conjunto coerente e preparar apenas a prévia inglesa, quando
  viável. Verificar hashes e reinserção antes de atualizar o manifesto.
  Não substituir arquivos utilizados por uma partida aberta.
- Após checkpoint inglês, repetir o processo para PT-BR. Revisão humana
  permanece necessária; contagem de entradas não certifica cobertura completa.

## 4. Gráficos e desempenho offline

- Usar logs/capturas existentes para piscada preta e textura do tutorial do orbe.
- Revisar readback, invalidação, buffers e fences com testes dos métodos reais.
  Manter KYOEI_BATCH_READBACK opt-in até comparação em gameplay.
- Corrigir apenas causas demonstráveis. Microbenchmark não comprova FPS.
  Instrumentação deve ser opt-in e limitada; registrar próxima coleta necessária.

## Checkpoints e encerramento

- Cada avanço funcional: verificações pertinentes, diário e um commit.
  Investigações longas: notas de retomada a cada 30–45 minutos.
- Conferir quais testes são headless antes de rodar. Não executar jogo nem
  repetir suítes sem motivo. Não versionar dados do dump ou texto original completo.
- Se faltar evidência visual, documentar e avançar para tarefa independente.
- Encerrar com build/testes afetados, Git e RETOMADA atualizados: commits,
  comandos, resultados, artefatos privados, limitações e próximo passo exato.
- Informar o que Davy deve ver depois, distinguindo provas offline de gameplay.

## Comando para retomar

> Continue seguindo docs/PLANO-MANHA.md. Estou indo para a escola; não abra
> o jogo nem dependa de testes meus. Comece pelo inglês e só depois faça o
> português. Salve checkpoints e reserve o final para documentar. A quota
> está renovada; não finja consultar o percentual se ele não estiver disponível.
