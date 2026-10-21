# Entrada do modo foto pelo menu nativo

Pesquisa estatica de21/10/2026. Nenhum jogo aberto, nenhuma alteracao nos
layouts do dump ou nas rotinas de menu. Ainda nao ha opcao nova no menu.
ELF proprio CUSA03562 01.03, SHA256:
`1968814b70f69ec85ff3e556c4b66b53ec298c0dea71120e4f0ba594906e418e`.
Indice privado `build/research/guest-code.sqlite` conferido contra esse hash;
rotinas e limites abaixo tambem conferidos no ELF. Analise local em
`build/research/photo-mode/pause_menu_offline.py` (nao inicia processos do jogo).

## Objetos diferentes

| Objeto | Descritor | Vtable | Acesso identificado |
| --- | --- | --- | --- |
| UnitHudPause | 153d6d0 | 12e1520 | Global153d780 |
| UnitMenuManager | 1543c70 | 12e56c0 | Singleton1543dd0 |
| UnitMainMenu | 1543810 | 12e5320 | CampoA10 do gerenciador |

Todos os enderecos sao relativos a imagem. Nao confundir painel visual
UnitHudPause com o menu interativo UnitMainMenu.

- Raiz de pausa criada via355db0 em6e35ce, grupo9/argumentoC; tipo verificado
  em6e35f6, publicado em153d780 por6e360d. Campo global zerado em6e497a.
  Fabrica propria ae3660; tamanho270. Vtable instalada em ae36be/ae36c5.
- Gerenciador: fabrica b33d20 aloca13f0, publica singleton em b33daf e usa
  vtable12e56c0. Inicializacao b2b580, update b2c820 (13498bytes).
- Menu principal: b2c207 cria via355db0, grupo8/argumentoE; valida tipo e
  guarda em manager+A10 por b2c23e. Fabrica b2a5e0/tamanho2c0. A tabela base
  12e54f0 e substituida pela final12e5320 em b2a6a7/b2a6b8.

## Abertura e fechamento compartilham estado

O gerenciador consulta `[image+13361f0]`, com mascara em `+317c` e indicador
em `+e0`. Ha consumidores de outros bits nas telas do jogo: nao zerar o campo
inteiro ao sair da fotografia.

- b2c9f1/b2c9fb chama pausa.vtable+F0 (ae2f90); b2ca0b liga bit1 da mascara
  e b2ca12 escreve e0=1.
- Outro caminho em b2cd75..b2cda0 liga o mesmo bit e abre o painel.
- Fechamento b2cbfd..b2cc24 e b2d2f0..b2d30a remove somente bit1, zera e0
  apenas se a mascara restante for zero e chama pausa.vtable+100 (ae3220).
- ae2f90 usa estado1 em2effe0 e habilita desenho dos filhos; ae3220 usa
  estado2 e desabilita desenho dos filhos, alem de animacoes condicionais.

Estes campos nao substituem automaticamente a posse NativePause do modo foto,
que usa6f4600/manager+44d8 conforme FOTO-CAMERA-PAUSA.md. O vinculo nativo foi
localizado no update do gerenciador:

- b2d54a passa `menu_manager+B00` como lista de exclusoes; b2d559 prepara
  paused=1 e b2d566 chama6f4600 no gestor de unidades.
- b2e6b7 passa a mesma lista; b2e6cd prepara paused=0 e b2e6d7 chama6f4600
  apos limpar menu_manager+178 em b2e697.
- 6f4600 registra o pedido em gestor+44d8 e altera grupos/flags das unidades.
  Um segundo pedido nao zero quando ja pausado retorna cedo em6f4631..6f4642;
  portanto chamar novamente paused=1 nao reaplica necessariamente novas
  exclusoes (a fotografia hoje usa exclusao12, nao listaB00 do menu).

Entrada pela pausa precisa transferir posse de
forma que sair da foto nao solte uma pausa ainda exigida pelo menu/cutscene.

## Navegacao e layout possuem seis entradas fixas

Inicializacao b28ee0 carrega o layout tecnico
`common/hud/menu/main/menu_main` (referencia b28f87).
Arquivo extraido proprio `common/hud/menu/main/menu_main.hud`:
1473bytes, SHA256`167268dfda64c0c37fae7c7785dd7249f263298d4ab1e3a74eff0813e90971e8`.
EnvelopeBILZ, payloadzlib em144, expandido6651bytes. Sem reescrita nesta rodada.
Labels tecnicos selectbase01..06/selecttxt01..06 e selectcursor; seis curvas
menu_main_select01..06.crv acompanham a navegacao.

O metodo virtual+1A0, b296b0, contem os limites e tratamento de entrada:

- Indice atual menu+288 limitado a0..5 em b29e71..b29e83.
- Array de indices de filhos em+264, acessado por b29eb4; ha checagem de
  contagem de filhos e uso separado do filho10 para o cursor.
- Array de categorias em+24c acessado por b29fe3. O indice5 tem caminho
  especial em b29fad..b29fc9, alterando estado+298 e campo+248.
- Os demais chamam b303d0 no gerenciador com a categoria escolhida.
  **b303d0 nao e um despachante generico de acoes**: calcula transicao circular
  entre seis categorias, preenche arrays+AAC e altera estados/animacoes.
  Nao escolher um ID novo e chamar esta rotina como se aceitasse qualquer acao.

Portanto adicionar a setima opcao exige layout/curvas, navegacao, arrays,
cursor e um caminho proprio para fotografia. Alterar so a contagem ou
substituir uma opcao existente ainda nao e uma integracao correta.

## Assinaturas para retomar

Metadados apenas; nenhum byte de executavel ou texto original completo no Git.

| Funcao | Bytes pelo unwind | FNV64 integral |
| --- | --- | --- |
| ae2f90 | 645 | 5c4baef97e5ea846 |
| ae3220 | 327 | 9434afc4cee5fb00 |
| b33d20 | 1840 | 99f73792ec438407 |
| b2b580 | 4219 | d841b51d9f494ff0 |
| b2c820 | 13498 | 521c1319d99bbc5b |
| b2a5e0 | 350 | 654afd6fe0936d00 |
| b296b0 | 2619 | fc85cea13d1fc742 |
| b303d0 | 408 | 70e3cc3e65a3c34d |

## Proxima prova necessaria

1. O elo6f4600 ja foi identificado em b2d566/b2e6d7. Identificar agora o
   momento de fechamento/animacao e as exclusoesB00 para entregar a camera
   ao host. O enter atual recusa gestor+44d8 diferente de zero: nao remover
   essa protecao para simplesmente adotar uma pausa alheia.
2. Identificar criacao e propriedade dos filhos do layout, antes de escolher
   adicionar linha nativa ou compor um comando host no contexto do menu.
3. Testar primeiro observador somente leitura, quando execucao for liberada:
   menu principal, opcoes, cancelar, cutscene/loading e retorno. Confirmar
   transicoes/identidades reais antes de escrever flags ou adicionar uma opcao.
4. Provar entrada/saida da fotografia mantendo selecao, foco, simulacao e
   controles. A existencia de enderecos estaticos nao comprova essa integracao.
