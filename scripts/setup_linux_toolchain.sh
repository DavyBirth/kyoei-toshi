#!/bin/sh
# Run inside Ubuntu WSL after scripts/setup_linux_build.ps1.
set -eu
if [ "${1:-}" != "--verify-only" ]; then
    apt-get update
    DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential clang cmake ninja-build pkg-config git python3-dev libvulkan-dev libsdl3-dev libavcodec-dev libavformat-dev libavutil-dev libswresample-dev libswscale-dev
fi
test "$(uname -m)" = x86_64
clang --version | head -n 1
cmake --version | head -n 1
ninja --version
pkg-config --modversion vulkan sdl3 libavcodec libavformat libavutil libswresample libswscale
linux_project_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
mkdir -p "$linux_project_root/build/linux-environment"
printf '#include <stdio.h>\nint main(void){puts("Linux x64 compiler smoke PASS; this is not the game.");return 0;}\n' | clang -Wall -Wextra -Werror -x c - -o "$linux_project_root/build/linux-environment/compiler-smoke"
"$linux_project_root/build/linux-environment/compiler-smoke"
file "$linux_project_root/build/linux-environment/compiler-smoke"
