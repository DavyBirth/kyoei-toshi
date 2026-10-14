@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\run_native.ps1" -Language en -PerformanceStats
pause
