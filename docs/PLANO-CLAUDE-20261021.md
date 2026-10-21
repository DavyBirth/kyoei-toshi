# Plano e repasse do Claude — 21/10/2026

Para o Codex e para o Davy. Atualizado a cada passo; ver commits recentes.

## Feito nesta rodada (Claude)

- Inglês natural (ramo `traducao-natural-en`, worktree `G:\kyoei-toshi-natural`):
  13.268 textos + 168 texturas + 397 textos fixos de HUD/eboot
  (`translations/binary-en.json`, `tools/apply_binary_text.py`). Testado no jogo
  com pacote delta privado: menus, opções, itens, missões, salvar, mapa.
- Telas de controle no padrão americano (pedido do Codex em
  `docs/INSTRUCOES-TEXTURAS-CLAUDE.md`), inclusive "amarrado" e "cofre".
- Prompts: × do marcador de conversa com o × azul nativo (peso correto);
  `cursol_nextchat`, `sc_next`, `resalt_next` agora trocam para × / tecla E
  (commit 7a4617e). `build/prompts-en.kypack` reconstruído, teste PASS.
- **Desempenho:** `KyoeiPrompts::Select` chamava `getenv` 3x por textura ligada;
  o amostrador mostrou ~25% da thread GpuCommandProcessor em `strchr`.
  Variáveis lidas uma vez (commit 269fb53). **A/B válido, mesma cena/posição
  (save SAVEDATA-001, máquina de bebidas, 1280x720, 40s): 26,54 → 42,28 FPS
  (+59%).** Capturas privadas claude-test/shots/s-011611 (antes) e s-011326.

## Teste privado do agente (não usar como atalho)

`G:\kyoei-toshi-work\claude-test\run.ps1` (mudo, perfil próprio, delta
`natural-en.kydelta` sobre runtime-preview-en-16, eboot patch em out/).
`measure.ps1` mede FPS no save; `step.ps1` envia teclas e captura.

## Prioridades (ordem)

1. Reamostrar a thread da GPU com o ganho aplicado;
   próximos alvos vistos no perfil: `TextureCache::RefreshImage/FindTexture`
   (~28%), `PipelineCache::RefreshGraphicsKey` (~11%), espera em `Cnd_wait`
   (~20%), `BeginRendering/FindDepthTarget` (~9%).
2. **Travada ao girar a câmera depois do load (pedido do Davy):** compilação de
   shader/pipeline na hora. Solução: pré-aquecer o cache de pipelines (salvar as
   chaves usadas e compilar no carregamento) — ver `docs/re/SHADER-STAGE-CACHE.md`.
3. Plano de controles (Davy, antes do PT-BR): janelas salvar/carregar/teclado
   com símbolos de botão e visual PlayStation (hoje mostram "SAVEDATA-001");
   detecção PS/Xbox/Nintendo/teclado com escolha manual (Nintendo = padrão
   japonês); teclas personalizáveis; texturas de tutorial por dispositivo.
   Variante teclado de "Press ◎ Button" ainda diz "Button".
4. Ajustes de inglês vistos no jogo: objetivo do minimapa rola (ok), "Park:
   meet your friend" sobrepõe o marcador; títulos de fase no eboot mais curtos
   que nas texturas (limite de bytes); créditos: nomes da equipe ainda em kanji
   (decidir com Davy); logo 巨影都市 e copyright do título.
5. Integrar o inglês natural ao principal (merge `traducao-natural-en`), gerar
   pacote oficial (runtime-preview + delta HUD/eboot) e o portable.
6. Pendências antigas do Codex (CONTINUAR-CODEX.md): luminância 1x1/readback,
   tempo/slow motion, Linux nativo. Modo foto fica com o Codex.
7. Depois: PT-BR (estrutura pronta: `build/textures-blank`, mesma tabela).

## Regras desta rodada

- Davy pediu salvar a cada 5 minutos: commits pequenos e frequentes.
- Claude não consegue ler o /status; Davy (ou irmão) desliga o PC.
