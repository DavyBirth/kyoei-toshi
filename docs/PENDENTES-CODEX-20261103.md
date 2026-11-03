# Pendências para o Codex (03/11/2026)

Base: branch `desempenho-codex`, commit atual. Ler antes o fim de `docs/DIARIO.md` (03/11).

## 1. Ícones de PlayStation no modo teclado
Aparecem botões PS mesmo com teclado:
- △ ao lado do ícone do celular (HUD em campo); deveria ser a tecla F.
- Rodapés do jornal/intervalo ("L Rolar, SHARE, ✕ Sair"; "L Selecionar, ◯ Olhar, ✕ Sair") e da Coleção.
- Mapa de teclas: `src/kyoei_pc_controls.h` (F=0x1000 △, E/Enter=0x2000 ◯, Espaço=0x4000 ✕, Ctrl=0x8000 □).
- Ícones de teclado: `tools/make_keyboard_icons.py`, pacote `tools/build_prompt_pack.py` (prompts.kypack por idioma).

## 2. Botão "Modo foto" no menu de pausa
Abaixo de "Carregar jogo" (como no Godzilla). Textura do menu: `texture/hud/mmenu_selecttxt01..06.tex` (pt-BR em `translations/textures-en.json`, regiões `language: pt-BR`). Modo foto já existe: `src/kyoei_photo_*.c`.

## 3. Falta conferir na tela (precisa de Davy ou de controle)
- Leitor de livros (Itens → Por categoria → Leituras): ver se quebra palavras. Se quebrar, fazer como `src/kyoei_im_wrap.c`.
- Tela de fim de jogo ("Continuar?/Sim/Não" já traduzidos).
- Rodapé de resposta de e-mail parece invertido (setas=Enviar, E=Escolha; strings 返信決定/返信選択): comparar com o japonês.
- Ordem do nome em Personagens/jornal ("Misaki Ken").

## Ferramentas de teste
- Galeria: `tools/build_text_gallery.py` (--mails --items --intermission --item-prefix --only) + `build_localized_delta.py --extra lua/plan.luc=...`.
- Entrada de teste: `KYOEI_TEST_INPUT` aceita `tap <tecla> <ms>` e `click <x> <y>` (frações da janela; o clique ainda não troca abas de Itens).
- Sempre mudo (`tools/mute_test_audio.ps1`) e perfil `G:\kyoei-toshi-work\claude-test3\user`.
