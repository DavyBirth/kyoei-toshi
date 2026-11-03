# Dependencias da candidata publica — 03/11/2026

A candidata contem os avisos encontrados nas fontes efetivamente utilizadas.
Isso nao comprova, por si so, entrega do codigo-fonte correspondente ou compilacao
a partir de uma pasta limpa. O ZIP permanece marcado publication_ready=false.

| Componente | Referencia local verificada | Pendencia de publicacao |
| --- | --- | --- |
| Runtime Godzilla fixo | commit e976b59a3f056c5ea5d0ddfe1c5ef566cce2f45e, manifesto docs/godzilla-core.json | Entregar fontes e instrucoes de compilacao; o Git hoje so leva scripts/LICENSE do snapshot. |
| GoW3 GPU | commit 1436e7e79807de8e832fc1b6da2f8f20a4293172, docs/gow3-gpu.json | Entregar snapshot correspondente e adaptacoes gpu-gow3. |
| Bibliotecas estaticas do runtime | third_party/godzilla-core/prebuilt/libraries.cmake | Resolver reproducao das bibliotecas; headers/cache local nao substituem fontes completas. |
| FFmpeg | pacote ext-ffmpeg-core, prebuilt identificado como ffmpeg-94dde08 | Receita identificada: FFmpeg 7.1.1, vcpkg df8bfe519564ae001903e5cdd32af0999531ef71 e ffmpeg.patch do ext-ffmpeg-core. Fonte upstream baixada e verificada pelo SHA512 do port; snapshot vcpkg completo tambem obtido. Resta montar e testar o kit de fontes completo. |
| FDK AAC | gz_aac-src/NOTICE | Aviso agora incluido; falta associar fontes/receita ao binario utilizado. |
| FSR-Vulkan | LICENSE.txt, NOTICE.md, UPSTREAM.md e licenca SDK 1.1.4 | Avisos agora incluidos; entregar o fonte utilizado e seus subconjuntos upstream. |
| ImGui | third_party/imgui/LICENSE.txt | Aviso incluido; entregar a revisao efetiva com o fonte do runtime. |
| Python, Pillow e NumPy | interpreter LICENSE.txt e metadados dist-info | Incluidos no pacote, isolados de PYTHONHOME/site de usuario; smoke test passou sem Python externo. |
| Noto Sans e Noto Sans CJK | OFL na pasta de licencas | Fontes redistribuidas; nenhum atlas do jogo entra no ZIP. |

O preparador precisa apenas do dump CUSA03562 01.03 e dos modulos do PS4 do
usuario. Ele gera localmente todos os patches, imagens e a imagem preparada.
Os 19 StageModule sao obrigatorios; nao distribuir nenhum desses derivados.

Compilacao limpa ainda pendente: CMakeLists.txt e gpu-gow3/CMakeLists.txt usam
cache de headers e bibliotecas do projeto irmao. O Godzilla nao foi modificado.
O proximo passo de distribuicao de fontes deve substituir esse cache por
entradas explicitas e versionadas e comprovar o build fora desta maquina.

Referencia da receita fixada: [portfile vcpkg](https://github.com/microsoft/vcpkg/blob/df8bfe519564ae001903e5cdd32af0999531ef71/ports/ffmpeg/portfile.cmake), [versao](https://github.com/microsoft/vcpkg/blob/df8bfe519564ae001903e5cdd32af0999531ef71/ports/ffmpeg/vcpkg.json). SHA512 esperado do tarball upstream: `6b9a5ee501be41d6abc7579a106263b31f787321cbc45dedee97abf992bf8236cdb2394571dd256a74154f4a20018d429ae7e7f0409611ddc4d6f529d924d175`. Nao usar download parcial ou sem verificar o hash.

Fontes obtidas em 03/11: FFmpeg-n7.1.1 tar.gz SHA256 f117507dc501f2a6c11f9241d8d0c3213846cfad91764361af37befd6b6c523d; vcpkg df8bfe5 tar.gz SHA256 2960bcde9fa2f319fc8927668375ab905fdd17fc98799a482ad61583f6679345. Os padroes de headers do CMake agora apontam para build/_deps local; caches de compilacao ja existentes mantem seus caminhos configurados.
