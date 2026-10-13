@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\test_orb.ps1" -Next
pause
