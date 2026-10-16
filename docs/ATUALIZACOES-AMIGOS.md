# Atualizações pequenas

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
registrar as versões base aceitas; esta ferramenta aceita uma base por pacote.

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
