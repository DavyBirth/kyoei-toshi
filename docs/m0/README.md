# M0 — inventário validado

Validação técnica concluída em 08/10/2026. Fonte local indicada no inventário:
`G:\GodzillaShadPS4PC\launcher\game_data\CUSA03562`.
Usar exclusivamente o dump do disco do Davy, conforme AGENTS.md. Os hashes
identificam os arquivos presentes; não comprovam a origem física da cópia.

| Item | Resultado |
|---|---|
| Código no param.sfo | CUSA03562 |
| CONTENT_ID | JP0700-CUSA03562_00-E060000000000001 |
| APP_VER (versão da aplicação) | 01.03 |
| VERSION (campo separado do SFO) | 01.00 |
| CATEGORY | gp |
| Arquivos | 87 |
| Tamanho total | 17.798.062.971 bytes |
| Arquivos DAT/*.psarc | 25 |
| Módulos .prx/.sprx presentes | 24 |
| Módulos declarados pelo eboot | 61 |
| Bibliotecas declaradas pelo eboot | 63 |
| Importações do eboot | 704 |
| NIDs sem nome conhecido | 10 |

`eboot.bin`, `sce_sys/param.sfo`, `sce_module` e `DAT` estão presentes.
Os módulos presentes incluem 19 StageModule.prx, libSceSecure.prx,
libc.prx, libSceFios2.prx, libSceNpToolkit2.prx e right.sprx.
Módulos declarados são dependências do executável; sua declaração não
significa que todos estejam distribuídos como arquivos no dump.

## Evidências e repetição

- `inventory.json`: caminhos relativos, tamanhos e SHA-256 dos 87 arquivos.
- `SHA256SUMS.txt`: os mesmos hashes em formato de lista.
- `param-sfo.json`: metadados comparados com leitura direta do SFO.
- `imports.json` e `IMPORTACOES.md`: importações e dependências.
- `tool-provenance.json`: caminhos e hashes das ferramentas do Godzilla utilizadas.

Na raiz deste projeto, executar `python tools/verify_m0.py`.
O resultado esperado é `VALIDADO: 87 arquivos; 17798062971 bytes; CUSA03562; APP_VER=01.03; VERSION=01.00`.
A conferência desta sessão releu todos os arquivos, confirmou os hashes,
a lista SHA256SUMS, os metadados SFO e os hashes das ferramentas.

As importações foram geradas novamente com:

```powershell
New-Item -ItemType Directory -Force work
python G:/godzilla-port-codex/tools/imports.py G:/GodzillaShadPS4PC/launcher/game_data/CUSA03562/eboot.bin G:/godzilla-port-codex/third_party/shadPS4/src/core/aerolib/aerolib.inl work/imports-verified.json
```

O JSON regenerado foi comparado estruturalmente com `imports.json`: idêntico.
Para regenerar o relatório legível, executar `python tools/report_m0.py`.
Nenhum arquivo do jogo foi copiado para o repositório ou alterado.
M0 não testa execução no shadPS4; a prova de reinserção pertence ao M2.
Não foi criada cópia do runtime e não foi iniciado o port M6.
