# Variante PSARC encontrada no Kyoei

Leitura independente implementada em `tools/psarc.py`, sem bibliotecas externas.
A ferramenta abre os arquivos somente para leitura. Suporta a variante medida
nos 25 PSARC deste dump; rejeita outras variantes em vez de supor compatibilidade.

## Estrutura observada

Todos os números do cabeçalho e da TOC usam ordem big-endian.

| Offset | Bytes | Campo | Valor observado |
|---|---:|---|---|
| 0x00 | 4 | Assinatura | PSAR |
| 0x04 | 4 | Versão | 0x00010004 |
| 0x08 | 4 | Compressão | zlib |
| 0x0C | 4 | Fim da TOC / início dos dados | varia |
| 0x10 | 4 | Tamanho de entrada | 30 |
| 0x14 | 4 | Número de entradas, incluindo manifesto | varia |
| 0x18 | 4 | Tamanho máximo de bloco | 65.536 |
| 0x1C | 4 | Flags | 0x0000000D |

A TOC começa em 0x20. Cada entrada contém 16 bytes de MD5 do caminho,
4 bytes de índice inicial na tabela de blocos, 5 bytes de tamanho descompactado
e 5 bytes de offset absoluto no PSARC. Os campos de 40 bits são necessários:
`common.psarc` ultrapassa 4 GiB.

Depois das entradas, até o fim da TOC, há comprimentos de bloco de 2 bytes.
Zero representa um bloco bruto de 65.536 bytes. Um comprimento igual ao
tamanho esperado do bloco representa dados brutos; comprimentos diferentes
são decodificados com zlib. O leitor verifica o tamanho de cada resultado
e o término do stream comprimido, sem aceitar silenciosamente falhas de zlib.

A primeira entrada, com MD5 zerado, contém o manifesto de caminhos.
Neste dump, os nomes são UTF-8 separados por byte nulo, com contagem
exatamente igual às demais entradas. O leitor também aceita nomes separados
por linhas para fixtures, mas esse modo não foi observado neste dump.

**A ordem do manifesto não corresponde à ordem das entradas.** A ligação
correta, conferida em todos os arquivos, é MD5 dos bytes do caminho em
maiúsculas ASCII. O caminho original mantém sua grafia na extração.
A TOC está em claro. Não foi necessário descriptografar conteúdo.
A interpretação completa dos bits de flags ainda não foi comprovada.

## Referências técnicas

Consultadas para conferir a estrutura geral, sem copiar seu código:

- [Leitor PSARC em rs-utils](https://github.com/0x0L/rs-utils/blob/master/bin/psarc.py):
  cabeçalho, entradas e blocos zlib; sua variante de Rocksmith usa TOC cifrada.
- [Leitor do TLOU PSARC Tool](https://github.com/amrshaheen61/TLOU_PSARC_Tool/blob/master/TLOU%20PSARC%20Tool/Core/Psarc.cs):
  campos de 40 bits e ligação dos nomes por hash.

As diferenças do Kyoei (flags, separador nulo e hash em maiúsculas) foram
estabelecidas pela leitura do dump local. As referências não provam que
todos os jogos com PSARC usam a mesma variante.

## Verificação

`python tools/test_psarc.py` usa somente fixtures sintéticas. Cobre nomes
em ordem diferente, mistura de blocos brutos e zlib, arquivo vazio,
caminhos perigosos no Windows, colisão de nomes, truncamento, hash de nome
incorreto, offsets de 40 bits, índices de blocos inválidos e zlib corrompido.

A extração calcula SHA-256 do resultado decodificado e relê as cópias para
comparar tamanho e hash. Isso comprova a escrita dos resultados e os limites
do leitor, mas não substitui a prova de reinserção e execução no shadPS4 do M2.
