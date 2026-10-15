# Sessão noturna — 15/10/2026

A cópia privada está em `build/City-Shrouded-in-Shadow-Portable`.
Copiar a pasta inteira para NTFS/exFAT e abrir `City Shrouded in Shadow.exe`.
O launcher oferece somente inglês e japonês, resolução e tela cheia.
Resultado esperado: launcher com arte do jogo, perfil novo e controles em README.txt.
Windows 10/11 x64, CPU AVX2 e driver Vulkan compatível são necessários.

## Tradução e imagens

Preservado o avanço recebido: 13.084 EN privadas. Acrescentadas 12 EN:
13.096/16.100 preenchidas, 3.004 vazias. No runtime são 13.226 incluindo
130 entradas piloto; não confundir as duas bases. Prévia en-12 com 59 texturas,
incluindo avisos de abertura e opções revisadas. SHA256 common.psarc:
`6b8018b985f9261b88d84e086222676cd16b39cc6f5f8283816a0b066c9b24dd`.

Auditoria estrutural dos 173 JSONs e reinserção/releitura de 117 recursos PASS.
Triagem das vazias: 2.121 cadastros provisórios, 256 modelos de notícia,
320 créditos, 18 testes numerados, 7 notas explícitas de desenvolvimento,
44 candidatos provisórios/indisponíveis, 3 rótulos técnicos e 235 de revisão
individual. Isso não prova inacessibilidade nem autoriza declarar 100%.
Nomes e leituras incertas exigem evidência; revisão humana continua necessária.

## Portátil e privacidade

125 arquivos no manifesto: 24.221.637.327 bytes, cerca de 24,2 GB / 22,6 GiB.
87 arquivos originais conferidos com SHA256 do inventário do disco; dump intacto.
Inglês adicional conferido com a prévia. Sem saves, logs, conta, configurações
pessoais ou históricos. Usuário padrão Player; dependências/fontes locais.
Metadados de depuração e caminhos de fonte removidos do PE, preservando código.
Varredura de identificadores textuais ASCII/UTF16 PASS. Coincidências aleatórias
em PSARC comprimidos não alteram arquivos idênticos ao disco original.
Isso descreve a limpeza do conteúdo; o Windows de destino cria seus próprios
registros quando o programa é executado. Cópia pessoal, não publicar dados do jogo.

Manifesto SHA256:
`839cadeb1d44c42024f73291d93eb844624b11d3af352afbc478e0d30f4031c7`.
Launcher --check e loader --list-stubs --no-gpu PASS também em pasta com espaços
e acentos, sem partida. Nenhum jogo foi iniciado/controlado nesta sessão.
Prévia en-12 e pacote preservados; en-08/09/10/11 obsoletas removidas após
conferir substitutos e ausência de processos consumidores. Evidências mantidas.

## Desempenho e pendências

Build nativo e testes de cache/estatísticas/layout/privacidade PASS. Cache Vulkan
validado por GPU, gravação atômica e proteção contra perfil truncado; preparo
antes do WarmUp e salvamento na saída normal. Diagnósticos limitados registram
pausas de compilação. Identificação do núcleo corrigida para CUSA03562/01.03.
Snapshot e projeto irmão não modificados. Ver DESEMPENHO.md.

60 FPS não demonstrados; log anterior misto registra 31,66 FPS agregados.
Não alterar tempo da simulação nem descartar sincronização sem prova.
Quebra por palavras, pontuação isolada, escolhas longas e questão visual dos
orbes continuam pendentes. Campos/rotinas de fonte mapeados em
m3/HUD-INVESTIGACAO.md; nenhum hook de quebra ativado sem contrato comprovado.
Logo original e textos embutidos em vídeos continuam pendentes.

O teste de abrir/fechar uma partida foi rejeitado pela revisão automática,
que citou restrição anterior de não iniciar/reiniciar/controlar o jogo.
Autorização específica solicitada e ainda sem resposta; bloqueio não contornado.
Próxima etapa depende de teste em tela e comparação de percurso/cache frio/quente.

Monitor de cota a cada 600 segundos, somente métricas locais. Na conclusão,
salvar consolidado/commits antes de solicitar o desligamento autorizado por Davy.
O registro de uma solicitação de desligamento não prova o desligamento físico.
