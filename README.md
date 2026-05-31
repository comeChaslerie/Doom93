# Doom93

> Recode du **Doom** original de id Software (1993), construit sur le moteur
> **[Engine²](https://github.com/EngineSquared/EngineSquared)** (Engine Squared).

Doom93 est un projet d'apprentissage du C++ moderne (C++20) et du paradigme
**ECS** (Entity Component System). Le gameplay est reconstruit brique par brique
au-dessus d'Engine², qui fournit le `Core`, le registre d'entités (basé sur
[EnTT](https://github.com/skypjack/entt)) et l'ordonnanceur de systèmes.

<<<<<<< Updated upstream
'xmake'
=======
## Sommaire
>>>>>>> Stashed changes

- [Prérequis](#prérequis)
- [Installation](#installation)
- [Compilation & exécution](#compilation--exécution)
- [Tests & qualité](#tests--qualité)
- [Architecture](#architecture)
- [Structure du dépôt](#structure-du-dépôt)
- [Assets](#assets)

<<<<<<< Updated upstream
'xmake Doom93'
=======
## Prérequis

- [xmake](https://xmake.io/) (gestion du build et des dépendances)
- Un compilateur **C++20** (Clang ou GCC récent)
- **clang-format** et **clang-tidy** (≥ 22) pour la vérification du style et du linting
- Sur Linux : `libglu1-mesa-dev freeglut3-dev mesa-common-dev mesa-utils`

Les dépendances C++ (`entt`, `glm`, `spdlog`, `fmt`, `tinyobjloader`, `gtest`)
sont récupérées automatiquement par xmake.

## Installation

Le moteur Engine² est inclus en sous-module Git. Clonez le dépôt avec ses
sous-modules :

```sh
git clone --recurse-submodules https://github.com/comeChaslerie/Doom93.git
cd Doom93
```

Si le dépôt est déjà cloné sans les sous-modules :

```sh
git submodule update --init --recursive
```

## Compilation & exécution

```sh
xmake            # configure et compile le projet
xmake run Doom93 # lance le jeu
```

## Tests & qualité

Le projet est testé via **GoogleTest**, chaque source de `src/` ayant sa cible
de test miroir dans `tests/`.

```sh
xmake test                  # exécute toute la suite de tests
xmake format -c             # vérifie le formatage (clang-format)
xmake check clang.tidy      # analyse statique (clang-tidy)
xmake check_leaks           # détection de fuites mémoire (macOS)
```

La CI GitHub Actions ([`.github/workflows/ci.yml`](.github/workflows/ci.yml))
applique sur Linux et macOS : vérification de propreté du dépôt, `clang-format`,
`clang-tidy`, build multi-plateforme, tests, couverture (gcovr) et détection de
fuites mémoire.

## Architecture

Le code suit une organisation **ECS** stricte :

- **Components** (`src/game/component/`) — données pures attachées aux entités :
  `Health`, `Armor`, `Stat`, `Ammo`, `Damage`, `TextureHandle`…
- **Systems** (`src/game/system/`) — logique sans état opérant sur les
  composants : `ComputeDamage`, `ApplyDamage`, `AmmoInventory`.

Les systèmes sont enregistrés sur l'ordonnanceur d'Engine²
(`Startup`, `Update`) depuis [`src/main.cpp`](src/main.cpp).

## Structure du dépôt

```
.
├── src/
│   ├── main.cpp              # point d'entrée : setup du Core et des systèmes
│   └── game/
│       ├── component/        # composants ECS (données)
│       └── system/           # systèmes ECS (logique)
├── tests/                    # tests GoogleTest (miroir de src/)
├── assets/                   # ressources du jeu (voir ci-dessous)
├── EngineSquared/            # sous-module : le moteur Engine²
├── xmake.lua                 # définition du build et des cibles de test
└── .github/workflows/        # pipelines CI
```

## Assets

Les ressources sont organisées par catégorie dans `assets/` :
`audio/`, `engine/`, `entities/`, `environment/`, `levels/`, `ui/`.

Elles proviennent du projet libre **[Freedoom](https://freedoom.github.io/)** —
voir [`assets/FREEDOOM-CREDITS.txt`](assets/FREEDOOM-CREDITS.txt) et
[`assets/FREEDOOM-COPYING.txt`](assets/FREEDOOM-COPYING.txt).
</content>
</invoke>
>>>>>>> Stashed changes
