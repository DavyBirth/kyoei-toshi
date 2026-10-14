# Backend de saves — verificações offline

Lista de slots rejeita nomes com UTF-8 inválido e valida o índice retornado
pela apresentação antes de acessar a lista. Índice fora dos limites é tratado
como cancelamento, sem copiar memória fora do array para o diretório de saída.
Regressões em tools/test_save_list.py exercitam o serviço adaptado real.

O port mantém os arquivos do jogo na pasta de cada slot e os metadados ao lado,
em `.sce_sys/param.bin`. Não mudar o formato dos arquivos gravados pelo jogo.
As adaptações ficam em src/ e em cópia gerada de rt_savedata.c; core fixo intacto.

## Gravação de metadados

`kyoei_save_metadata.h` escreve uma cópia temporária exclusiva na mesma pasta,
confere escrita completa, descarrega os buffers e substitui param.bin. Falha
de escrita ou substituição retorna ERR_INTERNAL ao jogo, em vez de sucesso.
Na falha, remove somente o temporário criado e preserva o param.bin anterior.
Não cria backups permanentes nem duplica pacotes. Criação de slot que falha
antes de publicar metadados tenta remover apenas as duas pastas vazias criadas,
sem recursão; qualquer pasta com conteúdo permanece.

Isso protege esta operação de metadados: não é transação dos arquivos de
payload do jogo, nem garantia contra todas as formas de queda de energia.

## Identificação de montagens

`kyoei_save_slot.h` aceita somente o nome completo `/savedata0` a `/savedata15`,
com terminador dentro do campo ABI. Prefixos, lixo no final, zeros extras e
campos sem terminador não podem selecionar/desmontar o slot 0 por atoi.

## Evidências sem gameplay

`python tools/test_savedata_storage.py` compila o backend realmente adaptado
com mocks só da raiz/montagem/log. Escreve arquivos sintéticos em
uma pasta temporária validada dentro de build/test-save-storage, removida ao
terminar; nunca aponta aos saves de Davy. Prova:

- payload e metadados persistem após desmontar/terminar/reabrir;
- erro e escrita parcial preservam todos os bytes dos metadados anteriores;
- substituição bloqueada por handle real do Windows retorna erro e preserva
  o original; não sobra temporário;
- criação falha não publica montagem e permite nova tentativa bem-sucedida;
- nomes de montagem inválidos não desmontam o slot ativo.

`test_kyoei_savedata` verifica encaminhamento da ABI Mount2 à implementação
compartilhada. Testes não iniciam jogo, janela ou acesso aos saves reais.
