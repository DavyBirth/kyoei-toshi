# Referencia local: Uncharted 0.2.3

Arquivo adicionado por Davy em `third_party`, analisado em18/10/2026:
`Uncharted-The-Nathan-Drake-Collection-PC-port-0.2.3.tar.gz`.
SHA256: `6ba36265422b656f410b69b4968f5b8c2ce9321dcb57c3a1e77c827d8d867a69`.
Autor identificado no CHANGELOG: BilalReloaded. Analise dos fontes locais;
nao executamos o pacote Windows nem importamos codigo para a copia Godzilla.

O caminho `gow3_pc/gpu/shadps4` usa uma integracao propria do renderizador.
Os ganhos publicados no CHANGELOG sao relatos desse projeto, nao resultados
do Kyoei. Patches de enderecos, correcoes de NaN, modo de espera das jobs e
mudancas de resolucao/AA nao sao transferiveis por semelhanca entre ports.

Ponto concreto: `video_core/renderer_vulkan/vk_rasterizer.cpp`, funcao
`ResetBindings`, linhas3417..3455. Intervalos adjacentes/sobrepostos de um
mesmo buffer e mesmo acesso sao ordenados e unidos antes de registrar o
acesso. A aplicacao e restrita a compute: o comentario relata custo de3..4%
ao ordenar tambem draws de Uncharted1/2. Nao elimina barreiras nem mistura
leitura com escrita. O ganho depende da carga: nossos batches observados
tinham cerca de1.383draw e17..18dispatch, muito diferentes dos cerca de1.500
dispatch/frame citados no comentario sobre a multidao de Uncharted3.
Essa diferenca desaconselha promover a fusao aqui sem medir seu custo.

Outro ponto: `buffer_cache/buffer_cache.cpp`, `SmallGuestCopy`, delega copias
ao trabalhador de gravacao com `scheduler.RecordHostCopy` quando a gravacao
e diferida, ou ao pool de copias. Tambem ha arenas, publicacao assincrona e
tratamento de paginas GPU/CPU integrados ao runtime. Transplantar o trecho
isolado nao prova validade da ordem, da propriedade dos dados ou do readback.

Decisao desta rodada: primeiro medir ciclos da thread por batch, mantendo
resolucao, efeitos, simulacao e sincronizacao. Nao repetir os tunings rejeitados
de sinalizacao, lote192 e cache512 sem evidencia nova. Nenhum ganho de FPS
atribuido a esta analise ou ao contador de ciclos.
