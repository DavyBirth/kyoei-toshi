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
