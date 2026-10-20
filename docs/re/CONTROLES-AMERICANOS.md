# Controles americanos

Pedido de Davy em20/10/2026: X confirma/interage; bola volta/cancela.
Adaptacao em CMake gera o pad proprio trocando SOUTH/EAST. O jogo continua
recebendo suas mascaras nativas: 0x2000 para confirmar, 0x4000 para voltar.
Isso tambem troca as acoes desses dois botoes durante a partida. Teclado e
mouse mantem os comandos anteriores. O modo foto conserva seus comandos
proprios, incluindo X para fotografar. A copia fixa Godzilla nao foi editada.

## Indicacoes na tela

Dialogos de save, mensagens e teclado virtual mostram X confirmar/bola voltar.
`tools/build_prompt_pack.py` gera KYPROMP2 com tres variantes por textura:
original para reconhecer a imagem convidada, teclado e controle americano.
O upload escolhe a variante; nao escreve na memoria de texturas do jogo nem
nos arquivos originais. O reconhecimento continua usando original/teclado,
evitando confundir as duas imagens apos a troca de simbolos.

45 entradas cobrem os prompts anteriores, mais a troca de objetos:
fonte de icones, HUD/interacoes, intermission/dialogos, esquiva, seis marcadores
de conversa e TC_maru/TC_batu. Pares nativos compativeis trocam seus pixels
originais; glifos compostos preservam bordas/hastes e recebem o outro simbolo.
Os glifos compostos usam a mascara suavizada do X/bola da fonte nativa,
preservam o fundo opaco do marcador e mantem byte-identicos os blocos de mip
fora da regiao alterada. Nao redesenhar o X com linhas nem apagar para alpha0:
essas abordagens produziram o defeito apontado por Davy em salvar/conversar.
Os arquivos derivados ficam somente no build privado. O gerador da distribuicao
tambem usa o formato novo. O lancador local exige pacote atualizado; runtime
aceita o formato antigo para compatibilidade, mas avisa que precisa reconstruir.

## Provas e limites

Build completo PASS. Teste compila a navegacao real dos dialogos e usa as
mascaras extraidas do pad gerado: X fisico confirma, bola fisica cancela,
uma/duas respostas e lista de saves PASS. Leitura dos pacotes finais japones/
ingles pelo leitor real PASS; selecao de pixels e reconhecimento com memoria
reutilizada PASS. Atalhos ja/en em CheckOnly PASS.

Partida privada/muda `american-physical` confirmou indicacoes dos dialogos
e X no marcador de salvar. O defeito nesse glifo composto foi apontado por
Davy. Corrigido com glifo nativo, fundo opaco e preservacao dos blocos originais.
Teste com arquivo proprio cobre sete interiores opacos e igualdade exata dos
pixels das seis hastes. Partida privada/muda `buttons-native-glyph` confirmou
o marcador de salvar com X nativo, fundo opaco e borda limpa; captura privada
`transfer-20261020/native-glyph-loaded.png`. Controle fisico ainda pendente.

Pendencia conhecida: quatro diagramas de instrucoes `operation_normal`,
`operation_normal_s`, `operation_normal_207` e `operation_normal_207_s` possuem
simbolos, textos e linhas de chamada embutidos. Ainda mostram a disposicao
original. Precisam adaptar as instrucoes e as chamadas, preservando as posicoes
fisicas dos botoes no desenho do controle. Nao declarar todas as texturas prontas.
