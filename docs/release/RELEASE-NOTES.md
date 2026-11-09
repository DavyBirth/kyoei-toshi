# City Shrouded in Shadow — 1.0

## Português

Port experimental para Windows de Kyoei Toshi, com japonês, inglês e português do Brasil.
Inclui teclado e mouse, controles PlayStation/Xbox/Nintendo, modo foto e opções gráficas.
Os textos pt-BR e ingleses receberam revisão editorial; o relatório inglês está no kit Source.

Baixe **Kyoei-1.0-Windows.zip**, extraia tudo e abra **City Shrouded in Shadow.exe**.
Escolha **Preparar jogo** e forneça o dump do seu próprio disco **CUSA03562 01.03**
e os módulos do seu próprio PS4. O pacote não contém nem baixa o jogo ou firmware.
Preserve a pasta **data/** ao atualizar: ela guarda saves e configurações.

Requisitos: Windows 10/11 x64, CPU AVX2, GPU/driver Vulkan e espaço para a cópia do dump
e os arquivos temporários, em NTFS ou exFAT. Linux via Wine/umu exige testes próprios.

Limitações conhecidas: desempenho variável; campanha completa ainda não testada do início
ao fim no PC; nomes no minimapa podem ser cortados na borda; manchas brancas passageiras
podem aparecer em fundos desfocados. A revisão editorial não equivale a conferir todas
as cenas em jogo. Essas limitações não bloqueiam o escopo 1.0 definido pelo projeto.

O ZIP Source inclui os snapshots fixos e fontes das dependências. O runtime foi compilado
a partir de uma extração nova; as bibliotecas precompiladas de referência não foram todas
recompiladas. Não se afirma reprodução bit a bit. Consulte SOURCE-BUILD.md e os manifestos.

## English

Experimental Windows PC port of Kyoei Toshi, with Japanese, English and Brazilian Portuguese.
Includes keyboard/mouse, PlayStation/Xbox/Nintendo controllers, photo mode and graphics options.
English and Portuguese received an editorial review; full-campaign gameplay testing is incomplete.

Download **Kyoei-1.0-Windows.zip**, extract everything and open **City Shrouded in Shadow.exe**.
Select **Prepare game** and supply your own **CUSA03562 v01.03** disc dump and your own
PS4 system modules. No game or firmware is included or downloaded. Preserve **data/**
when updating: it contains your saves and settings.

Requires Windows 10/11 x64, AVX2 CPU, Vulkan GPU/driver and enough NTFS/exFAT storage
for a copy of the dump and temporary files. Linux through Wine/umu needs separate testing.

Known issues: variable performance, full campaign not tested start to finish, minimap names
may be clipped at the edge, and transient white spots may appear in blurred backgrounds.
Editorial review does not mean every translated scene has been checked in game.

**Kyoei-1.0-Source.zip** contains the fixed snapshots and dependency sources. Its runtime
target passed a build from a fresh extraction; not every reference static library was rebuilt.
No bit-for-bit reproducibility claim is made. See SOURCE-BUILD.md and the bundled manifests.

Each ZIP has a SHA256 file. GitHub's automatic source archives omit the unversioned
snapshots; use the attached **Kyoei-1.0-Source.zip** to build.
