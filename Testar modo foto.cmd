@echo off
setlocal
cd /d "%~dp0"
set "KYOEI_PHOTO_HUD=1"
set "KYOEI_PHOTO_HUD_OBSERVE=0"
set "KYOEI_PHOTO_SCENE=preview"
set "KYOEI_PHOTO_PHONE=1"
set "KYOEI_PHOTO_PHONE_OBSERVE=0"
set "KYOEI_PHOTO_SAVE_MARK=1"
set "KYOEI_PHOTO_SAVE_MARK_OBSERVE=0"
set "KYOEI_PHOTO_DOF=1"
set "KYOEI_PHOTO_LIGHTS=preview"
echo Previa em 1080p. F10: entrar/sair do modo foto. F12: salvar PNG em photos.
echo WASD: mover camera. Espaco/Ctrl: subir/descer. Mouse ou J/I/K/L: olhar.
echo Roda do mouse: zoom. Shift: mover mais rapido. A partida pausa durante a foto.
echo F11: mostrar/ocultar protagonista (experimental).
echo Foco e abertura: ajustes experimentais nas setas; R restaura o original.
echo Luzes: tres fontes independentes, gerais ou direcionadas, com posicao/cor/intensidade.
echo Controle Luz selecionada: WASD e mouse movem/apontam a luz com camera parada.
echo F9: ajuda. Controle View/Share: entrar/sair; os comandos aparecem no modo foto.
echo Tab ou 1-4 / L1-R1: categoria. Cima/baixo: selecionar. Esquerda/direita: ajustar.
echo R ou R3: restaurar ajustes. Cores aparecem na previa e na foto salva.
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\run_native.ps1" -Language en -Width 1920 -Height 1080 -InGameDialogs %*
if errorlevel 1 pause
