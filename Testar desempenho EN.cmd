@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\test_hud.ps1" -FitHud -BatchReadback
pause
