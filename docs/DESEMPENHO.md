# Desempenho — objetivo de 60 FPS

Davy pediu em 14/10/2026 análise minuciosa com objetivo de pelo menos 60 FPS.
Orçamento de referência: 16,67 ms por quadro. Objetivo ainda não atingido nem
garantido pelo hardware/código atual. Avaliar estabilidade dos tempos, não só
média; não obter FPS artificial repetindo frames ou acelerando lógica do jogo.

## Evidência disponível

Percursos observados em ANALISE-COMECO.md: 22,74/25,41 FPS, misturando eventos
e rua. Não são comparação controlada entre versões. TLS zero nesses trechos;
PrepareFrame ~0,16–0,18 ms, Present ~0,39–0,43 ms; uma janela de leitura linear
~11,65 ms. Tempos de threads diferentes não podem ser somados como caminho
crítico. Percentual global baixo de CPU/GPU não exclui thread ou fila saturada.

Reanálise offline de 20261013_135113.log: 100 janelas/12000 intervalos,
34,31 FPS agregados incluindo menus/transições; janela mínima 17,02 e máxima
60,05 FPS. Não representa FPS da cidade. Linear-downloads: seis janelas,
média 10,46 ms e máximo 21,50 ms; seis amostras de readback tinham uma imagem,
uma espera e apenas 256 bytes. Investigar sincronização/frequência, não supor
que a largura de banda desses 256 bytes seja o problema. Relatório privado:
build/performance-audit-20261014.json. Log antigo não marca atividade F9;
ausência da marca significa desconhecido, não medição limpa.

## Ordem de investigação

1. Rastrear consumidor dos readbacks e dependências/fences: o que exige resultado
   imediato, quantas submissões/esperas ocorrem e se um lote realmente as reduz.
   Preservar coerência, visibilidade de memória e resultados do jogo.
2. Medir threads de lógica, tradução de comandos, compilação de shaders, fila
   GPU e esperas. Work stats atuais medem tempo de parede de trechos da CPU;
   não são timestamps GPU nem atribuição completa do gargalo.
3. Investigar pacing/limites e se a simulação assume 30 Hz antes de desbloquear
   taxa. Confirmar movimento, animação, áudio, física e cutscenes na taxa maior.
4. Comparar builds na mesma rota/save, resolução, idioma e opções; separar
   cache frio/quente, carga/menus, diagnóstico F9 e gameplay. Fazer uma mudança
   por comparação. Traduzir texto não prova nem exclui regressão de desempenho.
5. Validar imagem e correção junto ao ganho. Não reduzir qualidade, pular
   sincronização ou habilitar batch readback por palpite.

Sem jogo: revisar caminhos, testar dependências com fixtures e analisar logs.
Próxima coleta autorizada: tempos brutos/identificação de cena quando possível,
CPU por thread e GPU correlacionados. Não abrir partida automaticamente.

O analisador agora preserva diagnostic=0/1 e legado desconhecido, separados
para frame/work stats; mantém também totais e pausas de todas as janelas.
Relata orçamento de 60 FPS, mas resumos de 120 intervalos não permitem calcular
FPS mínimo, contar todos os quadros atrasados nem obter p95 global exato.
Valores impressos são arredondados; médias junto ao limite requerem dados brutos.
