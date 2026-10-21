# Modo foto: previa com pausa e camera livre

Abra **Testar modo foto.cmd** na raiz do projeto, com outras partidas fechadas.
Usa a compilacao atual, janela1920x1080, ingles e seus saves normais.
Na rua, com a janela ativa:

- **F10:** entrar/sair; ocultar interface e pausar pela API nativa do jogo.
- **WASD:** deslocar camera; **Espaco/Ctrl:** subir/descer.
- **Mouse ou J/I/K/L:** olhar; **roda do mouse:** zoom pelo campo de visao.
- **Shift:** deslocamento mais rapido.
- **F11:** mostrar/ocultar protagonista (experimental; validado na cena de teste, outras roupas pendentes).
- **F9:** mostrar/ocultar a barra de ajuda do modo foto.
- **Setas esquerda/direita:** escolher ajuste; **cima/baixo:** alterar valor.
- **R:** restaurar lente, rotacao, cores e velocidade.
- **F12:** salvar PNG em `photos/`, na pasta de execucao.

Primeira barra de ajuda propria, com comandos e estado do protagonista.
Ela e desenhada no host depois da captura, portanto fica fora do PNG.

No controle (Xbox / PlayStation):

| Comando | Acao |
| --- | --- |
| View / Share | Entrar/sair do modo foto |
| Analogico esquerdo / direito | Mover / olhar |
| LT/L2 / RT/R2 | Descer / subir |
| LB/L1 ou RB/R1 segurado | Movimento rapido |
| Direcional esquerda / direita | Escolher ajuste |
| Direcional cima / baixo | Alterar valor do ajuste selecionado |
| R3 | Restaurar ajustes |
| A / Cruz | Salvar PNG |
| Y / Triangulo | Mostrar/ocultar protagonista |
| X / Quadrado | Mostrar/ocultar ajuda |

Mapeamento inicial; teste com controle fisico pendente. Interfaces de referencia pedidas por Davy: Spider-Man da
Insomniac, The Last of Us Part II, Death Stranding 2 e Ghost of Tsushima.
Referencia de organizacao/usabilidade, sem alegar equivalencia de recursos.

Sair restaura a camera original e libera a pausa adquirida pela previa.
Perder foco/minimizar restaura a interface; a proxima fase do jogo libera
sua pausa. Esc/Tab e acoes de dialogo tambem encerram a previa. Espaco e
movimento vertical enquanto a camera livre esta habilitada.

O launcher portatil tem **Photo preview** em Controls, desligado por padrao.
Codigo e tela atualizados; pacotes pessoais/dos amigos ainda precisam ser
reconstruidos para receber esta versao. O arquivo de teste da raiz usa o
executavel atual sem depender desses pacotes.

## Fotos e limites

### Retorno de Davy em20/10/2026 e proximas prioridades

1. **Deriva corrigida e confirmada por Davy:** a camera se movia sozinha
   no modo foto, mesmo sem comando intencional.
   Aceite: camera parada com entradas neutras, incluindo controle conectado,
   entrada/saida do modo e perda/retorno de foco.
   Rodada curta de20/10: reproduzido movimento com valores quase centrais do
   pad; adicionada zona morta aos quatro eixos, exclusiva da camera fotografica.
   Teste falhou antes e passou depois; compilacao atualizada.
   Retorno posterior de Davy em20/10: camera estavel e afastamento funcionando.
2. **Liberdade de deslocamento:** Davy considera chato ficar perto do
   protagonista. O limite atual de10unidades e provisório; preferencia por
   camera realmente livre, sem ficar presa ao personagem. Revisar o limite
   junto com culling, streaming e iluminacao para permitir afastamento amplo
   com cenario correto, preservando posicao do personagem e retorno da camera.
   Limite radial removido na compilacao de20/10. Teste visual de afastamento
   amplo automatizado nao concluido: tentativas interrompidas por perda de foco e mudanca
   de cursor durante a validacao, sem continuar enviando entradas.

### Ajustes fotograficos implementados nesta previa

O painel mostra o ajuste selecionado e os valores. Disponiveis:

- Lente: distancia focal em milimetros, usando sensor virtual de24mm de altura.
  E uma convencao fotografica do port, nao calibracao fisica da camera PS4.
  O FOV nativo continua limitado a15..110graus.
- Rotacao da camera (roll), em passos de2graus.
- Velocidade da camera de0,25x a5x; Shift/L1/R1 acrescentam rapidez temporaria.
- Exposicao de-3 a+3EV, contraste de0 a2 e saturacao de0 a2.
- Filtros Original, Preto e branco, Sepia, Frio, Quente, Kaiju1954,
  Tokusatsu retro, Kaiju noturno e Kaiju anime.
- Granulacao e vinheta ajustaveis de0 a100%, inclusive em outros filtros.
  O preset1954 inclui preto e branco, grao e pequenas marcas de pelicula.
  O anime usa uma paleta grafica; nao modifica modelos ou animacoes.
  Trocar preset escolhe grao/vinheta iniciais; ambos continuam ajustaveis.

Cores opcionais atuam apenas na apresentacao do modo foto e no PNG salvo.
O framebuffer original do jogo nao e alterado; ao sair, a imagem normal volta.
Os valores escolhidos de cor/velocidade ficam na memoria desta execucao para
reutilizar em outra foto. R/R3 restaura os padroes, incluindo a lente de entrada.
Preview de cor para superficies8bit RGBA/BGRA SDR; HDR continua sem suporte.
A imagem de ajuda fica fora do PNG. Testes agente usam perfil privado/mudo.

