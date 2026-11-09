# Kyoei Toshi — City Shrouded in Shadow

Preservation, fan translation and a PC port of **巨影都市 / Kyoei Toshi** (*City Shrouded in
Shadow*, PS4, Bandai Namco / Granzella, 2017), a game released only in Japan.

The project ships **no game data**. Everything here works on a dump of a disc you own; the
repository holds only tools, the runtime, translations and documentation.

## What it does

- **PC port (Windows, Linux through Wine):** a loader for the game's PS4 executable, a runtime
  for the system libraries, and a Vulkan renderer (the GoW3 PC core, derived from shadPS4).
- **English and Brazilian Portuguese:** read at run time over the original archive (a small
  delta, no modified copy of the 6 GB archive). Portuguese includes localized interface
  images, fixed menu strings and the missing intermission-font accents. The text has received an editorial review; full-campaign gameplay testing remains incomplete.
- **PC features:** keyboard and mouse; PlayStation, Xbox and Nintendo controllers with their
  own button prompts and controls diagrams; touch pad gestures with alternatives for pads
  without one; photo mode; game-styled save/load/name screens; graphics options (anisotropic
  filtering, AMD RCAS sharpening, AMD FSR 1). The PS4 look is the default.

Status: playable, experimental. The English and Portuguese text have received an editorial review. The frame rate is limited
by the processor (about 50–60 FPS standing, 35–45 turning the camera on a mid-range PC).
The 1.0 preparation checklist is in [docs/RELEASE-1.0.md](docs/RELEASE-1.0.md).
The public candidate builder and its current limits are documented below.

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

Download and extract the **Source ZIP from the release** to build. A Git clone alone omits the fixed third-party snapshots and dependency sources. The source kit includes them, recorded with their hashes:

- `third_party/gow3-gpu` — `gpu/` of [gow3_pc](https://github.com/FelipePrado0/gow3_pc), tag
  `windows-v2.4.1` (`docs/gow3-gpu.json`);
- `third_party/godzilla-core` — the runtime of the companion Godzilla PS4 port, commit
  `e976b59` (`docs/godzilla-core.json`, `tools/snapshot_godzilla.py`);
- `third_party/imgui` — Dear ImGui.

```
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build --target kyoei -j 4
```

This builds `build/kyoei.exe` and the unit tests (`build/test_kyoei_*.exe`, `tools/test_*.py`).

## Using it with your own dump

1. Download the Windows ZIP from the release and extract it completely.
2. Open `City Shrouded in Shadow.exe` and select **Prepare game**.
3. Provide your own CUSA03562 version 01.03 disc dump and your own PS4 system modules.
4. Choose English, Brazilian Portuguese or Japanese. Keep `data/` when updating: it holds your saves and settings.

See [installation instructions](docs/release/INSTALL.txt) and
[source build instructions](docs/release/SOURCE-BUILD.md).

## Legal

This is a non-commercial fan project. It contains no copyrighted game files; do not publish
derived game data (archives, textures, extracted text). The runtime contains GPL-2.0 software
(shadPS4 and derivatives); see `LICENSE`. *Kyoei Toshi* is a trademark of its owners.

---

**Português:** preservação, tradução e port para PC de *Kyoei Toshi*. Nada do jogo está aqui:
tudo funciona sobre o dump do seu próprio disco. O diário do projeto (`docs/DIARIO.md`) e as
instruções para quem trabalha nele (`AGENTS.md`) estão em português.

## Public review candidate

`tools/build_public_candidate.py build/Public-Candidate --dependency-cache <dependency-source-cache>`
assembles the launcher, runtime, isolated Python/Pillow/NumPy, OFL fonts,
translation JSON and preparation tools. It never copies the private game installation,
game archives, firmware, saves or generated language patches into the ZIP.
The first run asks for the user's own CUSA03562 v01.03 dump and PS4 modules.

The builders create unapproved candidates by default. Release validation must separately
check the source build, bundled installation tools, licenses and absence of game data.
See [installation instructions](docs/release/INSTALL.txt),
[dependency audit](docs/release/DEPENDENCIES.md) and the manifests shipped with each ZIP.
Private updaters and private portable folders must never be used as public downloads.
