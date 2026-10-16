# Luminância e a leitura de 1×1 pixel

Executável próprio CUSA03562 01.03, SHA256
`1968814b70f69ec85ff3e556c4b66b53ec298c0dea71120e4f0ba594906e418e`.
Endereços relativos à imagem. Análise estática, sem abrir o jogo.

## O que o teste comprovou

O registro `20261015_234620.log` correlaciona duas leituras de imagem 1×1,
256 bytes com pitch/alinhamento, a esperas de cerca de 91 e 133 ms no scheduler.
Os ticks exigidos já estavam submetidos, sem flush adicional no Wait.
Isso não identifica qual sistema do jogo produziu a imagem nem prova que a
transferência de 256 bytes seja o trabalho responsável por toda a espera.

## Candidato localizado no executável

Há um sistema de luminância/exposição automática. Rótulos técnicos embutidos
e referências RIP decodificadas permitem ligar:

| Rótulo técnico | Endereço do rótulo | Função que o referencia |
|---|---|---|
| CTextureLuminance | `0xd135f0` | `0xcd9f0` |
| CTextureLuminanceManage | `0xd16390` | `0xd0fd0` |
| GetResult_UpdateInternalLuminance | `0xd1516b` | `0xd08e0` |
| ApplyEffects_UpdateAutoExposure | `0xcf2db7` | `0x67430` |
| InitializeAutoExposure | `0xce9eed` | `0x4e4e0` |

São rótulos usados em diagnóstico/asserts; ajudam a reconhecer subsistemas,
mas não substituem a verificação dos receptores e tipos efetivos.

`0x67430` chama o gestor em `0xd1ec0` pelo ponto `0x678a9`, usando um objeto
obtido de outro objeto+`0x60`. O inicializador `0x4e4e0` chama a configuração
`0xd2ad0` em `0x4e681`. Outros chamadores de `0xd1ec0` são `0x69be0` e
`0x6c280`; não assumir que só existe um uso por quadro.

No caminho habilitado pelo bit0 de gestor+`0x54`:

1. `0xd2015` chama `0xd04d0` sobre gestor+`0x40`, com ESI=0.
2. Um retorno não zero salta para `0xd261a`, antes das leituras abaixo.
3. Com retorno zero, seleciona um índice e guarda o contador gestor+`0x58`
   numa entrada gestor+`0x74 + 4*índice`.
4. `0xd204a` chama `0xd0e50`, novamente ESI=0; guarda o float retornado em
   gestor+`0x1c8 + 4*índice`.
5. `0xd2086` chama `0xd0e90`, ESI=0, e copia o resultado para uma entrada
   de 16 bytes em gestor+`0xb8 + 16*índice`.

`0xd0e50` passa o booleano a `0xd08e0` e retorna o float de objeto+`0x74`.
`0xd0e90` também atualiza por `0xd08e0` e retorna o endereço objeto+`0x64`.
Portanto o gestor contém histórico de resultados, em vez de simplesmente
calcular um float do zero e devolvê-lo em toda chamada.

## Consultas: o que já sabemos e o que falta

`0x92070` lê o estado de objeto+`0x60`. `0xd04d0` tem caminhos para os valores
2, 3 e 4. Nos caminhos de 3/4 percorre objetos de consulta em
objeto+`0x40 + 8*i`, limitados pela contagem+`0x3c`, e chama slot `0x58`
passando o booleano recebido. Um retorno não zero interrompe a sequência.

`0xd08e0`, em outro ponto, usa slot `0x60` dos objetos de consulta para obter
resultados, também passando o booleano. A rotina tem múltiplos caminhos por
estado e laços de consulta; não reduzir seu comportamento a uma leitura simples.
`0xcdb80` zera quatro ponteiros a partir de objeto+`0x40`.

`0xcf400` consulta um backend por objeto+`0x20`. No trecho de `0xcf641`,
o método vem do slot `0x70` do backend, com uma superfície/recurso obtida de
objeto+`0x90`. O estado 2 seleciona a flag `0x100000`; outros estados
selecionam zero nesse caminho. Ainda faltam a identificação da implementação
concreta desse slot, o contrato dessa flag e a origem exata da superfície.
Não chamar ESI=0 automaticamente de “não bloqueante” antes dessa resolução.

## Consequência prática

Exposição automática é um candidato plausível para feedback de poucos pixels.
**Não foi demonstrado que a imagem 1×1 observada seja desse sistema.** Dimensões
iguais não estabelecem identidade. Não desativar exposição, reaproveitar um
resultado antigo nem adiar arbitrariamente a publicação: isso pode modificar
a iluminação ou entregar um dado que o consumidor espera estar atualizado.

Próxima ligação necessária: identificar o recurso de runtime por endereço e
formato e confrontar com o objeto/superfície usado no backend. Só depois
avaliar uma publicação assíncrona respeitando os testes de disponibilidade
existentes no jogo. Nenhuma alteração de gameplay foi feita nesta etapa.

A coleta local agora pode registrar endereço guest, formato Vulkan numérico,
pitch e papel de imagens 1×1. Papéis: 1=storage binding, 2=lookup de render target,
3=download. Limite total de 16 combinações únicas endereço/formato/papel por
thread; repetições não geram linhas. Só fica ativa em PerformanceStats. As
esperas longas de readback incluem o endereço para cruzar com esses registros.
Endereços podem ser reutilizados; lookup/binding não prova execução de um shader
nem liga automaticamente a imagem ao sistema de luminância acima.

## Reproduzir o mapa

```powershell
python tools/inspect_frame_flow.py G:/kyoei-toshi-work/native-05/out --facts docs/re/luminance-flow-facts.json --depth 1 --max-functions 250 --out build/research/luminance-flow.json
```

O relatório privado contém o mapa técnico derivado, sem ser distribuído junto
dos updates. O índice SQLite local permite conferir as instruções citadas.
