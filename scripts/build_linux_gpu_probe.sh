#!/bin/sh
# Compile the local reference GPU library as a Linux integration probe.
# This does not produce a playable Kyoei executable.
set -eu
probe_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
probe_source="$probe_root/build/reference-bloodborne/gpu"
probe_build="$probe_root/build/linux-full/gpu"
probe_headers="$probe_root/build/linux-full/Vulkan-Headers"
probe_revision=c46850864f4661461b0f6cb9922c058ffea4915e
test -f "$probe_source/CMakeLists.txt"
if [ ! -d "$probe_headers/.git" ]; then
    git clone https://github.com/KhronosGroup/Vulkan-Headers.git "$probe_headers"
    git -C "$probe_headers" checkout --detach "$probe_revision"
fi
test "$(git -C "$probe_headers" rev-parse HEAD)" = "$probe_revision" || {
    echo "Expected audited Vulkan-Headers $probe_revision; preserve/review the existing cache first." >&2
    exit 1
}
probe_xbyak="$probe_root/build/linux-full/Xbyak"
probe_xbyak_revision=cd6bad3cad2e07a3e277c23d2b1f065320024717
if [ ! -d "$probe_xbyak/.git" ]; then
    git clone https://github.com/herumi/xbyak.git "$probe_xbyak"
    git -C "$probe_xbyak" checkout --detach "$probe_xbyak_revision"
fi
test "$(git -C "$probe_xbyak" rev-parse HEAD)" = "$probe_xbyak_revision"
mkdir -p "$probe_build/include"
if [ ! -e "$probe_build/include/xbyak" ]; then
    ln -s "$probe_xbyak/xbyak" "$probe_build/include/xbyak"
fi
cmake -S "$probe_source" -B "$probe_build" \
    -DVulkan_INCLUDE_DIR="$probe_headers/include" -DBB_LTO=OFF
cmake --build "$probe_build" --target bbgpu --parallel "${KYOEI_BUILD_JOBS:-6}"
