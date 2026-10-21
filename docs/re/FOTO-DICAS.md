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
