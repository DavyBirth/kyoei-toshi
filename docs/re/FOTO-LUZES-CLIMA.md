# Pesquisa de luzes e ambiente no modo foto

Estado em20/10/2026: pesquisa local do dump proprio. Nenhuma luz extra, troca
de clima ou horario implementada. Nao confundir exposicao/paleta com esses
recursos. Prioridade segue foco/desfoque, luzes, clima/horario e menu de pausa.

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
