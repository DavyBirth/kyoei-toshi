# Atualização da tradução inglesa — 24/10/2026

**PENDENTE: Davy pediu interrupção em 24/10. Não distribuir ainda.**

O ZIP montado foi renomeado para `build/Kyoei-English-Translation-Update-20261024-PENDENTE.zip`. Os testes pequenos passaram, mas a instalação completa com o arquivo real ainda não foi validada. As instruções abaixo descrevem o funcionamento implementado, não uma liberação aprovada.

O pacote privado `build/Kyoei-English-Translation-Update-20261024.zip` transforma
a tradução en-16 enviada aos amigos na revisão natural en-17. O download tem
cerca de 10,2 MB, em vez de transportar o PSARC de 6,35 GB. Serve para Windows
e Linux com Wine/Proton; o aplicador usa Python 3.11 ou mais recente.

## Como instalar

1. Instalar primeiro o update **GoW3 de 24/10/2026** do sistema correspondente,
   descrito em [ATUALIZACOES-AMIGOS.md](ATUALIZACOES-AMIGOS.md).
2. Fechar jogo e launcher e mantê-los fechados até terminar.
3. Extrair o ZIP da tradução na pasta que contém `City Shrouded in Shadow.exe`.
4. Windows: abrir `Atualizacao-Traducao/Aplicar traducao.cmd`.
   Linux: executar `sh Atualizacao-Traducao/Aplicar-no-Linux.sh`, sem sudo.
5. Aguardar a mensagem de conclusão. Abrir o launcher habitual e escolher inglês.

O inglês passa a usar a revisão natural de diálogos, dicas e texturas, mais os
textos fixos de HUD/executável. O launcher e os recursos de interface necessários
também são atualizados. O runtime, arquivos em `game/`, japonês, saves, ajustes,
cache e prefixo Wine permanecem como estavam. Nenhum jogo é iniciado pelo update.

São necessários cerca de **6,5 GB livres durante a montagem**. A pedido de Davy,
o aplicador **exclui a tradução antiga após conferir os hashes e concluir a
instalação**. Não deixa um backup de 6,35 GB ocupando espaço. Permanecem somente
o registro da transação e os backups pequenos dos arquivos auxiliares trocados.

## Verificações e recuperação

O aplicador exige o hash conhecido da en-16 e do runtime GoW3 de 24/10. Recusa
pacotes corrompidos, caminhos fora da instalação e arquivos auxiliares alterados
que não correspondam ao manifesto instalado. Uma en-17 completa já instalada é
reconhecida sem substituir os arquivos novamente. O `manifest.json` original
continua identificando a base; a transação fica em `data/update-backups`.

Antes da instalação, todos os novos arquivos são preparados e seus hashes
conferidos. Os anteriores são movidos para recuperação durante a troca. Falhas
de instalação provocam restauração automática. Se faltar energia ou o processo
for encerrado, o próximo uso identifica a transação pendente e informa a pasta
para recuperação:

```text
python Atualizacao-Traducao/apply_translation_update.py --rollback "PASTA_DA_TRANSACAO"
```

No Linux, usar `python3`. O jogo e launcher precisam estar fechados. Essa
recuperação vale **antes de concluir**: depois do sucesso, o PSARC antigo é
apagado conforme solicitado e a reversão completa não está mais disponível.

Para conferir compatibilidade sem instalar:

```text
python Atualizacao-Traducao/apply_translation_update.py --check-only
```

## Formato e reprodução

`translation_patch.py` compara os corpos dos PSARCs, descontando suas TOCs.
Ambos foram gerados por `psarc_patch.py` sobre o mesmo dump: compartilham o
corpo original e acrescentam os recursos traduzidos no final. O patch guarda a
TOC nova e a cauda traduzida; manda copiar localmente 6.344.699.095 bytes da
en-16. O formato `KYTRPAT1` contém hashes SHA256, tamanhos e os limites dessa
cópia. As leituras são feitas em blocos, sem carregar o PSARC inteiro na RAM.

O patch de arquivo tem 8.209.392 bytes antes da compactação do ZIP. O restante
do download contém o aplicador, launcher e recursos derivados de interface.
Não contém `common.psarc`, executável de PS4, saves ou configurações pessoais.
Os derivados ficam somente no `build/` ignorado; no Git entram as ferramentas,
testes e esta documentação.

```text
python tools/build_translation_update.py --base CAMINHO_DA_EN16/common.psarc --out build/Kyoei-English-Translation-Update-20261024.zip
```

O gerador exige a portátil privada atualizada e um ZIP de saída ainda inexistente.
Base en-16: `489f1cb783d1b4b5ea62b8cd1432b59ce57efb94d1b33a8dcb7bb18c8a7a9d58`.
Destino en-17: `c06e2d4044559c664fa7d9d304314390495befa9162bd222679bd110c0b63e28`.

O tamanho e SHA256 do ZIP ficam no arquivo JSON de mesmo nome em `build/`.
A validação técnica não substitui a conferência visual da en-17 no jogo.
