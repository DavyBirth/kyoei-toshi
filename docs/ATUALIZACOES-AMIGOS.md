# Atualizações pequenas

## Revisão atual: 18/10/2026

- Windows: `build/Kyoei-Windows-Update-20261018.zip`, 7.346.824 bytes.
- Linux/Wine: `build/Kyoei-Linux-Wine-Update-20261018.zip`, 7.348.002 bytes.
- Cumulativos: aceitam Friends-Test original e os updates de 15, 16, 17 e 17b.
- Runtime de produção separado, RelWithDebInfo, experimentos desligados.
  Reduz o custo dos diagnósticos desligados e permite medir apresentação sem
  instrumentar draws; mantém as melhorias das revisões anteriores. Engasgos
  e a meta de 60 FPS continuam pendentes; não há ganho grande de FPS comprovado.
- Três testes do gerador, três Windows e onze Linux/WSL passaram. Os ZIPs reais
  passaram em dez cenários (cinco bases por sistema): preflight sem escrita,
  aplicação, reaplicação, hash, backup exato, save e prefixo preservados.
  Evidência privada: `build/research/update-validation-20261018.json`.
- Somente runtime/aplicador/instruções/hashes. A revisão de inglês natural
  do ramo `traducao-natural-en` não está incluída: segue em prévia separada.

SHA256 Windows: `c6b12f1edec706e0f47d6a52d79deeb745aff0580d4c267c87aa94d1d2216ee4`.
SHA256 Linux/Wine: `d3a9453ff41df207b6aba2fc6f2a6222b86ea0e567419050c3dcc910c88b8c73`.
Runtime comum: `183bcfe24386fc698ac681b4bf4e36acd30ae3c511fb82e534173a9934229641`.

Fechar jogo e launcher, extrair na pasta do jogo substituindo a pasta
`Atualizacao-Kyoei`. Windows: abrir `Aplicar atualizacao.cmd`. Linux/Wine:
executar `sh Atualizacao-Kyoei/Aplicar-no-Linux.sh`, com Python 3.11+, sem sudo.
Abrir depois pelo launcher habitual. O Linux deste pacote usa Wine/Proton.

## Revisão anterior: 17/10/2026b

- Pacote Windows: `build/Kyoei-Windows-Update-20261017b.zip` (7.334.300 bytes).
- Pacote Linux/Wine: `build/Kyoei-Linux-Wine-Update-20261017b.zip` (7.335.464 bytes).
- Ambos usam o mesmo runtime e aceitam a instalação Friends-Test original e as
  atualizações de 15, 16, 17 e 17b de outubro. São pacotes cumulativos.
- Aplicação real em instalações descartáveis: Windows e Linux/Wine passaram nas
  quatro bases. Preflight, aplicação, reaplicação, backup, save e prefixo Wine
  conferidos em todos os oito cenários. Relatório privado em
  `build/research/update-validation-20261017b.json`.
- Estes ZIPs contêm somente o runtime próprio, o aplicador, instruções e hashes;
  nenhum arquivo do jogo, tradução completa, save ou configuração pessoal.

Hashes SHA256 dos ZIPs:

- Windows: `5c8d50a2ed3ba199eafb2c62734713c3e9e58bfb6aa234f09317968ce1df907b`
- Linux/Wine: `0655b01cc6f525b7170ab22ebdb97163f8e251891123b4c4db4b8210a1296c14`
- Runtime nos dois pacotes: `6196e2a968e248af422d080f21fa8449e67ce55e726501adf64743c6033f0e55`

Para os amigos: fechar o jogo e o launcher, extrair o ZIP na pasta que contém
`City Shrouded in Shadow.exe` e substituir a pasta `Atualizacao-Kyoei` se ela
já existir. No Windows, executar `Atualizacao-Kyoei/Aplicar atualizacao.cmd`.
No Linux x86-64 com Wine/Proton, executar `sh Atualizacao-Kyoei/Aplicar-no-Linux.sh`
(Python 3.11+; sem `sudo`). Depois, iniciar pelo launcher habitual. O aplicador
confere versão e SHA256, cria backup do runtime anterior e preserva saves/cache.

Este é um atualizador manual por ZIP. Não há endereço de releases configurado
para busca/download automático. O pacote Linux atende Wine/Proton; o port Linux
nativo ainda não inicia o jogo.

## Revisão anterior: 17/10/2026

- Windows: `build/Kyoei-Windows-Update-20261017.zip`, 7.327.905 bytes.
- Linux/Wine: `build/Kyoei-Linux-Wine-Update-20261017.zip`, 7.329.069 bytes.
- Cumulativos: aceitam Friends-Test original e updates de 15 e 16/10.
- Pré-compilação dos pipelines conhecidos e nove conversores internos;
  união de buffers de vértices simplificada. Readback cooperativo experimental
  permanece desligado por padrão. Ainda há travadas; ganho de FPS não comprovado.
