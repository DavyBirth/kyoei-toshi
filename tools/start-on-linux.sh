#!/bin/sh
# Runs the Windows build through Proton (umu-run) or Wine. Keeps its Wine
# settings inside this folder. CachyOS: sudo pacman -S wine  (or umu-launcher)
cd "$(dirname "$0")" || exit 1
export WINEPREFIX="$PWD/data/wineprefix"
mkdir -p "$WINEPREFIX"
if command -v umu-run >/dev/null 2>&1; then exec umu-run "City Shrouded in Shadow.exe"
elif command -v wine >/dev/null 2>&1; then exec wine "City Shrouded in Shadow.exe"
else echo "Install Wine or umu-launcher first (CachyOS: sudo pacman -S wine)."; exit 1; fi
