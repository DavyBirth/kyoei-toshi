# Camera e pausa da previa fotografica

20/10/2026,CUSA03562 01.03,dump proprio. ELF SHA256
1968814b70f69ec85ff3e556c4b66b53ec298c0dea71120e4f0ba594906e418e.
Somente ferramentas/metadados no Git; bytes/desmontagens/imagens privados.

## Integracao

KYOEI_PHOTO_SCENE=preview: hooks3542e0(inicio gestor),326300(composicao)
e6f4600(pausa nativa),assinaturas inteiras e prologos6bytes conferidos.
SysV relay reutilizado; prologo do compositor usaR14, copiado exatamente.
Controlehost/QPC separado do tempo convidado. NativePause so adquirida quando
gestor+44d8=0,exclusao12 como binding original. Pedido nativo posterior recebe
posse; nao desfazer pausa preexistente. Troca gestor/camera/tabela encerra;
nao seguir instancia antiga potencialmente destruida. Core fixo intacto.

Global1340800 -> viewport0(+48) -> camera(+18),confirmada na cidade:
tabela127d4f8,eye(-2,879;1,644;-77,154),alvo(-7,477;1,644;-77,150),FOV40.
Cameras zeradas no loading nao eram a camera da cidade. GxUnitCameraFreeView
existe,mas lifecycle nao mapeado; nao criamos objetoextra.

Override curto eye80/alvo260/up270/FOV284 durante compositor original;
fontes restauradas antes de retornar,matrizes calculadas permanecem. Pausa
omite camera-update normal: recompor explicitamente vista principal apos
3542e0 permite movimento. Bridge neutraliza botoes/sticks/gatilhos convidados
desde pedidoF10. InputSDL testa essa neutralizacao/foco/restauracao.

Grupo10=UnitEffectAttach,tabela12d2320. NativePause limpa mascara0x1 em+20,
dispatcher355b70 exige0x2 para desenho,preservada. Tratamentoespecial nao e
prova de efeito oculto. Outros efeitos/cenas ainda precisam comparacao.

## Ocultacao adicional

Gate2f01f0 so percorre filhos(flags20&0x42)==0x42. Filhos registrados
independentemente via355db0 podem escapar. KYOEI_PHOTO_PHONE=1 filtra324720
apenas descendentes da raizMailIcon1534530/table12dd330,arvore256nos/depth8,
tipos containers conferidos,revalidacao da raiz e falha mantem desenho.
Nao consome mensagens. Teste de walker passou; cena usada nao tinha MailIcon
ativa,logo nao prova atribuicao ao relato. Notificacao ativa pendente.

KYOEI_PHOTO_SAVE_MARK=1 filtra35b810 apenas table12d2060(UnitSaveMark),
modelo/animacao tecnicos save_point_mark_00. Maquina e APIs de save separadas.
Instalacao antes deTLSopt permite assinatura original inteira incluindoFS:[0];
adaptacaoTLSposterior segue caminho original. Atualizacoes do marcador intactas.

## Provas e limites

Roteiros privados photo-scene-observe-20261019,photo-scene-preview-20261019,
photo-scene-compose-20261020. Primeiro teste pausou,mas camera nao moveu.
Segunda versao(recomposicao explicita) mostrou giro/avanco/subida e retorno
F10/reentrada. Roda mudou FOV,minimizar liberou pausa e restaurou FOV40.
Dois PNGs pausados diferiram em367pixels,regiao380..400/859..905; restante
igual. Nao atribuir regiao a simulacao/efeito/defeito sem identificar.

Fotos1920x1080 em photos/,ignoradas. Leituras/roteiros privados em build/research.
photo-save-mark-final-20261020 confirmou supressao da classe12d2060 no log e
comparacao visual antes/durante/depois: marcador azul some/volta,maquina e
cenario permanecem. Foco/minimizacao no binario final liberou pausa0/FOV40.
O roteiro abriu interacao Listen,nao comprovou abertura do dialogo de save.
Repeticao com teclaE mantida200ms abriu a descricao nativa da maquina; Space
cancelou,estado final pausa0/FOV40. Assim a interacao permaneceu funcional;
nenhum save foi confirmado ou sobrescrito. Teclas de gameplay precisam durar
um poll do pad; pulsos instantaneos F10/F12 sao tratados pelo SDL diretamente.
Pendentes: notificacao ativa,menus/posse de pausa,
cutscenes/transicoes,outras placas,audio na pausa(testes agente mudos) e
historicos temporais/iluminacao/culling que possam ler fontes originais.


