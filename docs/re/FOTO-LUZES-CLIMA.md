# Pesquisa de luzes e ambiente no modo foto

Estado atual em20/10/2026: tres luzes extras implementadas e comprovadas,
com tipos geral/spot, cor, potencia, posicao e direcao. Clima/horario ainda
nao implementados. As secoes anteriores abaixo preservam a evolucao da pesquisa;
o resultado mais recente esta ao fim deste documento.

## Fontes de luz

RTTI registra GxUnitLightPoint (descriptor1356900, parent13892c0), instancia
624bytes via2ac650, vtable1254ed0. Essa tabela aparece na drawgroup7 da cena
privada201. GxUnitLightSpot (1356b90, parent1389390) usa656bytes/2ac8f0.
Construtor da point chama358af0; nao basta alocar memoria host e copiar a
tabela: ha inicializacao e registro nativos que precisam de dono/destrutor.

A virtual90 point2ac830 (184bytes) atualiza alcance em+22c com base no brilho
RGB acessado pela virtuale0. Accessor2acc40 retorna self+230. Parametros
+250/+254 participam do calculo. A virtual98 compartilhada359f00 (222bytes)
e principalmente culling e flags por viewport; nao assumir que seja emissao
final da luz. O helper359fe0 consulta transformacao e camera para visibilidade.

Proximo passo: localizar registro/coleta no GxSysLight e ciclo de vida, provar
uma fonte propria em perfil privado, depois adicionar mover/posicionar, RGB,
intensidade e alcance na HUD. Manter luz da fase intacta; nao reaproveitar uma
luz existente silenciosamente como se fosse uma fonte adicional.

## Ambiente

UnitSkySphere (descriptor1556d18, parent1389540, ctorba99f0,1056bytes)
existe no registro. Isso nao prova que exista um relogio continuo ou ciclo
climatico. Chuva/ceu/iluminacao podem depender dos scripts de cada fase.
Antes de oferecer horario, distinguir horario narrativo da iluminacao visual;
alterar uma legenda/relogio nao equivale a trocar a luz da cena.

Guardar transformacoes e propriedades somente com identidade e dono atuais;
restaurar ao sair/foco perdido, sem gravar save. Validar cena201 e uma segunda
cena antes de promover para o atalho usado por Davy. Dumps/disassembly e
screenshots permanecem na pasta privada ignorada build/research/photo-mode.

### Registro e ciclo de vida

GxSysLight ctor290910 aloca104bytes e chama3439f0. Esse inicializador define
singleton1354420, inicializa lista emself+20 e sincronizacao emself+40.
A insercao343c20 usa lock321250, lista2ff480, unlock3211e0. Portanto a chamada
vista no construtor point e registro da luz no sistema, nao apenas metadado.
Uma implementacao futura precisa provar a remocao simetrica antes de criar
fontes extras: deletar copia host ou descartar ponteiro deixaria registro vivo.
Nao chamar construtor de GxSysLight de novo: singleton da cena ja existe.

A vtable point tem entradas0/8 em209b60/209b70, compartilhadas pelo objeto
base; a primeira encaminha a2f5d50, a segunda tambem libera pelo alocador.
Isso ainda nao prova retirada das listas da cena: antes de liberar, investigar
finalizacao/gerenciamento de vida da unidade. Nao tratar essas entradas como
API pronta para apagar luz em uso por jobs do renderizador.

### Instancia ativa e copia compacta —20/10

Leitura somente do perfil privado encontrou singleton1354420 apontando para
vtable1254b48 (GxSysLight, finalizada no ctor290910). A tabela1259ba0 e a
classe-base GxSysLightBase, escrita temporariamente pelo inicializador3439f0.
Nao filtrar apenas pela tabela-base: isso descartava a instancia real.

