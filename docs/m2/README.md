# M2 — extração e reinserção de textos

## Prova realizada

O dump próprio CUSA03562, APP_VER 01.03, é a base registrada no M0.
Foram analisados 2.331 recursos extraídos no M1: 2.032 tabelas CSH e
299 scripts LUC. Todos passaram pela extração, serialização em JSON e
reinserção sem alterações, com igualdade byte a byte e zero falhas.

Há 235.069 ocorrências de strings, das quais 17.866 contêm caracteres
japoneses. São candidatos, não uma contagem de falas únicas: identificadores,
caminhos e listas de palavras também aparecem nesses arquivos. A cobertura
de todos os textos visíveis, inclusive imagens e vídeos, ainda não foi provada.
O relatório de metadados está em [audit.json](audit.json).

Uma cópia completa de teste foi construída em
`G:\kyoei-toshi-work\m2\prova-01\CUSA03562`. Seus 87 arquivos,
17.798.062.971 bytes, têm os mesmos SHA-256 do M0 após reinserir os
2.331 recursos. [dump-proof.json](dump-proof.json) registra a comparação.
Não há dados do jogo nem textos originais completos no repositório.

**M2 ainda não está concluído.** A cópia abriu no shadPS4 0.16.0 e chegou
à cena inicial com cenário renderizado e narração japonesa legível.
A captura local foi inspecionada; os hashes e o escopo da observação estão
em [runtime-proof.json](runtime-proof.json). A primeira tentativa ficou num
diálogo antes de gerar log; a segunda executou o jogo. O log apresentou
erros `sceKernelStat` (arquivo não encontrado), que apareceram também na
comparação posterior com o dump original. A cópia encerrou mais tarde
com `Out of order flip IRQ`; FIFO não resolveu. O original foi fechado
normalmente após cerca de seis minutos, sem reprodução observada dessa
falha gráfica. Não foi realizada regressão do jogo inteiro. O foco atual
é o [port nativo com a base do Godzilla](../PORT-NATIVO.md).

O [mapa local de textos](text-map.json) registra 13.712 strings japonesas
UTF-8 únicas em 140 recursos. As categorias são indicações por nome de
arquivo, ainda não confirmação de cada tela. Há 4.447 strings não UTF-8
preservadas, principalmente configurações/flags; a codificação ainda não
foi certificada. O texto completo do índice fica fora do Git.

## Uso

Executar na raiz do projeto. Os caminhos abaixo são exemplos locais;
cada saída deve ter um nome novo e ficar fora do repositório, do dump
original e da extração M1.

```powershell
python tools/text_tool.py extract CAMINHO_DO_RECURSO SAIDA.json
python tools/text_tool.py insert CAMINHO_DO_RECURSO SAIDA.json RECURSO_NOVO
python tools/psarc_patch.py ORIGINAL.psarc NOVO.psarc --replace caminho/interno=RECURSO_NOVO
```

O JSON contém `id`, `original`, `translation` e `japanese_candidate`.
Manter `original` intacto; `translation: null` conserva o texto original.
A ferramenta verifica o SHA-256 da base, IDs completos e únicos e o tipo
do recurso. Strings não UTF-8 são preservadas sem exposição no JSON.
Traduções não podem conter NUL. Alterações de tamanho e acentos foram
testados com dados sintéticos; isso ainda não comprova a fonte do jogo (M3).

```powershell
python tools/test_text.py
python tools/test_psarc_patch.py
python tools/audit_m2.py
python tools/roundtrip_dump.py DIRETORIO_NOVO_DA_COPIA
```

Os seis testes de texto e quatro testes de pacotes passaram. A auditoria
usa o catálogo e hashes locais do M1. A cópia completa requer espaço para
aproximadamente 17,8 GB e não sobrescreve um diretório existente.

## Formatos e preservação

### Envelope BILZ

LUC começa com `00 63 75 6C`; CSH com `00 68 73 63`. O cabeçalho próprio
tem 128 bytes, com versão 2 em big endian no offset 4. No offset 128 há
`BILZ`, tamanho descomprimido, tamanho comprimido e flags (quatro campos
de 4 bytes, big endian). As flags observadas são 7. O zlib começa no
offset 144. A leitura exige fim correto do fluxo e limita a saída a 64 MiB.
O cabeçalho é preservado. Sem mudanças, o envelope original é devolvido
inteiro: recomprimir zlib poderia alterar bytes mesmo com conteúdo igual.

### LUC / Lua

O conteúdo é bytecode Lua 5.2 com inteiros e instruções de 4 bytes,
`size_t` de 8 bytes e números float de 4 bytes, little endian. O parser
segue a estrutura de protótipos, constantes e depuração; não executa scripts.
IDs como `p0/p0/k3` identificam constantes de string. Somente essas
constantes e seus comprimentos são alterados. Código, números, fontes de
depuração, nomes locais e upvalues permanecem intactos. A estrutura foi
comparada com os fontes oficiais [lundump.c](https://www.lua.org/source/5.2/lundump.c.html)
e [ldump.c](https://www.lua.org/source/5.2/ldump.c.html).

### CSH

O payload começa com flags, número de colunas e número de linhas
(três u32 big endian). Cada célula tem offset e tag, ambos u32.
A pool começa após a matriz, em `12 + linhas * colunas * 8`.
Os três bits superiores da tag são o tipo; os 29 inferiores contam
palavras de 4 bytes. O offset é absoluto dentro do payload descomprimido,
aponta para a pool e é alinhado a 4 bytes.

| Tipo | Observação |
| --- | --- |
| 0 | String UTF-8 terminada em NUL e padding zero |
| 1 | Dados opacos de 8 bytes; significado ainda desconhecido |
| 2, 4 | Valor de uma palavra, preservado |
| 3, 5, 6 | Sentinela com offset e comprimento zero |
| 7 | Não observado; rejeitado pelo parser |

IDs `rN/cN` identificam as células. Strings alteradas são anexadas à pool
e apenas o ponteiro da respectiva célula é atualizado. Isso preserva aliases
e os dados das outras células. Sem alterações, o resultado é idêntico.

### Atualização PSARC

Sem recursos alterados, o pacote é copiado byte a byte. Com alterações,
o corpo original permanece, novos blocos são anexados e a TOC é ajustada.
Entradas e blocos antigos podem continuar presentes sem uso. Os demais
recursos não são recomprimidos. A ferramenta relê o arquivo produzido,
verifica os recursos substituídos e compara o hash da fonte com o M0.
A saída é uma cópia de teste contendo dados do jogo, destinada somente
ao uso local; ela nunca deve ser distribuída como patch.

## Próximas provas

Mapear quais strings aparecem em diálogos, menus, itens e legendas.
Investigar os formatos restantes antes
de declarar que todos os textos foram encontrados. Tradução, fontes e port
continuam sendo marcos posteriores.
