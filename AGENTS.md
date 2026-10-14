# Kyoei Toshi (City Shrouded in Shadow): preservação, tradução e port para PC

## Quem é quem
- **Davy** é o dono do projeto. Ele não programa: testa, olha a tela, joga e aprova. Fale com ele em português simples e diga sempre o que ele deve ver se der certo.
- **O agente** escreve, compila, roda e depura todo o código.

## O jogo
- 巨影都市 / Kyoei Toshi (City Shrouded in Shadow), PS4, Bandai Namco, desenvolvimento Granzella, 2017. Lançado no Japão, sem versão oficial em inglês ou português.
- **Cópia: somente o dump do disco do próprio Davy**, feito no PS4 dele. Nunca usar cópias baixadas da internet. Registrar no M0 o código do disco (CUSA…), a versão e os hashes.
- Pasta do dump: preencher no M0. Nunca versionar nada dela.
- Referência de comportamento: o emulador shadPS4, onde o jogo roda.

## Objetivo final: preservação
1. **Documentar** como o jogo funciona: formatos de arquivo, estrutura dos dados, textos, tudo em `docs/`.
2. **Traduzir** para inglês e português do Brasil, distribuindo só um patch (o texto traduzido e as ferramentas), nunca arquivos do jogo.
3. **Portar para PC** seguindo o projeto irmão do Godzilla (`G:\godzilla-port-codex`), que usa a mesma abordagem: carregador do executável do PS4, runtime das funções do sistema e gráficos do shadPS4, no Windows. Reaproveitar ao máximo o carregador e o runtime de lá.
4. **Reimplementar** aos poucos, no estilo OpenRCT2, até sobrar só a leitura dos arquivos de dados, como o port do P.T. (https://github.com/LoreanXavier/pt-pc).

## Marcos
- **M0, inventário:** confirmar o dump (eboot.bin, sce_module, sce_sys/param.sfo, pasta DAT com os .psarc), registrar código, versão e hashes, e listar módulos e importações do eboot como foi feito no Godzilla (`tools/imports.py` do projeto do Godzilla).
- **M1, arquivos:** extrair os .psarc (formato conhecido) e catalogar o que há dentro: textos, fontes, imagens, sons, vídeos, scripts.
- **M2, textos:** achar onde ficam todos os textos (diálogos, menus, legendas, itens), escrever um extrator e um reinseridor, e provar a ida e volta: extrair, reinserir sem mudanças e o jogo continuar idêntico no shadPS4.
- **M3, fontes:** garantir que a fonte do jogo tenha os caracteres do inglês e do português (acentos, ç, ã, õ). Se não tiver, adicionar.
- **M4, tradução:** planilha ou arquivos por cena com japonês, inglês e português lado a lado. A IA pode fazer o primeiro rascunho, mas tudo passa por revisão humana. Glossário fixo para nomes de monstros, heróis e lugares.
- **M5, patch:** um aplicador que pega o dump do usuário e gera a versão traduzida, para usar no shadPS4 (e no port, depois).
- **M6 em diante, port para PC:** adaptar o carregador e o runtime do projeto do Godzilla, com a escolha de idioma no próprio port.

## Código compartilhado com o Godzilla
- O carregador, o runtime e a parte gráfica do Godzilla (`G:\godzilla-port-codex`) são a **base de partida** do Kyoei, por já estarem mais avançados.
- Aqui fica uma **cópia fixa** desse código em `third_party/godzilla-core`, com a versão (commit) anotada no diário.
- Por orientação explícita de Davy em 08/10/2026, melhorias necessárias ao Kyoei são feitas e testadas **neste projeto**, sem exigir mudanças prévias no Godzilla. Preservar a cópia fixa de referência; adaptações próprias ficam em `src/` e na integração de compilação. Não modificar o projeto do Godzilla nesta sessão.
- O port do Kyoei (M6) só começa depois que o Godzilla tiver a parte gráfica funcionando (M5 de lá).

## Regras
- Regra permanente de Davy em 13/10/2026: apagar arquivos que não precisam ser mantidos. Para derivados obsoletos e recriáveis, conferir substitutos/dependências/processos e remover sem pedir nova autorização. Preservar dump, saves, trabalho único, arquivos atuais e evidências necessárias; não tratar dúvida sobre uso como prova de que um arquivo é dispensável.
- Nada do jogo no repositório: nem executável, nem .psarc, nem texto japonês original completo. O patch só contém a tradução e as ferramentas.
- Passos pequenos e um commit por avanço que funciona. Um marco só está feito quando dá para provar.
- Manter `docs/DIARIO.md`: ler antes de cada sessão e atualizar antes de encerrar.
