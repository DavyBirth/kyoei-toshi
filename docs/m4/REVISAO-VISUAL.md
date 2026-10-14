# Revisão visual da tradução

## Retorno de Davy em 14/10/2026: prévia inglesa en-04

Davy testou a prévia e relatou que está bem entendível, permitindo compreender
o humor e as possibilidades das escolhas. Como exemplo, citou uma situação
envolvendo roubar o uniforme de uma policial. É retorno do usuário, não
aprovação editorial de todo o inglês ou certificação de capítulos completos.

Problemas relatados, ainda sem causa demonstrada:

- Escolhas longas não preenchem/cabem adequadamente nos campos da tela.
- Algumas palavras são partidas entre linhas nas legendas.
- Às vezes um ponto fica isolado na linha seguinte.

A revisão precisa verificar largura útil, margens, escala/fonte, quebra em
espaços e tratamento da pontuação nas telas afetadas. Não afirmar que um único
ajuste de HUD resolve todas elas: escolhas e legendas podem usar consumidores
diferentes. Não cortar o conteúdo nem remover pontuação para ocultar o defeito.
Preservar tokens e a quantidade de quebras explícitas exigida pela reinserção;
não inserir novas quebras por tentativa sem verificar o comportamento na tela.

Resultado esperado: escolhas completas dentro dos campos, palavras inteiras
quando houver espaço possível e pontuação junto à frase. Frases muito longas
precisam de tratamento demonstrado pelo layout, sem perda de sentido.

Nenhuma nova captura, partida ou correção de renderer foi realizada a partir
deste relato. Coletar IDs/cenas e evidência visual quando disponíveis; usar o
relato para orientar a investigação, não como prova da causa.

## Textos fora dos recursos de texto

Davy confirmou o interesse em traduzir também:

- Logo/título do jogo.
- Aviso japonês exibido antes dos logos de abertura.
- Nome de episódio/capítulo ao iniciar uma fase.
- Outros textos gravados em imagens ou vídeos.

Esses exemplos fazem parte do escopo final; a cobertura atual não está provada.
Primeiro classificar cada tela: texto separado, imagem/textura ou vídeo. Não
presumir que o título de capítulo seja um vídeo ou que o aviso seja uma textura.

O M1 já identificou texturas .tex e movie.psarc com Title01.usm, Gzcm.usm,
201news.usm e test.mp4. Isso não identifica o conteúdo de cada um desses
exemplos nem comprova texto embutido. A referência é docs/m1/README.md.

Para texto separado, usar o extrator/reinseridor após identificar o recurso.
Para texto na arte, produzir uma versão localizada preservando estilo e
transparência, depois verificar formato, dimensões e uso no runtime.
Para texto embutido em vídeo, definir legenda/sobreposição ou substituição
localizada após investigar reprodução, áudio, duração e sincronização.

Usar somente o dump próprio e distribuir traduções/ferramentas/patch aplicável,
nunca vídeos, texturas ou arquivos completos do jogo. A versão gráfica do logo
precisa de definição visual e revisão antes de ser ativada. Não contar esses
textos como concluídos apenas porque existem 16.100 candidatos nos JSONs.
