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
Lente usa convencao sensorvirtual24mmaltura: f=12/tan(FOV/2), com limite
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
visual sao separados. Binding682a50->276ac0 sobreplayer+6c0 em investigacao.