### Pedidos que continuam em pesquisa

- Entrada descoberta no menu de pausa nativo: pedido explicito de Davy em20/10.
  Classe UnitHudPause localizada; acao de entrada/retorno ainda nao integrada.
  Por orientacao posterior de Davy, fica DEPOIS de foco/desfoque, iluminacao,
  clima e horario. Luzes desejadas sao fontes posicionaveis com cor/intensidade,
  como a referencia Spider-Man, e nao somente exposicao.
- Iluminacao virtual, foco/profundidade de campo, clima e hora do dia.
- Ampliar cobertura de protagonista invisivel para variantes e acessorios.

**Historico da lista desejada:**

- Controles de iluminacao.
- Ajuste de foco e profundidade de campo.
- Distancia focal da lente, pedida em20/10. O FOV ja muda o enquadramento;
  valores em milimetros dependem de definir uma equivalencia de sensor.
- Protagonista invisivel: F11 experimental; testes reais ainda NAO ocultam o corpo.
  Painel indica pedido experimental, nao garante invisibilidade aplicada.
- Filtros visuais, exposicao e contraste.
- Alterar clima e hora do dia.
- Espaco para ampliar os ajustes fotograficos conforme os mecanismos do jogo
  forem identificados.

Registrar como direcao desejada, sem prometer suporte antes de investigar.
Priorizar estabilidade e liberdade da camera. Ajustes fotograficos devem ser
opcionais, restritos ao modo foto e restaurados ao sair; ocultar protagonista
deve atuar no desenho, preservando sua logica e os saves. Clima/hora precisam
de pesquisa propria sobre efeitos, scripts e estados da cena.

O PNG vem da superficie final PS4 antes do redimensionamento e dos overlays
do port. As fotos conferidas tem1920x1080. RGBA/BGRA8bit suportados; outros
formatos, incluindo HDR10, recusados com mensagem no log. Nomes preservam
fotos anteriores. Capturar aguarda GPU/comprime PNG e pode causar pausa
curta; manter fotos fora dos benchmarks.

O deslocamento nao tem mais o limite radial de10unidades; coordenadas nao
finitas sao recusadas. Campo de visao15..110graus. A camera continua usando
os dados carregados da cena; renderizacao/streaming a longa distancia ainda
precisam ser conferidos. Outras cenas, cutscenes, transicoes,
sombras/reflexos, transparencias e streaming ainda precisam de validacao.
Exposicao tem controle host. Foco/desfoque foi validado na cena de teste e
esta no atalho experimental; outras cenas ainda precisam de verificacao.

H alterna so o guia original grande/pequeno/oculto. F10 usa estado proprio.
Filtros adicionais reconhecem a arvore do icone de mensagens e a classe
especifica do telefone3D do save; mensagens, maquina e APIs de save preservadas.
A notificacao ativa ainda precisa de reproducao para confirmar cobertura.

## Evidencia

Davy confirmou F12 em19/10. Em20/10 entrada real gerou fotos de camera girada,
avancada e elevada durante a pausa. F10 restaurou a visao normal e permitiu
reentrar. Roda do mouse alterou enquadramento; minimizar resultou em pausa0 e
FOV original40. A pausa precisou de recomposicao explicita da vista principal:
interceptar somente o compositor normal tambem congelava a camera livre.

Compilacao completa/testeSDL/walker de proveniencia passaram. Na rodada final,
o marcador azul3D do save desapareceu comF10 e voltou ao sair, preservando a
maquina e o cenario. Uma notificacao de mensagem separada ainda precisa de
reproducao; sua correcao nao foi declarada visualmente comprovada.
Detalhes em [FOTO-CAMERA-PAUSA.md](re/FOTO-CAMERA-PAUSA.md).

A distancia focal segue a convencao optica nativa de35mm; a HUD mostra mm e
explica que cima aproxima e baixo amplia o enquadramento. O filtro Kaiju anime
usa agora cores suaves, sem faixas de posterizacao; grao/vinheta sao ajustaveis.

## Luz adicional experimental

Disponivel em **Testar modo foto.cmd**. Comeca desligada. Nas setas ou no
 direcional, selecione Luz adicional e ajuste para ligar. Ha sete cores,
 intensidade e deslocamento em X/altura/Z. A luz fica perto da camera ao ser
 ligada e permanece posicionada enquanto voce reenquadra. Desligue e ligue
 para posicionar novamente perto da camera. R/R3 desliga e restaura os ajustes.

E uma fonte real do jogo: ilumina personagem/cenario e aparece no PNG, sem
 alterar as luzes da fase. Ao sair do modo foto fica desativada. Valores ficam
 apenas na memoria desta execucao, sem gravar save. Nao adiciona sombras novas.

Prova visual em20/10: branca, vermelha, intensidade, deslocamento horizontal,
 reset e dois ciclos de entrada/saida na cena201, com PNG1080p. Ainda falta
 segunda cena e controle fisico. A fonte nativa e reutilizada (uma alocacao por
 processo); trocar o sistema de luz encerra a previa por seguranca. Atalho
 normal e distribuicao anterior permanecem sem esta ativacao experimental.
