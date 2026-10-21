# Dicas que ainda aparecem na foto

Reproduzido em20/10/2026, sessao privada/muda studio-3, cena201. Mesmo com
HUD oculta, frases de dicas aparecem na parte inferior, alternam e entram no
PNG. Nao confundir com guia de composicao ou aviso de captura do host: estes
ficam fora da foto. Nao ocultar todos os textos, pois ha letreiros no cenario.

## Evidencia recuperada

Na lista draw12 e listas de update foram identificados dois proprietarios:

| Tipo | Vtable relativa ao ELF | Filhos observados |
| --- | --- | --- |
| UnitHudTips | 12d8e58 | UnitDummyText, vtable12e1ec0, flags20=19 |
| UnitHudLoadingTips | 12dc718 | Duas UnitFontEx2, vtable12e1d78, flags20=1b/5b, e sprite1256658 flags5b |

UnitHudTips ctor a65d40, init a65830, update a65970. UnitHudLoadingTips init
a92630, update a92750. Ambos usam draw98=2f0090 e walkerE0=2f01f0. O gate
existente desse walker nao basta: filhos podem ter desenho independente.
O bit40 em flags20 participa do filtro de filhos no walker; flags1b/19 nao
possuem esse bit, enquanto5b possui. Provar pertencimento a raiz atual antes
de suprimir fontes desenhadas separadamente.

UnitFontEx2 draw98=ae6820 (3681bytes/FNV168be630d35f5764), walkerE0=ae7980.
UnitDummyText draw98=2e56e0 (588bytes/FNVb1205c06d881b81c), walkerE0=ae7af0.
Walker2f01f0 tem96bytes/FNV52fde02d01a9e568 (gate existente). Hashes relativos
ao ELF proprio registrado no projeto; conferir funcao integral antes de hook.

A chamada ae705a encaminha ao base2e56e0, que chama sprite324720 em2e5701.
Entretanto ae6820 continua desenhando depois do retorno; bloquear somente o
sprite/base nao e prova de ocultacao completa. Nao reutilizar gate do MailIcon
como se cobrisse toda fonte. Nenhum hook novo foi instalado nesta pesquisa.

## Retomada

1. Identificar singleton ou dono atual dessas duas raizes e validar identidade.
2. Filtrar apenas fontes com pertencimento estrutural comprovado, incluindo
   desenho independente. Leitura bounded, falha conserva desenho. Evitar varrer
   todas as listas para cada glifo ou remover todas as UnitFontEx2.
3. Validar antes/depois com dicas presentes: sem dica no PNG, letreiros normais,
   retorno ao sair/foco perdido, segunda cena e transicao sem ponteiro antigo.

Artefatos privados: hud-studio-20261020/composition-panel.png, portrait-panel.png,
spot-aligned-panel.png, scene-units.json e leitores tips_live.py/environment_live.py.
Capturas/disassembly/fontes de texto do jogo nao sao versionados.

## Donos identificados offline em 21/10/2026

Indice privado `build/research/guest-code.sqlite`, aberto somente para leitura,
conferido contra SHA256 do ELF proprio:
`1968814b70f69ec85ff3e556c4b66b53ec298c0dea71120e4f0ba594906e418e`.
Nenhum jogo iniciado nesta rodada.

- LoadingTips: singleton em `image+152cbf0`. Fabrica `a92ed0` aloca `1e0`
  bytes, publica o objeto em `a92f4e` e instala vtable `12dc718` em `a92f5c`.
  Destruidores `a92440`/`a924f0` zeram o singleton em `a924bb`/`a9256b`.
- Tips comuns: dono publicado em `image+14976d0` pela fabrica `a50c40`
  (`a50cda`), vtable `12d7708` (`a50ceb`), tamanho `120` bytes.
  A consulta por descritor `152a7b0` ocorre em `a50f3e`/`a50f53`;
  apos verificacao de tipo, o resultado e guardado em `owner+78` em `a5104d`.
  Este campo aponta para a raiz `12d8e58`, nao para uma fonte individual.
  Destruidores `a4fce0`/`a4fd40` zeram o singleton em `a4fd22`/`a4fd85`.
- `root+40` contem o recurso de layout carregado por `2f05a0`, nao um
  ponteiro de pai. `2f0420` instancia os filhos e `2f03c0` os associa.
  Nao inferir ancestralidade seguindo `+40`.

