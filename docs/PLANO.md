# Próximas etapas: estabilidade, desempenho e tradução

Atualizado em 12/10/2026, após os testes da primeira fase no port nativo.

## 1. Fechar um percurso reproduzível na primeira fase

Prioridade: câmera normal nítida, encontro com Ultraman e efeitos sem fechar,
conclusão de um objetivo, salvamento e recarregamento. Registrar a versão do
executável, percurso e resultado. A primeira fase aberta não comprova uma
missão completa nem compatibilidade com todas as fases.

## 2. Medir e melhorar desempenho junto com a estabilidade

Compilar em RelWithDebInfo, que mantém símbolos e otimiza o código. Comparar
o mesmo percurso e câmera após aquecimento. KYOEI_FRAME_STATS=1 registra a
cada 120 intervalos entre apresentações do jogo: FPS médio, média, mediana,
percentil 95 e maior intervalo em milissegundos. Redesenhos ociosos não contam.
Os números medem entrega de quadros, não isolam CPU e GPU; pausas de menus,
filmes e compilação devem ser anotadas e separadas do gameplay.

Investigar em seguida os maiores atrasos: compilação/cache de shaders,
sincronização CPU/GPU, leituras de imagens lineares necessárias à câmera,
acesso a arquivos e custo dos diagnósticos. Só chamar uma mudança de ganho
de desempenho após comparação controlada. Aparência simples do jogo não
determina o custo de executar seu código e gráficos de PS4 no PC.

## 3. Fazer um piloto pequeno de tradução para inglês agora

O M2 já tem extração e reinserção idêntica de 2.331 recursos CSH/LUC; isso
não prova que todos os textos visíveis foram encontrados. Começar por UMA
frase facilmente reconhecível, vista antes e depois no jogo. Depois escolher
20–30 entradas entre menu, tutorial, objetivo e diálogo curto. Verificar
caracteres, largura, quebras de linha, identificadores e escolha de idioma.
Não presumir que uma fonte com alguns textos latinos cobre todo o inglês.

Distribuir apenas traduções e ferramentas. Aplicar sobre o dump próprio em
saída de teste separada. Não versionar texto japonês completo ou arquivos
originais; documentar apenas o necessário para identificar recursos.

## 4. Completar cobertura de textos e fontes, depois traduzir por cena

Mapear o que falta no M2 e certificar fontes no M3, incluindo acentos do
português. Criar glossário fixo de heróis, monstros e lugares; revisar inglês
e português por cena com Davy. Conferir contexto, controles e limites de
layout dentro do jogo. Tradução em massa antes do piloto pode gerar trabalho
que precisará ser refeito.

## 5. Consolidar patch e expandir o port

Aplicador verificando código, versão e hashes do dump; prova de uso no
shadPS4 e no port. Avançar fase por fase, incluindo salvamentos. Reimplementar
sistemas aos poucos apenas quando seu comportamento estiver documentado.
O código fixo do Godzilla continua preservado; adaptações do Kyoei ficam aqui.

## Modo foto: depois da melhoria de desempenho

Pedido de Davy em 18/10/2026: incluir um modo foto, usando o port do P.T.
como refer?ncia a estudar. Implementar depois de resolver os problemas de
desempenho da vers?o atual, sem depender da reimplementa??o final do renderer.

- Primeiro: ocultar a interface e capturar imagens na resolu??o original.
- Depois: congelar a simula??o mantendo a renderiza??o ativa e liberar a
  c?mera, com posi??o, rota??o e campo de vis?o ajust?veis.
- Avaliar exposi??o e profundidade de campo como op??es posteriores.

Investigar como separar c?mera, simula??o e desenho antes de prometer c?mera
livre com pausa. Ao sair do modo foto, restaurar c?mera, interface, controles
e simula??o; preservar saves e o comportamento normal do jogo. Validar com
Davy a entrada, a captura e o retorno ? jogatina. Nenhuma implementa??o iniciada.

## 6. Etapa final opcional: renderizador próprio e reimplementação nativa

