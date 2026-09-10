# Doom93

[![CI](https://github.com/comeChaslerie/Doom93/actions/workflows/ci.yml/badge.svg)](https://github.com/comeChaslerie/Doom93/actions/workflows/ci.yml)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Build system: xmake](https://img.shields.io/badge/build-xmake-brightgreen.svg)](https://xmake.io/)

> A recode of id Software's original **Doom** (1993), built on top of the
> **[Engine²](https://github.com/EngineSquared/EngineSquared)** (Engine Squared) engine.

Doom93 is a learning project focused on modern C++ (C++20) and the **ECS**
(Entity Component System) paradigm. The gameplay is rebuilt piece by piece on top
of Engine², which provides the `Core`, the entity registry (powered by
[EnTT](https://github.com/skypjack/entt)) and the system scheduler.

## Table of contents

- [Requirements](#requirements)
- [Installation](#installation)
- [Build & run](#build--run)
- [Testing & quality](#testing--quality)
- [Architecture](#architecture)
- [Repository layout](#repository-layout)
- [Assets](#assets)

## Requirements

- [xmake](https://xmake.io/) (build system and dependency management)
- A **C++20** compiler (recent Clang or GCC)
- **clang-format** and **clang-tidy** (≥ 22) for style checking and linting
- On Linux: `libglu1-mesa-dev freeglut3-dev mesa-common-dev mesa-utils`

C++ dependencies (`entt`, `glm`, `spdlog`, `fmt`, `tinyobjloader`, `gtest`) are
fetched automatically by xmake.

## Installation

The Engine² engine is included as a Git submodule. Clone the repository together
with its submodules:

```sh
git clone --recurse-submodules git@github.com:comeChaslerie/Doom93.git
cd Doom93
```

If the repository was already cloned without its submodules:

```sh
git submodule update --init --recursive
```

## Build & run

```sh
xmake            # configure and build the project
xmake run Doom93 # launch the game
```

## Testing & quality

The project is tested with **GoogleTest**: every source file in `src/` has a
mirrored test target in `tests/`.

```sh
xmake test                  # run the full test suite
xmake format -c             # check formatting (clang-format)
xmake check clang.tidy      # static analysis (clang-tidy)
xmake check_leaks           # memory leak detection (macOS)
```

The GitHub Actions CI ([`.github/workflows/ci.yml`](.github/workflows/ci.yml))
runs on both Linux and macOS: repository cleanliness check, `clang-format`,
`clang-tidy`, multi-platform build, tests, coverage (gcovr) and memory leak
detection.

## Architecture

The code follows a strict **ECS** organization:

- **Components** (`src/game/component/`) — pure data attached to entities:
  `Health`, `Armor`, `Stat`, `Ammo`, `Damage`, `TextureHandle`…
- **Systems** (`src/game/system/`) — stateless logic operating on components:
  `ComputeDamage`, `ApplyDamage`, `AmmoInventory`.

Systems are registered on Engine²'s scheduler (`Startup`, `Update`) from
[`src/main.cpp`](src/main.cpp).

## Repository layout

```
.
├── src/
│   ├── main.cpp              # entry point: Core and systems setup
│   └── game/
│       ├── component/        # ECS components (data)
│       └── system/           # ECS systems (logic)
├── tests/                    # GoogleTest suites (mirror of src/)
├── freedoom/                 # Freedoom WAD and licences (see below)
├── EngineSquared/            # submodule: the Engine² engine
├── xmake.lua                 # build definition and test targets
└── .github/workflows/        # CI pipelines
```

## Assets

Game data lives in [`freedoom/`](freedoom/) at the repository root:

```
freedoom/
├── freedoom1.wad             # levels, textures, flats, sprites and palettes
├── FREEDOOM-CREDITS.txt
└── FREEDOOM-COPYING.txt
```

Everything the game needs is inside the single WAD file: the loader
([`src/game/loader/`](src/game/loader/)) parses its lumps at startup, so there
are no separate texture or level files to manage.

The WAD is committed to the repository — `.gitignore` whitelists `freedoom/**`
— so there is no download step after cloning. It is opened by a path relative
to the project root in [`src/main.cpp`](src/main.cpp), which is why `xmake.lua`
sets `set_rundir("$(projectdir)")` on every target that touches it.

The data comes from the free **[Freedoom](https://freedoom.github.io/)** project
— see [`freedoom/FREEDOOM-CREDITS.txt`](freedoom/FREEDOOM-CREDITS.txt) and
[`freedoom/FREEDOOM-COPYING.txt`](freedoom/FREEDOOM-COPYING.txt).
