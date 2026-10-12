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