- Linux: não aborta por maps protegido de processo identificado sem relação
  com Wine/Proton. Se Wine, executável Windows ou identidade desconhecida não
  puder ser verificado, recusa e informa PID. Continua recusando runtime ou
  launcher mapeado; interpreta escapes de caminhos do /proc e compara caminho
  inteiro. Não precisa mudar permissões do sistema nem usar sudo.
- Onze testes Linux/WSL, três do gerador e três Windows passaram. ZIPs reais
  aplicados/reaplicados em seis instalações descartáveis (três bases, dois
  sistemas): hash, backup, save e prefixo Wine preservados. Arquivos permitidos
  e ausência dos identificadores pessoais conhecidos conferidos no payload.
  Relatório privado: `build/research/update-validation-20261017.json`.

Fechar jogo/launcher, extrair dentro da pasta do jogo substituindo a pasta
`Atualizacao-Kyoei`. Windows: `Aplicar atualizacao.cmd`. Linux:
`sh Atualizacao-Kyoei/Aplicar-no-Linux.sh`. Depois abrir pelo launcher habitual.
O erro de `/usr/bin/kitten` reportado acompanhava a saída 1 do aplicador;
não foi comprovada falha independente do terminal. Teste no Linux do amigo
continua necessário para confirmar seu ambiente específico.

Payload SHA256: `f4e2385bff0ec2dd7f58b78ed141f8c2d910e6fbe16e641f1f72e66a0e4b0aae`.
Windows ZIP: `442fd0d3d809272a766ad484465fcf157b4471b85797fa96f29d969f23556333`.
Linux ZIP: `b68bdc87b5fde2dff2f418dd2130dbf3ac97a98e945ef3508e6badbc4b1d8a65`.

## Revisão atual: 16/10/2026

- Windows: `build/Kyoei-Windows-Update-20261016.zip`, 7.311.654 bytes.
- Linux/Wine: `build/Kyoei-Linux-Wine-Update-20261016.zip`, 7.312.368 bytes.
- Ambos aceitam a instalação Friends-Test original e quem aplicou o update de
  15/10. São cumulativos: não precisa aplicar o anterior primeiro.
- Incluem retorno cedo na fila de callbacks vazia (menos consultas ociosas ao
  contador GPU) e diagnóstico limitado de recursos 1×1/esperas longas.
- Ganho de FPS ainda não comprovado; diagnóstico completo é opt-in e o jogo
  não foi aberto pelo agente para preparar esses pacotes.

Extrair na pasta do jogo, substituindo os arquivos da pasta `Atualizacao-Kyoei`
se ela já existir. Fechar jogo e launcher antes. Windows: executar
`Aplicar atualizacao.cmd`; Linux: `sh Atualizacao-Kyoei/Aplicar-no-Linux.sh`.
Requisitos e backup permanecem como descritos abaixo. Instalações sintéticas
com os payloads reais das duas bases foram atualizadas e reaplicadas nos dois
sistemas: hashes, backups, save e prefixo Wine preservados (quatro cenários).
As pastas Friends-Test e Portable originais ficaram intactas.

Para gerar releases cumulativos, usar `--accept-update <ZIP-anterior>` (pode
repetir a opção). O gerador verifica identidade e hash do payload desses ZIPs
antes de aceitar os hashes instalados. Não adicionar hashes arbitrários.

Payload novo SHA256:
`509c01e74e7613ddc17033aa922cd3448b1c05329133f430e85e612d3a9dc7c6`.
Windows ZIP: `df8cbd08c599332b2b1c87a7d0e3ab9ce91e151b332cc169832123f5acc59f7d`.
Linux ZIP: `faee99ef256fc9b35f943b25c31fdd650dcb1280daa23cd96c1744103f7d9355`.

## Revisão anterior: 15/10/2026

Pacote preparado em 15/10/2026: `build/Kyoei-Windows-Update-20261015.zip`,
7.310.399 bytes (aproximadamente 7,3 MB). É exclusivo para Windows x64,
destinado ao runtime do pacote Friends-Test enviado anteriormente.
Não contém dump, arquivos preparados do PS4, traduções completas do jogo,
saves, configurações pessoais, logs ou caches. Contém só runtime próprio,
aplicador, hashes e instruções. Não enviar novamente o pacote de 20 GB.

## Aplicar

1. Fechar o jogo e o launcher.
2. Extrair o ZIP dentro da pasta onde está `City Shrouded in Shadow.exe`.
3. Abrir a pasta `Atualizacao-Kyoei` e executar `Aplicar atualizacao.cmd`.
4. Após a mensagem de sucesso, abrir o launcher normal.

