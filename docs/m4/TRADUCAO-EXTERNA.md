# Entrega de tradução inglesa para o ChatGPT

Atualização: Davy pediu português também, depois do inglês que já está em
andamento. Entrega EN anterior permanece intacta. Pacote PT separado:
`G:\kyoei-toshi-work\translation\external-pt-full-01\Kyoei-Portugues-handoff.zip`.
São 16.136 candidatos novos e 94 rascunhos PT como contexto, 173 JSONs.
O importador aceita en/pt-BR separadamente; duplicidade é verificada por
idioma, permitindo adicionar PT a um ID cujo inglês já existe. Nove testes
PASS e fixture real com acentos/reinserção PASS. Davy retorna os ZIPs/JSONs
anexando nesta conversa, inclusive lotes parciais. Mesmo limite de cobertura
de imagens/vídeos/HUD binário do pacote EN.

Preparada a pedido de Davy em 13/10/2026. Somente rascunho; tradução final
depende de revisão humana e layout. Não compartilhar os arquivos do dump.
Os arquivos desta entrega contêm texto original privado e ficam fora do Git.

Entrega atual: `G:\kyoei-toshi-work\translation\external-en-full-01`.
Enviar `Kyoei-English-handoff.zip` e as instruções de `INSTRUCTIONS.txt`.
O ZIP contém 173 arquivos de trabalho, glossário e manifesto: 16.100
candidatos novos em 138 recursos, incluindo 20 recursos Text_Stage.
As 130 traduções inglesas anteriores constam no manifesto como contexto;
não retorná-las como trabalho novo. ZIP verificado, 1.244.259 bytes.

Isso cobre os candidatos japoneses do catálogo Lua/CSH auditado, com exclusão
de tabelas de fontes e filtros de nomes. Candidatos não equivalem a falas
visíveis: há constantes internas/debug misturadas. Não habilitar alterações
nelas apenas porque aparecem no arquivo. Créditos são opcionais e nomes
exigem leituras confirmadas. Há filas pendentes de texto em imagens, vídeos,
HUD binário e codificações não UTF-8; cobertura completa do jogo não certificada.

O ChatGPT deve preencher en/notes, preservando os demais campos e comandos.
Retorno em JSONs com os nomes originais, em lotes, com lista de pendências.
Não aceitar um resumo como substituto dos arquivos nem declaração de que
o jogo inteiro foi traduzido quando apenas alguns arquivos foram processados.

O agente importa cada arquivo com:

```powershell
python tools/external_translation.py import CAMINHO_DO_JSON translations/novo-lote-en.json
```

O destino deve ser novo. O importador confere identidade, idioma, recurso,
hash do arquivo e de cada original, IDs duplicados/existentes, tokens e nomes
protegidos. Extrai somente os campos aprovados para o patch; japonês original
e claims de validação do outro GPT não entram. Prova reinserção/leitura real
e preservação das constantes não alteradas antes de criar o rascunho.
Valores vazios ficam pendentes, não apagam texto no jogo. A auditoria mecânica
não certifica qualidade da tradução, nomes não cadastrados ou layout.

Oito regressões sintéticas PASS; os 16.100 pares recurso/ID são únicos.
Uma fixture de retorno real p0/k271 passou importação/reinserção, sem original
no rascunho; removida depois do teste. Nenhum pacote do jogo reconstruído.

Para gerar uma nova entrega sem sobrescrever a atual:

```powershell
python tools/external_translation.py prepare-all DIRETORIO_PRIVADO_NOVO --limit 300
```

Não modificar os arquivos já enviados enquanto Davy ou o outro GPT trabalha
neles. Novas entregas precisam de outro diretório para manter hashes/IDs
coerentes durante a revisão.
