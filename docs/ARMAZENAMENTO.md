# Armazenamento e recuperação de extrações

Limpeza de 09/10/2026: total medido do projeto e pasta de trabalho caiu de
**38,44 GB para 1,16 GB**. Removidos **37,29 GB** (aproximadamente 97%).
O dump original de 17,80 GB fica separado e não está incluído nesses totais.
Valores em GB decimais; o Windows pode apresentar números diferentes em GiB.
Medidas e verificações em `storage-cleanup.json`.

O dump original fica em
`G:\GodzillaShadPS4PC\launcher\game_data\CUSA03562`. Ele é a origem dos dados
do jogo e nunca entra na limpeza do projeto.

O port usa esse dump diretamente, `build/kyoei.exe` e a preparação
`G:\kyoei-toshi-work\native-04\out`. Salvamentos e caches atuais ficam em
`native-04\user`. Conservar essas pastas e a base fixa `third_party/godzilla-core`.

A extração M1 em `G:\kyoei-toshi-work\m1` agora é parcial: ficam os textos,
scripts, tabelas e demais recursos pequenos. As cópias extraídas de texturas,
modelos, pacotes de animação, sons e vídeos são reproduzíveis e foram removidas.
O catálogo completo em `work/m1/catalog.json` e as provas históricas em
`docs/m1` continuam intactos. Não recriar o catálogo a partir da pasta parcial.
`verify_m1.py` continua exigindo uma extração completa para repetir a prova M1.

Para recuperar um recurso, a ferramenta verifica o catálogo e o hash do
PSARC original, extrai somente o necessário e compara com o hash registrado:

```powershell
python tools/restore_m1.py system.psarc --entry GxCore/texture/null-black.tex
```

Sem `--entry`, recupera os recursos ausentes daquele pacote inteiro. Usar essa
forma apenas quando for necessário, pois pacotes grandes recriam vários GB.
Arquivos já presentes são conferidos e nunca sobrescritos.

Também foram removidos o dump duplicado de prova M2 e as preparações antigas
`native-01/out`, `native-02/out`, `native-03/out`. Logs, capturas, catálogos,
índices de texto, provas de ida e volta e dados de usuário foram preservados.
Esses resultados continuam sendo evidência histórica; suas grandes saídas
reproduzíveis não precisam continuar no disco.

O inventário detalhado da limpeza fica fora do Git em
`G:\kyoei-toshi-work\cleanup-20261009`. Os scripts `plan_cleanup.py` e
`cleanup_storage.ps1` registram esta operação com lista fixa de alvos; não são
uma limpeza periódica automática e não devem ser repetidos indiscriminadamente.
