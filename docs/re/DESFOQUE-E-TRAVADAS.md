# Desfoque e travadas — 17/10/2026

Exigência de Davy: fidelidade ao PS4 em efeitos, resolução interna, velocidade
e comportamento. Não retirar um efeito para apresentar isso como correção do
renderer ou de desempenho. A comparação com a referência ainda está pendente.

## Reprodução observada

O relato foi esclarecido como personagem borrado e sem detalhe durante sprint.
Percurso automatizado com teclado, a partir de uma cópia privada do save:
parado, caminhada, sprint e parada. Testes mudos, controle físico ignorado no
perfil de teste. Capturas `sprint-sprint-20261017-{0,1,2,3}.png` em
`build/research/detiler-fix-20261016/`.

Na captura durante sprint, cenário e personagem estão fortemente desfocados,
mas HUD/minimapa permanecem nítidos. A vida passa de 100 para 90 no percurso;
ao parar, imagem nítida novamente, já em outra situação da fase. Isso é
compatível com pós-processamento/dano/transição, mas não prova qual evento o
acionou nem que esteja igual ao PS4. Não atribuir automaticamente a streaming,
texturas ausentes ou queda de resolução. Capturas posteriores repetiram o
percurso; não são comparações controladas de FPS.

O ELF próprio contém referências técnicas a ApplyEffects_DepthOfField,
ApplyEffects_GaussianBlur e ApplyEffects_MotionBlurCamera. As referências foram
localizadas e os getters examinados por LLVM a partir dos limites de unwind.
Isso identifica controles de pós-processamento, sem provar execução do efeito
na captura. Nenhum bypass desses efeitos foi mantido.

## Investigação das pausas

`KYOEI_STREAMING_TRACE=1` habilita observações limitadas no host: upload e
busca de imagens, obtenção de buffer de imagem, coleta de texturas, desenhos,
callbacks e pacotes GFX. Desligado por padrão; não altera registros do jogo,
resolução, recursos, comandos, sincronização ou resultados de readback.

Escopos com pelo menos 25 ms geram até oito registros por identidade entre
32 identidades por thread. Pacotes usam o opcode como identidade e no campo
`address`; demais etapas usam endereço de imagem quando disponível. Medições
são tempos de parede inclusivos. Pacotes podem atravessar yields, portanto
não são tempo CPU ativo; esperas de vídeo não são automaticamente defeitos.
Não somar escopos aninhados. O diagnóstico adiciona custo: não usar seus
tempos para alegar ganho comparativo de FPS.

O trace também registra até 64 combinações de tamanho de alvo/viewport/depth
por thread. Tamanhos menores podem ser passes intermediários normais; 32768
é o limite inicial do estado quando não há attachment correspondente, não
uma prova de framebuffer/imagem alocada desse tamanho. Não deduzir resolução
da cena apenas dessa lista nem tratá-la como histórico completo por quadro.

Nas primeiras execuções, transferências observadas de 33 e 64 MiB custaram
aproximadamente 27–42 ms. Houve lote de comandos de aproximadamente 300 ms
sem uma única transferência explicando sua duração. Diagnóstico de pacote
foi ampliado para separar operações individuais. Uma primeira tentativa tinha
limite compartilhado consumido por esperas do menu; limite agora separado.
Travada original de aproximadamente 800 ms continua sem causa corrigida.

## Opção descartada

Foi prototipada uma opção opt-in de desligar três getters de efeitos, testada
em CPU. Davy explicitou fidelidade antes da validação visual dessa opção.
O teste isolado foi fechado normalmente (exit 0), antes de chegar ao sprint.
Opção, atalho, parâmetro e fontes foram retirados da integração e o runtime
recompilado sem eles. Pesquisa própria preservada somente no arquivo privado
`build/research/discarded-blur-option-20261017.zip`; não distribuir nem reativar.
Os ZIPs dos amigos de 17/10 foram gerados antes desse protótipo e não o contêm.

## Busca exata de imagens na primeira página

`FindImage` percorre o intervalo inteiro para reunir imagens candidatas, mesmo
quando já existe uma imagem com endereço, tamanho, dimensões, tipo e formato
compatíveis. Uma candidata exata começa no mesmo endereço e necessariamente
está na primeira página. As candidatas exatas aparecem nessa página na mesma
ordem da varredura completa; preservar a última mantém a escolha original.

O caminho novo consulta essa página sob o mutex existente, respeita `Picked`,
sobreposição e todos os critérios originais. Sem candidata exata, executa a
varredura/resolução de sobreposições anterior. Não mantém cache auxiliar,
handles persistentes ou índice que precise invalidar. Atualizações de recurso,
LRU, views, amostras, cópias e barreiras permanecem no código original.

5.000 layouts aleatórios (3.061 acertos exatos) comparados com varredura de
referência passaram: aliases, registro fora da ordem dos slots, exclusões,
flags, formatos compatíveis/estritos e fallback por sobreposição.
`KYOEI_VERIFY_IMAGE_LOOKUP=1` executa ambas as buscas e confere cada consulta
real antes de resolver recursos; partida de verificação passou sem divergência,
incluindo o marco de 10.000 consultas, imagem conferida e saída 0.
`KYOEI_LEGACY_IMAGE_LOOKUP=1` permite o comparador anterior no mesmo executável.

Comparativo de aproximadamente 25 segundos, mesma vista e foco ausente em
48/48 verificações em ambas: original 30,09 FPS/máximo 48,77 ms; novo 30,14
FPS/62,98 ms, seis janelas de 120 intervalos em cada. Imagens conferidas,
ambos exit 0. Sem ganho de FPS demonstrado nessa cena; não declarar a travada
resolvida. Logs/ranges/imagens `image-{old,fast}-20261017` no perfil privado.
