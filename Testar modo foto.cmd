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
echo Previa em 1080p. F10: entrar/sair do modo foto. F12: salvar PNG em photos.
echo WASD: mover camera. Espaco/Ctrl: subir/descer. Mouse ou J/I/K/L: olhar.
echo Roda do mouse: zoom. Shift: mover mais rapido. A partida pausa durante a foto.
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\run_native.ps1" -Language en -Width 1920 -Height 1080 -InGameDialogs %*
if errorlevel 1 pause