A lista emself+20 tinha92nos. No1257608 tem setter1e9290 que apenas guarda
ponteiro em+8; nao presumir que esse registro retenha o objeto. Destructor
2f5d50 e apenasret. A finalizacao/geracao da unidade ainda precisa prova.
Tipos reais encontrados: GxUnitLightShadow, GxUnitLightPoint, UnitSpotLight,
UnitAttachPointLight, UnitOneshotPointLight, UnitPointLightG e UnitSpotLightG.
Os tiposGI registrados no executavel nao comprovam que sejam usados aqui.

Observeropt-in `KYOEI_PHOTO_LIGHTS=observe` em343b10 preserva a saida nativa.
Valida funcao inteira260bytes/FNV05e68dd6b0c7aae0. O trampolim de5bytes
preserva EAX e flags de `xor eax,eax; test r8d,r8d`, incluindo aJE original.
Teste executa ambos os ramos em codigo sintetico e cobre saidas inalteradas.

Prova real privada/muda `light-observe-20261020/game-live-system.log`: coleta
antes e durante foto, caller35f6df, types/viewsffffffff, capacity4/count4.
Isso comprova atividade, nao emissao de luz nova nem que seja o coletor final.

Caminho relevante encontrado: GxSysLight virtual30 ->290b60 limpa mortos via
343a50 e chama290b80. Este monta cache self+58, count+60, capacity+64; cada
entrada16bytes contem ponteiro de luz, tipo+8 e mascara+0c. A virtual40,
290cf0 (1457bytes), consome esse cache, filtra tipos/camera e ordena fontes.
Tem argumentos adicionais na pilha: nao reutilizar a assinatura simplificada
de343b10.

Observer estendido a290b80 (361bytes/FNVdd610201f41bd4bc), com trampolim
separado para prologo de6bytes; teste executavel cobre preservacao da pilha.
Teste privado/mudo game-cache.log confirmou8amostras na pausa do modo foto:
caller345322, count29/capacity64. Saida do modo foto registrada normalmente.
Teste unitario confirma que entradas, ponteiros e contadores ficam intactos.
As duas assinaturas completas sao conferidas antes da instalacao; sem a
variavel opt-in nenhum observador e instalado. Ainda nao ha nova luz emitida.

Proximo passo: provar uma fonte propria nesse caminho sem alterar as luzes
originais nem registrar ponteiros que possam ficar pendurados. A atividade do
cache na pausa nao comprova por si so que uma entrada extra sera renderizada.

### Selecao final de luzes — retomada20/10

Prioridade reafirmada por Davy: continuar modo foto; minimapa fica para depois.
Observer opt-in agora inclui290cf0,1457bytes/FNVff3f63c35f31e2dc. Assinatura
SysV reconstruida com seis argumentos nos registradores e tres na pilha:
sistema, tipos, views, origem, camera, filtro, saida, capacidade, viewport.
Origem/camera/filtro sao encaminhados sem interpretar seus contratos completos.
A rotina ordena as fontes e limita a saida pela capacidade. O observer chama
primeiro a rotina original, preserva retorno/ordem e registra somente depois.
Registros limitados a4amostras normais/8na foto, com tipos point/spot escolhidos.
Nao confundir selecao para o renderer com prova de iluminacao visivel.

Teste executavel PASS: nove argumentos, resultado, sentinela alem do retorno,
conteudo das fontes/cache intacto e trampolim6bytes com os tres argumentos da
pilha preservados. Build completo vinculado em executavel privado
build/research/photo-mode/kyoei-photo-lights.exe. O kyoei.exe normal estava
ocupado pelo PID24736, com pasta preparada claude-test; partida preservada.
Nenhum jogo novo iniciado. A prova desse novo observer em cena ainda falta.
Sem KYOEI_PHOTO_LIGHTS=observe, nenhum dos tres hooks e instalado.

Proximo passo: coletar a selecao real na pausa quando a partida concorrente
terminar, identificar fontes pontuais efetivamente selecionadas e concluir o
ciclo de vida de uma fonte propria antes de oferecer posicionamento na HUD.
Nao copiar uma luz da fase nem chamar destructor incompleto como atalho.

### Fonte propria experimental — continuacao20/10

