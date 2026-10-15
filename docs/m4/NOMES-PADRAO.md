# Nomes padrão — inicialização e limites

## Adaptação de apresentação em 15/10/2026

Implementado `src/kyoei_names.c` para inglês: ligação de `wcstombs_s` no GOT
0x12f7ab8, conferida contra a importação libc/índice28, endereço nativo e
assinaturas do thunk/carga de campo. Antes da proteção final do carregador,
substitui apenas essa ligação por um adapter que delega à libc original.
Não escreve nos campos do protagonista nem amplia a estrutura do save.

Fontes para a apresentação Ken Misaki/Miharu Matsubara: nomes japoneses no
[PlayStation Blog](https://blog.ja.playstation.com/2017/10/16/20171016-kyoeitoshi/)
e leituras explícitas dos sobrenomes na
[apresentação publicada pela GAME Watch](https://game.watch.impress.co.jp/docs/news/1064842.html).
As formas em letras latinas são transliteração editorial dessas leituras,
sem alegação de grafia latina oficial publicada pelo fabricante.

Escopo: fontes que apontam exatamente para os dois campos verificados e
concatenação sobrenome/espaço/nome no retorno0xac4e64. Campos são comparados
com as quatro formas japonesas conhecidas; qualquer outro texto segue intacto.
Nessa apresentação inglesa, um nome personalizado escrito exatamente igual
a uma dessas formas japonesas também será romanizado. A identidade e os bytes
armazenados não mudam; não há substituição global em diálogos ou outros NPCs.

No editor ImGui, os defaults completos aparecem em romaji. Aceitar um default
devolve os caracteres originais ao convidado; um nome personalizado é devolvido
literalmente se couber no limite original. Nomes personalizados maiores que
quatro unidades continuam sem suporte: confirmação bloqueada com explicação,
sem corte silencioso. Alias e limite original são separados no modelo.

Testes de modelo e ABI real: quatro defaults completos, volta aos bytes originais,
nome personalizado, rejeição de comprimento inválido, guards, fonte fora do
campo preservada, chamada de concatenação delimitada e delegação da ABI PASS.
Ainda falta verificar visualmente todas as telas e caminhos em gameplay;
não declarar cobertura de consumidores que não passam por essa conversão.

## Evidência original de 13/10

Investigação estática do dump próprio em 13/10/2026; nenhuma alteração dos
nomes, do executável convidado ou dos saves aplicada nesta etapa.

A apresentação oficial confirma os nomes japoneses dos protagonistas e que
podem ser personalizados: [PlayStation Blog, 25/02/2016](https://blog.ja.playstation.com/2016/02/25/20160225-kyoeitoshi/)
e [PlayStation Blog, 16/10/2017](https://blog.ja.playstation.com/2017/10/16/20171016-kyoeitoshi/).
Essas páginas não demonstram sozinhas a leitura de todo sobrenome em romaji.
Ken e Miharu vêm de katakana; grafias candidatas dos sobrenomes continuam
dependendo de confirmação da leitura. Não chamar uma inferência de grafia oficial.

No ELF preparado (hash em docs/m3/elf-hud-references.json), foram localizadas
as quatro constantes UTF-16 iniciais, separadas por campo:

| Campo | Endereço relativo ao eboot |
| --- | --- |
| Primeiro nome masculino | 0x10d5ab0 |
| Sobrenome masculino | 0x10d5ab6 |
| Primeiro nome feminino | 0x10d5ac8 |
| Sobrenome feminino | 0x10d5ad0 |

Consumidores importantes: LEAs em 0x619730/0x619748 (feminino),
0x619751/0x619769 (masculino), 0x6c8be6/0x6c8c08 (masculino) e
0x6c8c92/0x6c8cb4 (feminino). Os caminhos copiam para destinos relativos
0x13bda8c e 0x13bda96, separados por dez bytes: cinco unidades UTF-16.
Outros consumidores foram observados em 0x5ca8xx, 0x5caexx, 0x5cb4xx,
0x6198xx e 0x67c8xx; analisar antes de alterar globalmente a constante.

As cópias demonstradas chamam o thunk 0xcce360 com EDX=5. Esse thunk passa
pelo GOT 0x12f7ac0 e índice de importação 29, identificado como `wcsncpy`
na tabela preparada. Não presumir que aumentar apenas esse argumento amplia
o destino: atingiria o campo seguinte.

O log privado native-05/logs/20261012_230727.log registra duas entradas IME
com max_length=4. A convergência entre código e chamada real confirma a
restrição atual de quatro caracteres mais terminador por campo de nome.
Miharu, por exemplo, tem seis letras e não cabe. Não abreviar a identidade,
truncar automaticamente ou aumentar o buffer sem mapear seus consumidores.

Próximo passo: separar representação interna e exibição dos defaults ou
mapear extensão compatível dos campos, filtros, substituições e formato de
save. Uma solução de exibição deve distinguir defaults de nomes personalizados,
inclusive um nome personalizado igual a um default. Substituição global por
igualdade de texto não oferece essa garantia. O modelo de edição novo respeita
o limite recebido pelo convidado e não aumenta os campos reais.
