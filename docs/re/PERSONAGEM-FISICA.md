# Personagem: registro, atualização e tempo

Análise estática do executável próprio, SHA256
`1968814b70f69ec85ff3e556c4b66b53ec298c0dea71120e4f0ba594906e418e`.
Endereços relativos à imagem. Nenhuma execução do jogo nesta etapa.

## Extração reproduzível

`tools/inspect_guest_types.py` lê o índice SQLite gerado por
`tools/index_guest_code.py`. Reconhece janelas de constantes que ligam o hash do
nome técnico (`0x302ff0`) ao registro de reflexão (`0x311570`). Recupera endereço
do descritor, tamanho, descritor pai e callback de criação. O callback pode
retornar nulo: não deve ser confundido automaticamente com um construtor real.

Branches, destinos de branches, instruções desconhecidas e escritas parciais
invalidam informações. A ferramenta é conservadora e pode omitir registros.
Não acompanha efeitos de memória arbitrários nem comprova instâncias em cena.
Só emite nomes técnicos ASCII selecionados, nunca diálogos ou bytes do jogo.

```powershell
python tools/inspect_guest_types.py build/research/guest-code.sqlite --elf G:/kyoei-toshi-work/native-05/out/eboot.elf --match '(?i)(player|character|physics|havok|chara|unit.*move|unit.*human|unit.*actor)' --out build/research/movement-types.json
```

Resultado conservador: 624 funções examinadas, 1.039 registros recuperados e
59 tipos selecionados. O relatório privado fica ignorado pelo Git. A primeira
exploração mais permissiva encontrou 62 candidatos; a contagem menor elimina
janelas cujo estado não pode ser sustentado com segurança.

O filtro conservador omite UnitPlayer e UnitPlayerBase: os endereços abaixo
vieram da exploração inicial e foram conferidos separadamente no índice e no
ELF. O relatório final não deve ser usado como lista completa de classes.

## Ligações conferidas

| Tipo | Descritor | Callback de criação | Tamanho | Pai registrado |
|---|---|---|---|---|
| GxUnitCharacterBase | `0x1497a50` | `0x71b610` | 2608 | `0x1356e20` |
| UnitCharacter | `0x149f0c8` | `0x745fd0` | 5392 | `0x1497a50` |
| UnitPlayer | `0x151c7d0` | `0x9fcbf0` | 5664 | `0x149f0c8` |
| UnitPlayerBase | `0x151c930` | `0x9fd960` | 5392 | `0x149f0c8` |

`UnitPlayerBase` retorna zero no callback; seu nome não prova que seja o pai de
`UnitPlayer`. O pai registrado de ambos é `UnitCharacter`.

`0x745fd0` aloca e chama `0x7469a0`, que chama `0x71bf80` e instala
`0x1284630` em self (`0x7469e1`). `0x9fcbf0` aloca e chama `0x9fccf0`,
que chama `0x7469a0` e instala `0x12cba00` (`0x9fcd1e`). O callback base
`0x71b610` instala `0x127e328`. Tabelas conferidas diretamente no ELF:

| Tabela | slot 0x78 | slot 0x80 | slot 0x90 |
|---|---|---|---|
| GxUnitCharacterBase `0x127e328` | `0x719a90` | `0x2aee20` | `0x2af890` |
| UnitCharacter `0x1284630` | `0x719a90` | `0x740190` | `0x7401b0` |
| UnitPlayer `0x12cba00` | `0x9fc1a0` | `0x9fc1f0` | `0x9fc210` |

A fila já mapeada em TEMPO-TAREFAS-ENTIDADES.md usa o código de membro `0x79`
para resolver slot `0x78`. Essa é uma ligação estática condicional: depende do
tipo efetivo da entidade que a cena inserir na fila.

`0x9fc1a0` testa o bit 0 de self+`0x69a`. Quando limpo, salta para a atualização
base `0x719a90`; quando ativo, usa self+`0x1608`, slot `0xd20` e termina pelo
slot `0xc8`. Não presumir que o jogador sempre percorre a atualização base.

Em `0x719a90`, se self+`0x6a0` existe, o código passa self+`0x14` em XMM0
ao slot `0xb8` desse objeto (`0x719aca`/`0x719acf`) e self+`0x18` ao slot
`0x78` (`0x719adc`/`0x719ae4`). São os campos de segundos e unidades de quadro
preenchidos pelo gestor de entidades. Sem esse objeto, há outro caminho
condicionado por self+`0x770`, com slots próprios `0xc0`/`0xc8`.
Depois seguem slots `0x1d8`, `0x1e8`, `0x200`, `0x208`, `0x210` e `0x218`.
Os nomes funcionais desses métodos e o tipo do objeto+`0x6a0` continuam abertos.

## Física: fronteira atual

Foram localizados registros de formas, ray/capsule checks, constraints e formas
dinâmicas. `UnitPhysicsShapeBase` tem callback nulo `0xb68a40`.
`UnitPhysicsDynamicsGround` registra descritor `0x154e288`, pai `0x154dd38`,
tamanho 2032 e criação `0xb6fbd0`. Esse callback instala primeiro uma tabela
base e depois a final `0x12ef290`, portanto a primeira LEA não basta para
identificar a classe. Slot `0x78` final aponta para `0xb6d4f0`, um salto para
`0x35b7f0`; isso ainda não identifica o solver nem o passo de integração.

Próximo: identificar o objeto+`0x6a0`, resolver os métodos específicos de
movimento e localizar o avanço do mundo físico. Ainda faltam medições em cena
para atribuir custo às rotinas. Nenhuma mudança no runtime ou ganho de FPS
decorre automaticamente deste mapa.