Direção esclarecida por Davy em 14/10/2026: manter e concluir o port híbrido,
depois avaliar substituir a tradução de comandos/shaders gráficos de PS4 por
renderização própria para PC. Não basta recompilar o carregador/runtime ou
trocar Vulkan por outra API para atingir esse objetivo. Vulkan pode continuar
como API do renderizador próprio; a mudança é consumir cenas, materiais e
recursos do jogo diretamente, em vez de interpretar comandos da GPU do PS4.

Investigar gradualmente a fronteira entre lógica e desenho: formatos de malhas,
materiais, texturas, animação, câmera, luzes, efeitos e submissão de cena.
Primeira prova: uma cena limitada renderizada pelo código próprio com câmera
e recursos identificados, comparada à referência. Depois ampliar cobertura
sem abandonar a versão híbrida funcional. Apenas interceptar alguns desenhos
ou reutilizar shaders convertidos não comprova independência gráfica completa.

Substituir só o renderer ainda deixa um port híbrido caso a lógica continue no
executável PS4. Reimplementação integral, conforme objetivo de preservação,
exige substituir também essa lógica aos poucos até depender só dos dados.
Descompilar ajuda a estudar comportamento; não recupera automaticamente o
projeto-fonte original nem produz um port nativo correto por recompilação.

Etapa opcional após estabilidade, tradução e desempenho da versão atual.
Viabilidade, custo e fidelidade dependem da investigação; não prometer prazo
ou 60 FPS pela troca de arquitetura. Distribuir código/ferramentas próprios
e exigir dump do usuário, preservando as regras de conteúdo do projeto.

Davy relata que o Godzilla está praticamente perfeito. Registrar como retorno
dele, sem presumir que Kyoei terá os mesmos gargalos ou desempenho. Continuam
valendo a preservação do core fixo e a proibição de modificar o projeto irmão.


## Desempenho por resolucao, opcoes graficas e outros PCs

Pedido de Davy em19/10/2026:continuar a correcao de desempenho e acrescentar
opcoes de resolucao/qualidade,como nos outros ports;priorizar testes em1920x1080,
resolucao do monitor dele. A referencia fiel ao PS4 permanece como padrao.
Opcoes posteriores de qualidade sao escolhas explicitas do jogador,sem alterar
velocidade da simulacao ou remover efeitos silenciosamente para esconder custo.

- Separar resolucao de saida/janela de resolucao interna/escala de renderizacao.
  Janela1080p sozinha nao comprova renderizacao interna1080p. Registrar ambas
  em cada captura/benchmark e avaliar menus,Hud,fontes e proporcao da imagem.
- Medir720p/1080p e,quando houver suporte validado,escala interna e opcoes
  individuais de efeitos/sombras/pos-processamento. Mostrar impacto real
  nas mesmas cenas,com FPS,tempos de quadro,p99,picos,VRAM e estabilidade.
  Comparar frio/quente e manter camera,rota,save e foco comparaveis.
- Oferecer configuracoes persistentes,com restauracao do padraoPS4. Confirmar
  primeiro quais opcoes o renderer realmente consegue alterar com seguranca;
  nao prometer que diminuir resolucao resolve gargalos de CPU/alocacao.
- Matriz inicial:PC de Davy e PC do amigo com GTX1660SUPER/32GBRAM. Processador
  do amigo informado como AMD1550G,modelo exato pendente de confirmacao.
  Coletar GPU,VRAM,CPU,driver,Vulkan e resultados reais de cada maquina;
  nao simular teste fisico da1660SUPER usando apenas o PC de Davy.
- Priorizar caminhos Vulkan por capacidades e limites suportados,com fallback
  e diagnostico util em placas distintas;evitar otimizar apenas para uma GPU.
  Amplificar a matriz para outras placas conforme testes reais disponiveis.

Este pedido amplia as opcoes futuras autorizadas,sem interromper a investigacao
atual de stutter. Testes de1080p passam a integrar a verificacao corrente.