Preparado `src/kyoei_photo_tips_membership.h`: resolve os dois donos atuais,
confere identidades e pertencimento em arvore limitada a 256 nos/profundidade8,
e reconfere singleton, dono, raiz e tipo da fonte antes de aceitar. Somente
UnitDummyText/UnitFontEx2 podem ser alvo. Nenhum cache de ponteiro entre chamadas.
O walker estrutural ja usado pelo MailIcon foi compartilhado, mantendo sua
entrada anterior; adicionadas protecoes contra overflow de enderecos.

Teste sintetico `test_kyoei_photo_tips` PASS: ambos os donos, texto externo de
mesmo tipo preservado, sprite/tipo errado rejeitado, dono trocado durante a
leitura, singleton destruido e ponteiros invalidos. Teste anterior
`test_kyoei_photo_phone` PASS, incluindo ciclos e limites de trabalho.
Compilacao direta com Clang C11, `-Wall -Wextra -Werror -UNDEBUG`.

**Estado no commit `0891278`: ainda nao conectado ao runtime.** Prova estrutural offline, nao prova
de ocultacao. Antes de instalar gates em `ae6820` e `2e56e0`, conferir assinaturas
integrais e sincronizacao de lifetime com o desenho. A releitura detecta troca
de ponteiro, mas nao garante lifetime em concorrencia nem detecta reutilizacao
do mesmo endereco. Evitar busca em arvore para cada glifo; avaliar custo por
objeto de fonte. Depois, quando Davy liberar execucao, provar PNG sem dicas,
letreiros preservados e restauracao em saida/foco/transicao/segunda cena.

## Gates experimentais preparados em 21/10/2026

`src/kyoei_photo_tips.c` instala os dois gates somente com
`KYOEI_PHOTO_TIPS=1` e gate HUD disponivel. Nenhum atalho foi alterado para
habilitar esta opcao. Entrada pelo configure da HUD, durante boot e antes
das threads convidadas. Fora de HUD oculta **e** cena fotografica ativa,
encaminha a chamada original. A condicao e conferida novamente depois da busca.

Hashes integrais e limites do unwind conferidos novamente no ELF proprio:
`ae6820`, 3681bytes/FNV`168be630d35f5764`;
`2e56e0`, 588bytes/FNV`b1205c06d881b81c`.
Ambas com prologo completo6bytes `push rbp/mov rbp,rsp/push r15`, coberto
pelo relay SysV existente. A unica chamada direta encontrada entre elas e
`ae705a -> 2e56e0`; filtrar apenas a base continuaria permitindo o desenho
posterior da estendida. Ha um gate na entrada de cada objeto, nao por glifo.

As duas assinaturas, relays e permissoes sao preparados antes de alterar
qualquer entrada. Falha preparatoria conserva ambas. Originais publicados
antes das escritas; falha irrecuperavel de protecao/cache encerra como nos
outros gates. Leitura de dados via `ReadProcessMemory` evita o intervalo
inseguro entre VirtualQuery e memcpy para memoria liberada. Nenhuma escrita
em fontes, flags, raiz ou listas; nenhum ponteiro de cena fica em cache.
Releituras continuam sem garantir snapshot coerente ou detectar reutilizacao
do mesmo endereco. Por isso a opcao permanece experimental/desligada.

Validacao offline:

- `test_kyoei_photo_tips_gate`: callbacks reais com spies SysV; ocultacao das
  duas fontes, caminho original fora da foto, texto externo do mesmo tipo,
  restauracao durante a busca, dono destruido, PAGE_NOACCESS/memoria liberada,
  e log limitado a oito amostras. PASS.
- Argumento opcional com caminho do ELF proprio: copia somente as duas funcoes
  para memoria de teste e verifica instalacao, prologos dos relays, protecaoRX,
  segunda assinatura adulterada sem instalar a primeira e chamada repetida.
  **Nenhuma funcao do jogo e chamada.** PASS.
- `test_kyoei_hud_trace`: relay real executado com codigo sintetico, argumentos
  e retorno SysV preservados. Corrigida dependencia ausente de
  `gz_memory_clamp` no teste, com adaptador para memoria das fixtures. PASS.
- Testes estruturais tips/phone PASS; build completo `kyoei.exe` vinculado.

Falta executar em cena com dicas presentes e comparar PNG/letreiros/custo,
saida/foco/transicoes e segunda cena. Nao declarar vazamento visual resolvido.
