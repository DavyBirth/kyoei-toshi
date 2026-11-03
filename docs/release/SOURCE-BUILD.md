# Compilar o kit de fontes no Windows

O ZIP de fontes e separado do aplicativo e nao contem o jogo ou firmware.
Ele inclui os snapshots fixos do core/GPU, fontes das dependencias, headers
gerados e bibliotecas estaticas de referencia, com hashes no manifesto.

Requisitos externos: Windows x64, LLVM/Clang, CMake 3.28+, Ninja, Windows SDK /
ferramentas C++ da Microsoft, Vulkan SDK e Python 3.11+.
Extraia todo o kit para uma pasta nova. Nao reaproveite CMakeCache.txt de outra
pasta. A arvore build/_deps e build/fsr-vulkan-src contem fontes, nao saves.

```
cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build --target kyoei -j 4
```

O executavel sai em build/kyoei.exe. A compilacao nao executa o jogo.
A configuracao padrao usa exclusivamente as dependencias incluidas no kit,
alem do compilador e dos SDKs instalados. Os parametros KYOEI_GPU_HEADERS e
DEPS podem apontar explicitamente para outro cache, se necessario.

Para gerar o aplicativo: instale tools/requirements-images.txt no Python usado
pelo empacotador e execute:

```
python tools/build_public_candidate.py build/App-Candidate --dependency-cache build/_deps
```

Os arquivos upstream-source/ffmpeg-n7.1.1.tar.gz e vcpkg-df8bfe5.tar.gz conservam
as fontes upstream e a receita fixada. O cache build/_deps/gz_ffmpeg-src contem
o workflow e ffmpeg.patch usados pelo fornecedor do prebuilt. Os outros
build/_deps/*-src conservam as fontes das bibliotecas estaticas. A substituicao
dessas bibliotecas e sua reproducao tambem precisam ser conferidas antes da
publicacao; incluir os fontes nao comprova que todos os prebuilts foram refeitos.

Validacao atual: configuracao em pasta nova passou apos incluir libraries.cmake
e o auxiliar benchmark_gpu_wait.c. A compilacao completa esta em andamento;
nao anunciar aprovacao antes do resultado final. O kit continua uma candidata.