### Candidato20/10: protagonista e interface propria
Filtro35b810 existente ampliado para UnitPlayer table12cba00, apenas quando
HUDhidden e F11 ativo. Ctor9fccf0/296bytes FNV94d0a273f4b2854d e thunk740240/
15bytes FNV810839f2dae557fa validados antesTLS; assinatura diferente desabilita
opcao. Nao mexe em simulacao/collisao/save. Teste offline usa callback real:
player omitido apenas ativo, NPC mantido, restauracao e assinatura indisponivel.
Nao prova cabelo/roupas/sombras/reflexos/objetos carregados nem partida real.
Barra ImGui em photo_panel, composta via callback antesRender do model, depois
do readbackPNG nativo. Framebuffer do jogo preservado com loadOpLoad; barreiras
existentes incluem photoHUD. Teste ImGui real verifica painel dentro da tela
em720/1080, modos teclado/controle; integracao Vulkan em partida pendente.
View/Share (SDL_BACK, sem mapeamento guest) entra/sai; gatilhos altura com zona
morta16, L1/R1 rapidez, dpad FOV, SouthPNG/Northplayer/Westajuda. Comandos guest
neutralizados enquanto foto; controles fisicos ainda precisam de validacao.

### Ajustes host20/10
Lente segue agora a convencao optica nativa35mm: f=17.5/tan(FOV/2), com limite
nativo15..110. Roll aplica rotacao do par right/up emtorno forward somente
no compositor temporario; reset usa FOV de entrada/roll0. Velocidade host.
GradeGPU fullscreen amostra frame de apresentacao e escreve swapchain; nada
na imagem guest e modificado. Descriptor por slotFrame, atualizado somente
apos fence existente; barreiraColorWrite->ReadWrite antes da ajuda. Recursos
liberados apos schedulersFinish, recompostos comwaitIdle sobsubmit_mutex se
formato muda. Apenas8bitRGBA/BGRA SDR; CPUnoPNG usa a mesma transferencia
sRGB/grade linear. Neutro nao transforma pixels nem executa passagemGPU.
Grade: ganho2^EV, curva contraste emtorno18%, saturacao/lumaRec709,5presets.
Telas e fotos privadas verificadas; ajuda foraPNG, reset e lens/roll reais.
F11 filtrando35b810 (base/UnitMrPlayer2) NAOocultou corpo: root e componente
visual sao separados. Binding682a50->276ac0 REJEITADO: player+6c0 e GxModObjectHit (colisao). Nao usar para invisibilidade.

### Prototipo de foco nativo20/10
KYOEI_PHOTO_DOF=1 instala override opt-in antes da otimizacaoTLS, somente com
assinatura completa conferida. Hook inicial na unidade DOF nao executou nesta
fase; retirado. A fase usa SysFilter (derivado registrado de GxSysFilter), e
nao instancia a unidade DOF generica. O candidato atual usa preparacao de fila
de efeitos2458a0 e aceita somente singleton/tipos base ou derivado conhecidos.
ModoOriginal e padrao. Manual calcula transicoes opticas conforme convencao
nativa e conserva exposicao; fonte da camera permanece intacta. Desligado usa
enable nativo; parametros originais voltam na preparacao apos sair. Mudanca
de singleton/manager invalida cache antigo, sem escrever em objeto antigo.
Foco .2..1000, abertura f/1..32; campos novos no fim da selecao. HUD indica
indisponivel ate observar sistema valido. Teste isolado exercita codigo real
de override/restauracao. Teste real photo-dof-render-20261020: hook executa,
parametros mudam e restauram; imagem estatica nao muda (media0, p99=0).
Portanto DOF NAO funcional visualmente; opt-in de pesquisa somente,
nao ativado no atalho. Precisa localizar o consumo real dos parametros.

### Consumo por viewport confirmado em20/10, continuacao

O prototipo anterior escrevia parametros legados+a0/+b0, ausentes do caminho
efetivo de renderizacao da fase. A copia2464f0 (1664bytes,
FNV17004802587f7a3b) transfere o bloco viewport-zero+790 para o ring de efeitos.
O consumidor247310..24749b usa esse ring+4a10, chama1fa70->57780->57820,
e a preparacao optica54ce0 le os campos correspondentes do CPostEffect.
Nao confundir o bloco+850 com DOF:20610->46780 e motion blur, confirmado
pelo aviso tecnico nativo; nenhuma alteracao nesse bloco foi integrada.

Candidato opt-in agora intercepta2464f0 apenas para singleton/tabela conhecida,
manager vivo e viewport principal atual. Ajusta habilitacao, intensidade1,
modo manual, distancia, abertura e FOV em graus (consumidor converte a radianos).
Preserva escala optica+7e8 e todos os demais campos; restaura o bloco fonte
imediatamente apos a copia nativa. Nao escreve em camera, saves ou fontes antigas.

Prova privada transfer-20261020,1920x1080,muda: foco0,74/f1 produziu desfoque
visivel na tela e no PNG. Crop estatico820x500: diferenca media10,4892/canal;
foco33,64, Desligado e Reset voltaram a diferenca0 nesse crop. PNG Reset e
Original identicos byte a byte (SHA25649eff3c986ff6065278c1af9757d8efa89144da4b115a48cce4428b3466d5f27).
Painel permaneceu nitido e fora do PNG. Primeira prova visual positiva de DOF.
Comparacao dedicada f1/f32 ainda pendente: uma sequencia selecionou cores
por conservar selecao anterior; tentativas corrigidas pararam por perda de
foco. Nao usar essas capturas como prova de abertura. Segunda cena pendente.
KYOEI_PHOTO_DOF=1 continua necessario; atalho normal ainda nao o ativa.
