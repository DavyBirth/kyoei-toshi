# Kyoei Toshi — City Shrouded in Shadow

Preservation, fan translation and a PC port of **巨影都市 / Kyoei Toshi** (*City Shrouded in
Shadow*, PS4, Bandai Namco / Granzella, 2017), a game released only in Japan.

The project ships **no game data**. Everything here works on a dump of a disc you own; the
repository holds only tools, the runtime, translations and documentation.

## What it does

- **PC port (Windows, Linux through Wine):** a loader for the game's PS4 executable, a runtime
  for the system libraries, and a Vulkan renderer (the GoW3 PC core, derived from shadPS4).
- **English translation:** read at run time over the original archive (a small delta, no
  modified copy of the 6 GB archive). Brazilian Portuguese is planned.
- **PC features:** keyboard and mouse; PlayStation, Xbox and Nintendo controllers with their
  own button prompts and controls diagrams; touch pad gestures with alternatives for pads
  without one; photo mode; game-styled save/load/name screens; graphics options (anisotropic
  filtering, AMD RCAS sharpening, AMD FSR 1). The PS4 look is the default.

Status: playable, experimental. The English text is under review. The frame rate is limited
by the processor (about 50–60 FPS standing, 35–45 turning the camera on a mid-range PC).

## Layout

| Path | What |
| --- | --- |
| `src/` | Kyoei's runtime pieces: input, overlay screens, photo mode, translation mount, HUD fitting, launcher (C#) |
| `gpu-gow3/` | Adaptation of the GoW3 PC GPU core to the Kyoei runtime (CMake adaptations and patches) |
| `tools/` | Python tools: dump inventory, archive (PSARC) and text formats, translation, textures, button prompts, controls diagrams, portable build, updaters |
| `translations/` | English translation (JSON per resource), UI texts, glossary of names |
| `docs/` | Findings: milestones `m0`–`m4`, game research `re/`, performance notes, the project diary (`DIARIO.md`, Portuguese) |
| `scripts/` | Windows/Linux helpers: run the prepared game, apply updates, Linux toolchain |

## Building

Requirements: Windows 10/11 x64, LLVM/Clang, CMake, Ninja, the Vulkan SDK, Python 3.11+ with
Pillow and NumPy (`tools/requirements-images.txt`).

`third_party/` is not versioned. It holds fixed snapshots, recorded with their hashes:

- `third_party/gow3-gpu` — `gpu/` of [gow3_pc](https://github.com/FelipePrado0/gow3_pc), tag
  `windows-v2.4.1` (`docs/gow3-gpu.json`);
- `third_party/godzilla-core` — the runtime of the companion Godzilla PS4 port, commit
  `e976b59` (`docs/godzilla-core.json`, `tools/snapshot_godzilla.py`);
- `third_party/imgui` — Dear ImGui.

```
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build
```

This builds `build/kyoei.exe` and the unit tests (`build/test_kyoei_*.exe`, `tools/test_*.py`).

## Using it with your own dump

1. Dump the game from your PS4 (CUSA03562, version 01.03) and record it: `tools/inventory_m0.py`,
   `tools/verify_m0.py`.
2. Prepare the executable: `tools/prepare_native.py`.
3. Build the portable folder (game files are copied from your dump, never downloaded):
   `tools/build_portable.py <output folder>`, then open `City Shrouded in Shadow.exe`.

## Legal

This is a non-commercial fan project. It contains no copyrighted game files; do not publish
derived game data (archives, textures, extracted text). The runtime contains GPL-2.0 software
(shadPS4 and derivatives); see `LICENSE`. *Kyoei Toshi* is a trademark of its owners.

---

**Português:** preservação, tradução e port para PC de *Kyoei Toshi*. Nada do jogo está aqui:
tudo funciona sobre o dump do seu próprio disco. O diário do projeto (`docs/DIARIO.md`) e as
instruções para quem trabalha nele (`AGENTS.md`) estão em português.
