# Mapa de funcionamento — evidências e perguntas

Documento incremental, iniciado em 13/10/2026. Não constitui reimplementação
completa nem prova de comportamento em gameplay.

## Motor e fontes públicas

A Silicon Studio anunciou em 15/01/2016 que o projeto Kyoei Toshi adotou
OROCHI 3 e YEBIS 3. Essa fonte identifica o middleware anunciado; não fornece
o código do jogo nem demonstra que cada subsistema GxCore vem do middleware.
[Anúncio oficial](https://www.siliconstudio.co.jp/news/pressreleases/2016/1601orochi-yebis.html).

Na consulta de 13/10/2026, a antiga página OROCHI redireciona ao aviso de
encerramento de vendas em junho de 2026 e transferência de direitos a terceiro.
Não confundir encerramento comercial com liberação do código.
[Aviso oficial](https://www.siliconstudio.co.jp/news/info/2026/2606orochi/2606orochi.html).

As buscas iniciais não localizaram distribuição pública oficial do código de
Kyoei Toshi/GxCore. Isso não prova sua inexistência. Continuar com o dump próprio,
o runtime local e fontes públicas verificáveis; não baixar dumps de terceiros.

## Caminho atual no PC

- Launcher seleciona idioma e configuração; runtime monta common.psarc
  traduzido sobre os caminhos do pacote original. Outros dados vêm do dump.
- Executável convidado usa o carregador e as funções de sistema adaptadas
  da cópia fixa Godzilla. Adaptações são geradas em build pelo CMake.
- SDL cuida da janela e eventos. A ponte pad converte teclado e controle para
  o contrato PS4. Modalidade ativa agora acompanha eventos de origem antes
  dessa conversão, para não confundir Enter mapeado com controle físico.
- Renderer Vulkan reaproveita shadPS4. A base usa um shim ImGui com hooks
  vazios; o overlay solicitado precisa de integração gráfica própria.
- IME ainda apresenta janela Win32. O modelo de teclado de controle está
  preparado e testado, mas ainda não conectado a uma apresentação na imagem.
- Save/load ainda usam apresentação nativa com backend de pastas existente.
  Não mudar armazenamento como parte da troca de apresentação.

Seleção e viewport da lista agora usam `kyoei_list_model`, independente de
Win32 e conectado à apresentação atual. Navegação conserva wrap e prioridade
existentes; altura insuficiente produz viewport vazio, evitando underflow.
Slots/confirmar/cancelar/resultados foram exercitados por teste sintético da
ABI verdadeira com worker de UI controlado. Não escreveu saves de Davy.

## Entrada: provas offline novas

`kyoei_input_mode` registra atividade real de teclado e botões do controle.
Eixos exigem cruzar 12000 unidades e voltar a até 4096 para rearmar. Conexão
não muda modalidade; drift e eixo mantido não retomam controle depois de
atividade de teclado. Triggers consideram somente valores positivos.
Estado compartilhado entre SDL e IME protegido por SRWLOCK.

`kyoei_name_model` prepara grade de 40 teclas, caixa, espaço, apagar, cursor,
confirmação e cancelamento. Círculo ativa tecla; X cancela; Options conclui;
quadrado apaga e triângulo alterna caixa. Nomes iniciais ficam selecionados
para substituição, mas podem ser preservados. UTF-16 conta unidades e protege
pares substitutos; estado final não aceita novas edições.

IME real também valida UTF-16 inicial, aceito e retornado pelo filtro antes
de gravar no buffer convidado. Não altera nomes nem saves existentes.

## Próximas perguntas

1. HUD: identificar consumidores de propriedades, font ID, medição, clipping,
   transformação e placa. Ver docs/m3/HUD-INVESTIGACAO.md.
2. Overlay: definir apresentação/composição e fila de eventos na thread dona,
   incluindo desenho quando o jogo espera resposta sem gerar novos frames.
3. Save/load: separar modelo de seleção/resultado da apresentação sem alterar ABI.
4. Lua: rastrear scripts que ligam texto a eventos e escolhas; não executar
   bytecode como ferramenta de pesquisa.
5. Recursos: documentar referências entre tabelas, HUD, fontes, imagens e áudio;
   cobertura de texto ainda não está completa.