Modo separado KYOEI_PHOTO_LIGHTS=source-proof. Nao habilitado em lancadores.
O modo observe continua sem modificar fontes. O prototipo chama a fabrica
nativa2ac650 na thread que reconstrui o cache, somente ao entrar na foto com
camera valida. Nao copia uma luz existente nem modifica fontes da fase.
Fabrica, inicializadores358af0/359b90/357410 e update2ac830 sao conferidos por
assinatura integral antes da instalacao. Posicao inicial:3unidades a frente
mais1para cima no referencial da camera fotografica. RGB de prova8/.2/.2;
update nativo calcula alcance. Mascara limitada ao viewport principal.
Ainda nao ha controles de posicionamento/cor na HUD: e uma prova de emissao.

Ciclo deliberadamente limitado: uma unica instancia nativa reutilizavel por
processo, registrada pela propria fabrica; fica desativada (+220=0/+30=0)
ao sair da foto, antes da reconstrucao do cache. Nao liberar enquanto caches/
jobs possam guardar o ponteiro. A alocacao permanece ate terminar o processo;
nao alegar que destructor/remocao simetrica foram resolvidos. Troca do ponteiro
de sistema encerra a prova sem seguir a instancia antiga. Isso ainda nao
certifica recarga de cena, reutilizacao do endereco ou funcionamento geral.

Teste com fabrica/update substituidos por spies PASS: nao criar fora da foto
ou sem camera, criar uma unica vez, ativar/desativar/reentrar, nao tocar fontes
originais/cache e recusar sistema substituido. Isso nao e prova de chamada da
fabrica real ou iluminacao visivel. Compilacao dos fontes reais e vinculacao
completa do executavel privado PASS.

Sessao privada/muda preparada em
build/research/photo-mode/light-source-20261020/run.ps1, com user separado.
Executavel: build/research/photo-mode/kyoei-photo-lights.exe. Roteiro recusa
iniciar se kyoei ou kyoei-photo-lights estiver ativo. PID24736/claude-test
continua aberto, preservado; nenhum jogo iniciado nesta etapa.

Quando houver janela livre: carregar cena201, capturar antes/F10/depois,
conferir created/enabled/disabled no log e selected_photo_source=1 na selecao
final; comparar PNG e host. Testar sair/reentrar e perda de foco. Se nao houver
emissao, rastrear montagem dos parametros de luz, sem promover o prototipo.
So apos prova visivel implementar controles e validar uma segunda cena.

### Emissao real e controles comprovados —20/10

Davy autorizou testes e esclareceu que clima/horario devem ser estudados sem
 forcar: se nao forem viaveis, finalizar os recursos uteis, integrar no menu
 e seguir as outras frentes. Nao chamar falta de prova de impossibilidade.

Partida muda native-source-1, PID37992: fabrica nativa criou uma unica fonte,
 selecionada em290cf0 (selected_photo_source=1). A luz vermelha iluminou
 personagem, parede e maquina, inclusive PNG1920x1080. Dois ciclos F10:
 created1/enabled2/disabled2; saida normal0. Amostra da parede em
 (750,100)-(1250,330): RGB medio antes33.95/35.85/35.43, ligada117.99/41.72/40.88,
 depois32.65/34.35/33.62. Segundo ciclo confirma o comportamento. Chuva/videos
 voltam a correr fora da foto; nao alegar igualdade de PNG inteiro.
 Relatorio privado light-source-20261020/native-source-report.json.

Modo adicional preview exposto somente em Testar modo foto.cmd. Fonte inicia
 desligada; painel permite ligar/desligar, sete cores, intensidade0.25..16 e
 offsetsX/Y/Z -50..50. Origem fixada ao ligar: a camera pode se mover sem
 arrastar a fonte. Desligar/religar reposiciona perto da camera. R/R3 restaura
 desligada. Entrada host sanitizada/transferida com lock; escreve na instancia
 propria na thread de reconstrucao do cache, sem alterar fontes originais.

