@echo off
setlocal
cd /d "%~dp0"
set "KYOEI_PHOTO_HUD=1"
set "KYOEI_PHOTO_HUD_OBSERVE=0"
echo Previa do modo foto em 1080p. F10: ocultar/restaurar HUD. F12: salvar PNG em photos.
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\run_native.ps1" -Language en -Width 1920 -Height 1080 -InGameDialogs %*
if errorlevel 1 pause