Não precisa instalar Python nem executar como administrador. PowerShell já
acompanha o Windows. O aplicador verifica título/versão, integridade do payload
e hash do runtime instalado antes de substituir. Recusa versões inesperadas.
Reaplicar o mesmo update informa que já está instalado.

O runtime anterior fica em `data/update-backups/<hash>.exe`; saves e cache
permanecem. Para desfazer, fechar jogo/launcher e copiar esse backup sobre
`runtime/kyoei.exe`. A substituição usa File.Replace com backup, depois da cópia
e verificação do arquivo novo. O bloqueio de sessão impede iniciar pelo launcher
durante a aplicação. Não iniciar executáveis manualmente enquanto aplica.
O manifest.json original permanece como identificação da distribuição base;
o runtime instalado pode passar a ter o hash novo do update.

## Conteúdo e limites

Este pacote acumula o runtime Windows atual em relação à cópia Friends-Test:
inclui as mudanças compiladas e a coleta detalhada, quando ativada por ambiente.
Não altera idioma, arquivos PS4, launcher ou Linux. Ganho de FPS continua sem
validação comparativa. Antes de apresentar aos amigos como melhoria de
performance, validar a mesma cena com a coleta desativada.

O pacote distribuído inicia via Wine/Proton no Linux, conforme `Start on Linux.sh`.
Ele usa o mesmo runtime Windows; há agora um ZIP com aplicador Linux, descrito
abaixo. Isso corrige a afirmação anterior de que era necessária outra compilação
para atualizar essa distribuição. O port Linux nativo experimental é separado.
Mudanças futuras em fontes, traduções ou launcher precisam
de uma lista explícita de arquivos próprios, com hashes e aplicação testada.
Nunca incluir `game/`, saves ou dados originais nesses pacotes de atualização.

## Reproduzir e verificar

```powershell
python tools/build_runtime_update.py --baseline build/City-Shrouded-in-Shadow-Friends-Test --out build/Novo-Update-Windows.zip
python -m unittest discover -s tools -p test_runtime_update.py
```

A saída deve ser um ZIP novo em `build/`. O gerador confere o runtime base com
seu manifest e limpa metadados de desenvolvimento do PE. Para releases futuras,
registrar as versões base aceitas; a ferramenta aceita a distribuição base e
os payloads dos ZIPs anteriores indicados por `--accept-update`.

Validação de 15/10: três testes com arquivos sintéticos (aplicação/backup/saves,
runtime incompatível e payload danificado), reaplicação, preflight do ZIP real
contra Friends-Test, conteúdo fechado em cinco arquivos e varredura dos tokens
pessoais conhecidos no executável sem ocorrências. Não executou o jogo nem
alterou a pasta Friends-Test ou o portable pessoal.

## Linux com Wine/Proton: pacote pronto

`build/Kyoei-Linux-Wine-Update-20261015.zip`, 7.311.112 bytes (~7,3 MB).
Atualiza a mesma base Friends-Test, com o mesmo payload do update Windows.
Requer Python 3.11 ou mais recente. Não requer Wine para executar o aplicador,
nem precisa de sudo. Não atualiza o port Linux nativo experimental.

Fechar jogo/launcher, extrair o ZIP dentro da pasta do jogo e abrir um terminal
nessa pasta:

```sh
sh Atualizacao-Kyoei/Aplicar-no-Linux.sh
```

Depois iniciar pelo `Start on Linux.sh` habitual. O aplicador verifica hashes e
versão, procura executáveis mapeados em `/proc` e recusa atualizar enquanto
estiverem em uso. Manter launcher e jogo fechados durante toda a aplicação.
`flock` impede concorrência entre aplicadores Linux; não equivale ao bloqueio de
arquivo Windows do Wine. Preserva saves/cache/prefixo Wine e copia o runtime
anterior para `data/update-backups/<hash>.exe`. Publica o novo runtime por rename
atômico no mesmo diretório. O backup permite desfazer como no Windows.

Testes em Ubuntu/WSL: quatro PASS, cobrindo preflight, substituição, backup,
preservação de save e permissões, reaplicação, payload corrompido, base errada e
recusa com runtime realmente mapeado. ZIP real verificado com `--check-only`
contra a base enviada sem alterá-la. Compatibilidade de gameplay no Wine/Proton
do amigo continua dependendo do teste dele; não há ganho de FPS validado.

```powershell
python tools/build_runtime_update.py --baseline build/City-Shrouded-in-Shadow-Friends-Test --out build/Novo-Update-Linux-Wine.zip --linux-wine
```