Partida muda controls-1, PID28176: ligou branca, mudou para vermelha, aumentou
 intensidade a3, moveuX+2 eX-2, resetou comR e saiu comF10. Painel e PNGs
 confirmados, disabled no reset, saida normal0. Capturas privadas
 controls-panel.png e cincoPNGs da rodada. Testes de parametros invalidos,
 limites/reset, navegacao com/semDOF/luzes e layout real720p/1080p PASS.
 Build completo PASS. Ambas as partidas do agente encerradas normalmente.
 Segunda cena/roupas/controle fisico continuam pendentes.

Pesquisa de ambiente avancou: registro ba89d0 de UnitSkySphere expoe
 _skyDayColor/_skyNightColor, sol/lua e horizonte, alem de rotacao e propriedades
 astronomicas. UnitHudDateTime registra _day/_time junto de _place/_money/
 _treasure: nao e prova de relogio de iluminacao. Falta provar instancia ativa
 de SkySphere na fase e consumidor temporal antes de decidir viabilidade.

### Tres luzes, HUD e apontamento relativo

Tres slots independentes; duas instancias reutilizaveis por slot (geral/spot),
no maximo seis alocacoes nativas por processo. Instancia do tipo anterior e
desativada antes de alternar. Mantida a restricao de ciclo de vida descrita
acima: nao liberar unidade registrada com referencias possivelmente pendentes.

Spot factory2ac8f0/vtable1254fc8, update2acad0. Direcao normalizada+250 e
angulos externo/interno+260/+264 em radianos; setters nativos358da0/358de0
fazem conversao de graus. Consumidor293e40 transforma a direcao na vista e usa
2acb90 para cosenos/meio angulo. Prova studio-1 capturas6a9: cone60 ilumina
parede amplamente, cone15 forma foco estreito, girar retira facho da parede.

Ao ligar, origem e orientacao sao ancoradas na camera fotografica. Os controles
de giro/inclinacao aplicam offsets dessa orientacao; a luz permanece fixa ao
reenquadrar. Teste cobre camera voltada-X e giro90. Controle direto WASD/pad
neutraliza entrada da camera. Prova privada studio-2: deslocamentoZ-0.59,
PNG e painel; ao sair camera_distance_from_entry=0.00, luz desativada. Controle
fisico e segunda cena ainda precisam prova.

### Viabilidade de clima e horario: leitura do ambiente ativo

Pesquisa somente leitura do perfil privado studio-2. GxSysEnvironment ativo
em singleton1385028, vtable1259898, tamanho710; inicializador33c450. Existe
calculo astronomico real na virtual30/33cb40: data+38, fracao temporal double
+48, passo+58, direcao solar+70 e cores derivadas+80/+90. Isso e infraestrutura
do motor; nao comprova que as luzes da fase acompanhem esses valores.

Registro33fca0 expoe latitude+20, longitude+24, altitude+28, obliquidade+2c,
rotationTime+48, rotationSpeedScale+58, cloudyFactor+64, stormyFactor+68 e
blizzardFactor+6c. Na partida observada, fatores clim?ticos e passo temporal
estavam zerados. Leitura de2313 referencias de unidades em listas de update/
draw nao encontrou UnitSkySphere; cobertura limitada as listas examinadas.

Os consumidores diretos do singleton recuperados foram gerenciamento do
sistema (344cc0/345170) e metodos de UnitSkySphere. Nao foi provada ligacao
entre esse relogio e a iluminacao carregada da cena201. PARAM_WEATHER e
WEATHER_1..4 sao constantes registradas na API Lua6980b0; icones de clima sao
usados no HUD. Nenhum desses identificadores prova troca visual de clima.

Decisao desta rodada: nao expor hora/clima a partir desses campos, nem alterar
estado narrativo para simular o recurso. Ainda nao e prova de impossibilidade.
Faltam identificar emissores de chuva por recurso e uma fase com SkySphere,
bem como restauracao segura e dependencias de sombras/GI/ceu. Priorizar
funcionalidades fotografias independentes enquanto isso.
