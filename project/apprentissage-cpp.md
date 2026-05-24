# Apprentissage C++ — piste guidée Doom93

> Objectif : passer d'un **bon niveau C** à un C++ moderne (C++20) **opérationnel**,
> en construisant à chaque étape un morceau **réel, modulaire et futureproof** du jeu
> Doom93, et toujours en pensant au moteur **EngineSquared** (ECS).
>
> Ce fichier est ton **guide de référence** : reprends-le au début de chaque session.
> Chaque exercice correspond à une issue du GitHub Project (track *Apprentissage C++*).
> Coche-les au fur et à mesure.

---

## 0. État d'esprit : du C au C++, et la philosophie ECS

Tu connais le C. Voici les bascules mentales à garder en tête tout du long.

| En C tu fais… | En C++ moderne tu fais plutôt… | Pourquoi |
| --- | --- | --- |
| `malloc`/`free` | RAII : objets qui se libèrent seuls, `std::unique_ptr` | zéro fuite, exception-safe |
| `char*` + `strcpy` | `std::string`, `std::string_view` | sûr, sans gestion manuelle |
| tableaux C `T arr[N]` | `std::array<T,N>`, `std::vector<T>` | taille connue, bornes, croissance |
| `#define` | `constexpr`, `enum class` | typé, scopé, vérifié à la compilation |
| `void*` générique | templates `<typename T>` | générique **et** typé |
| pointeurs partout | **références** `T&`, `const T&` | non-null, intention claire |
| structs + fonctions libres | structs + fonctions libres **(toujours valable en ECS !)** | voir ci-dessous |

### La règle d'or ECS (EngineSquared)

EngineSquared est un ECS façon Bevy au-dessus de `entt` :

- **Composant** = une `struct` de **données pures**, sans logique (`struct Health { float current; };`).
  → exactement comme une `struct` en C. **Tu sais déjà faire ça.**
- **Système** = un **callable** (fonction libre, lambda, ou classe avec `operator()`) qui prend
  `Engine::Core &core` et fait tourner la logique sur les composants.
- **Ressource** = une donnée **unique** partagée (ex: temps, config).
- **Plugin** = une classe qui regroupe et enregistre des systèmes/ressources (`Bind()`).

> ⚠️ Piège n°1 du débutant C++ venant de la POO classique : **ne mets pas la logique dans
> les composants**. Un composant qui a une méthode `takeDamage()` casse le modèle ECS.
> La logique vit dans les **systèmes**. On verra *quand* la POO (classes, héritage) est
> quand même utile (exos 4, 5, 8) — et quand l'éviter au profit de la composition.

### Cheat-sheet API moteur (à garder sous la main)

```cpp
#include "Engine.hpp"

// --- Composant : juste des données ---
struct Health { float current = 100.f; float max = 100.f; };

// --- Système : un callable(Core&) ---
void RegenSystem(Engine::Core &core) {
    auto &time = core.GetResource<Engine::Resource::Time>();      // delta time
    core.GetRegistry().view<Health>().each([&](Health &h) {
        h.current = std::min(h.max, h.current + 5.f * time._elapsedTime);
    });
}

int main() {
    Engine::Core core;

    // Enregistrer des systèmes sur un scheduler : Startup / Update / Shutdown
    core.RegisterSystem<Engine::Scheduler::Startup>([](Engine::Core &c) {
        auto e = c.CreateEntity();
        e.AddComponent<Health>();                  // ajout de composant
    });
    core.RegisterSystem<Engine::Scheduler::Update>(RegenSystem);

    core.Run();   // boucle jusqu'à core.Stop() ; RunSystems() = un seul tour
}
```

API entité utile : `e.AddComponent<T>(args...)`, `e.RemoveComponent<T>()`,
`e.HasComponents<A,B>()`, `e.GetComponents<T>()`, `e.TryGetComponent<T>()`, `e.Kill()`.
API core utile : `core.CreateEntity()`, `core.KillEntity(id)`, `core.GetResource<T>()`,
`core.RegisterResource(T{...})`, `core.AddPlugins<MonPlugin>()`, `core.Stop()`.
Schedulers : `Startup`, `Update`, `Shutdown`, `FixedTimeUpdate`, `RelativeTimeUpdate`.

> 📌 Les signatures exactes sont dans `EngineSquared/src/engine/src/` (`core/Core.hpp`,
> `entity/Entity.hpp`, `plugin/APlugin.hpp`). Vérifie-les si un appel ne compile pas.

---

## Progression des exercices

Phase 1 (exos 1–3) : bases du C++ en restant proche du C.
Phase 2 (exos 4–12) : classes, RAII, généricité, polymorphisme, puis un capstone ECS complet.

| # | Titre | Concept C++ central | Livrable jeu |
| - | --- | --- | --- |
| 1 | Composant `Health` + système `ApplyDamage` | namespaces, `enum class`, références | base vie/dégâts |
| 2 | Dégâts multi-sources | `const`-correctness, surcharge, args par défaut | dégâts typés |
| 3 | Inventaire de munitions | conteneurs STL, `range-for`, `std::optional` | stock de munitions |
| 4 | Classe `Stat` bornée | classes, encapsulation, invariants | vie/armure robustes |
| 5 | Possession de ressource | RAII, `unique_ptr`, move, règle de 0/5 | handle texture/son |
| 6 | `Cooldown` / value type | surcharge d'opérateurs, types-valeur | cadence de tir |
| 7 | `Stat<T>` générique / pool | templates, contraintes | briques réutilisables |
| 8 | Plugin custom + interface | héritage, `virtual`, abstrait vs composition | packaging de features |
| 9 | Systèmes lambda + events | lambdas, `std::function`, pub/sub | découplage par events |
| 10 | Machine à états ennemi | `enum class`, `std::variant`, `switch` | IA d'ennemi |
| 11 | Gestion d'erreurs | exceptions vs valeurs de retour | chargement robuste |
| 12 | **Capstone** : pipeline Vie/Dégâts/Mort | tout assembler en un plugin | feature complète |

---

## Exercice 1 — Composant `Health` + système `ApplyDamage`

**Objectif C++** : `namespace`, `enum class`, **références** (`T&`) vs pointeurs, `auto`,
initialisation par défaut des membres, `fmt`/log au lieu de `printf`.

**Lien avec le C** : une `struct Health` est identique à ce que tu écrirais en C.
La nouveauté : pas de typedef, les membres ont des **valeurs par défaut**, et la "fonction
qui agit dessus" prend une **référence** `Health&` (pas un `Health*` qu'on déréférence).

**Lien moteur/ECS** :
- `Health` est un **composant** (data only).
- `ApplyDamage` est un **système** (fonction libre `void(Engine::Core&)`).
- `DamageType` est un `enum class` (futureproof : on étendra les types sans casser le reste).

**Étapes** :
1. Crée `struct Health { float current = 100.f; float max = 100.f; };` dans `namespace game::component`.
2. Crée `enum class DamageType { Hitscan, Projectile, Explosion, Environment };`.
3. Crée un composant `DamageEvent { float amount; DamageType type; }` (un "ordre" de dégât posé sur une entité).
4. Système `ApplyDamage(core)` : parcourt les entités qui ont `Health` **et** `DamageEvent`,
   soustrait, clamp à 0, retire le `DamageEvent` (composant temporaire / one-shot).
5. Dans `Startup`, crée une entité avec `Health` ; dans un autre système, pose un `DamageEvent`.

**Critères "futureproof"** :
- Aucune logique dans `Health` (data only).
- `DamageType` extensible sans toucher au système.
- `ApplyDamage` ne suppose **rien** sur *qui* a infligé les dégâts.

**Pour aller plus loin** : compare ce design avec comment tu l'aurais fait en C (struct +
fonction `apply_damage(Health*, int)`). Qu'est-ce que l'ECS t'apporte ? (réponse : le
*matching* automatique des entités via `view<Health, DamageEvent>`).

---

## Exercice 2 — Dégâts multi-sources (const-correctness & surcharge)

**Objectif C++** : `const`-correctness (`const T&` pour lecture seule), **surcharge de
fonctions**, **arguments par défaut**, `[[nodiscard]]`.

**Lien avec le C** : en C, pas de surcharge → tu nommerais `apply_damage_falloff`,
`apply_damage_flat`… En C++ une seule fonction `ComputeDamage(...)` avec plusieurs signatures.

**Lien moteur/ECS** : les fonctions de calcul de dégâts restent des **fonctions libres pures**
(faciles à tester sans le moteur), appelées **depuis** le système `ApplyDamage`.

**Étapes** :
1. `float ComputeDamage(float base, DamageType type);` (modificateur selon le type).
2. Surcharge `float ComputeDamage(float base, DamageType type, float distance, float falloff = 1.f);`
   (atténuation pour explosions/projectiles).
3. Rends-les `[[nodiscard]]` et prends les params lourds en `const&` quand pertinent.
4. Marque tout ce qui ne mute rien en `const` (paramètres, méthodes plus tard).

**Critères "futureproof"** : le calcul est **pur** (pas d'accès au `Core`), donc testable à
l'unité. Le système ne fait qu'orchestrer.

**Pour aller plus loin** : écris un test xmake qui appelle `ComputeDamage` avec plusieurs
types et vérifie les bornes.

---

## Exercice 3 — Inventaire de munitions (conteneurs STL)

**Objectif C++** : `std::unordered_map`, `std::vector`, `std::array`, **range-based for**,
`std::optional`, `auto`, structured bindings (`auto& [k, v] : map`).

**Lien avec le C** : remplace tableaux + index manuels + recherche linéaire par des
conteneurs standard sûrs et expressifs.

**Lien moteur/ECS** : `Ammo` est un **composant** porté par le joueur :
`struct Ammo { std::unordered_map<AmmoType, int> stock; };`.

**Étapes** :
1. `enum class AmmoType { Bullets, Shells, Cells, Rockets };`.
2. Composant `Ammo` avec le `unordered_map` ci-dessus.
3. Fonctions libres : `bool TryConsume(Ammo&, AmmoType, int n)` (renvoie false si insuffisant),
   `void Add(Ammo&, AmmoType, int n)`, `std::optional<int> Peek(const Ammo&, AmmoType)`.
4. Parcours avec `for (auto& [type, count] : ammo.stock)`.

**Critères "futureproof"** : ajouter un type de munition = une entrée d'enum, rien d'autre.
`TryConsume` ne descend jamais sous zéro.

**Pour aller plus loin** : remplace l'`unordered_map` par un `std::array<int, N>` indexé par
`static_cast<size_t>(type)` et compare ergonomie/perf. Quand préférer l'un ou l'autre ?

---

## Exercice 4 — Classe `Stat` bornée (classes & encapsulation)

**Objectif C++** : LE gros morceau. `class`, `public`/`private`, **constructeurs** + liste
d'initialisation des membres, **invariants**, `this`, accesseurs `const`.

**Lien avec le C** : en C, une struct `{cur, max}` n'empêche personne de mettre `cur > max`
ou `cur < 0`. Une **classe** protège l'invariant : `0 <= current <= max` est *toujours* vrai.

**Lien moteur/ECS** : nuance importante. Un composant doit rester "données". Mais une **petite
classe-valeur** comme `Stat` (qui encapsule un invariant) est parfaitement OK *à l'intérieur*
d'un composant : `struct Health { Stat hp; };`. Tu apprends la POO sans violer l'ECS.

**Étapes** :
1. `class Stat { float _current; float _max; public: ... };`.
2. Constructeur `explicit Stat(float max)` → `_current = _max = max`.
3. Méthodes : `void Damage(float)`, `void Heal(float)`, `void SetMax(float, bool refill=false)`,
   accesseurs `float Current() const`, `float Max() const`, `float Ratio() const`, `bool IsEmpty() const`.
4. **Clamp systématiquement** dans chaque mutateur (privé helper `Clamp()`).
5. Réutilise `Stat` pour `Health` **et** `Armor`.

**Critères "futureproof"** : impossible de créer un état invalide depuis l'extérieur. Une
seule classe sert vie, armure, et plus tard bouclier/énergie.

**Pour aller plus loin** : ajoute des tests qui tentent de violer l'invariant (heal négatif,
damage > max) et vérifient qu'il tient. Discussion : pourquoi `explicit` sur le constructeur ?

---

## Exercice 5 — Possession de ressource (RAII & smart pointers)

**Objectif C++** : **RAII**, `std::unique_ptr`, `std::shared_ptr`, **sémantique de move**
(`std::move`), **règle de 0 / 3 / 5**, destructeurs.

**Lien avec le C** : remplace `malloc`/`free` (ou `fopen`/`fclose`) par un objet qui libère
**automatiquement** dans son destructeur. Plus jamais de `free` oublié.

**Lien moteur/ECS** : un `TextureHandle` / `SoundHandle` qui possède une ressource GPU/fichier.
Stocké dans une **ressource moteur** ou un composant. C'est le bon endroit pour comprendre la
propriété *unique* (un seul owner) vs *partagée*.

**Étapes** :
1. `class TextureHandle` qui "possède" un id de ressource (simule l'alloc/free par des logs).
2. Implémente la **règle de 5** : interdis la copie (`= delete`), autorise le move
   (`TextureHandle(TextureHandle&&) noexcept`), libère dans `~TextureHandle()`.
3. Variante : `std::shared_ptr` pour une texture partagée par plusieurs entités.
4. Montre `std::move` lors d'un transfert de propriété.

**Critères "futureproof"** : aucune fuite possible, propriété explicite, exception-safe.

**Pour aller plus loin** : explique pourquoi viser la **règle de 0** (composer avec des types
qui gèrent déjà leur ressource → tu n'écris aucun destructeur). Quand `unique` vs `shared` ?

---

## Exercice 6 — `Cooldown` / value type (surcharge d'opérateurs)

**Objectif C++** : **types-valeur** légers, **surcharge d'opérateurs**, `constexpr`,
`friend`, comparaisons (`operator<=>` C++20 facultatif).

**Lien avec le C** : tu manipulais des `float seconds` partout. Ici un petit type dédié
empêche de mélanger "secondes restantes" et "durée totale".

**Lien moteur/ECS** : composant `Cooldown` mis à jour chaque frame avec
`time._elapsedTime`. Sert à la cadence de tir, aux spawns, aux portes.

**Étapes** :
1. `struct Cooldown { float remaining = 0.f; float duration = 0.f; };`.
2. Méthodes : `bool Ready() const`, `void Trigger()` (remaining = duration), `void Tick(float dt)`.
3. (Optionnel) un value type `Seconds`/`Damage` qui surcharge `+`, `-`, `<`, pour éviter les
   confusions d'unités.
4. Système `CooldownSystem(core)` qui `Tick()` tous les `Cooldown` avec le delta time.

**Critères "futureproof"** : un seul composant `Cooldown` réutilisable pour tout ce qui a une
cadence. Pas de magie de unités.

**Pour aller plus loin** : à quoi sert `constexpr` ici ? Que change `operator<=>` ?

---

## Exercice 7 — `Stat<T>` générique et `Pool<T>` (templates)

**Objectif C++** : **templates** de classe/fonction, paramètres de type, `concept`/contraintes
C++20, déduction, instanciation.

**Lien avec le C** : remplace les macros et le `void*` générique par du code générique
**typé et vérifié**.

**Lien moteur/ECS** : le moteur lui-même est massivement templaté (`AddComponent<T>`,
`view<A,B>`, `GetResource<T>`). Comprendre les templates = comprendre le moteur.

**Étapes** :
1. Généralise `Stat` (exo 4) en `template <typename T> class Stat` (entiers ou flottants).
2. Ajoute une **contrainte** : `template <std::floating_point T>` ou un `concept` maison
   `Numeric`.
3. Écris un `template <typename T> class Pool` (réserve/relâche des objets réutilisables —
   utile pour balles/projectiles).

**Critères "futureproof"** : `Stat<float>`, `Stat<int>` sans dupliquer le code. `Pool<Bullet>`
réutilisable pour tout objet recyclable.

**Pour aller plus loin** : où le moteur utilise-t-il des `concept` (cherche `CScheduler`,
`CPlugin` dans `Core.hpp`) ? Recrée un mini-concept équivalent.

---

## Exercice 8 — Plugin custom + interface (héritage & polymorphisme)

**Objectif C++** : **héritage**, fonctions **`virtual`**, classe **abstraite** (méthode
virtuelle pure `= 0`), surcharge `override`, destructeur virtuel. **Et surtout** : quand
préférer la **composition (ECS)** à l'héritage.

**Lien avec le C** : le C n'a pas de virtual ; tu simulais avec des tables de pointeurs de
fonctions. Ici c'est natif.

**Lien moteur/ECS** : le moteur **utilise** l'héritage à un seul endroit clé : les **plugins**
(`Engine::APlugin` → `Bind()` virtuelle pure). C'est l'exemple parfait du bon usage du virtual :
un point d'extension. À l'inverse, **n'utilise pas** d'héritage pour les entités/monstres →
ça, c'est de la composition de composants.

**Étapes** :
1. Crée `class CombatPlugin : public Engine::APlugin` avec `using Engine::APlugin::APlugin;`.
2. Implémente `void Bind() override` : enregistre `CooldownSystem`, `ApplyDamage`, etc.
3. Enregistre-le : `core.AddPlugins<CombatPlugin>();`.
4. Écris une 2ᵉ interface abstraite *à toi* (ex: `IDamageModifier { virtual float Modify(float) const = 0; };`)
   et 2 implémentations, pour sentir le polymorphisme.

**Critères "futureproof"** : tes systèmes de combat sont packagés et réutilisables en une
ligne (`AddPlugins`). Tu sais distinguer "extension via virtual" de "variation via composants".

**Pour aller plus loin** : rédige 3 lignes : *pourquoi un monstre ≠ une classe `Monster`
héritée, mais un assemblage de composants ?* (réponse : flexibilité, pas d'explosion de
hiérarchie, data-oriented).

---

## Exercice 9 — Systèmes lambda + events (lambdas & std::function)

**Objectif C++** : **lambdas**, captures (`[&]`, `[=]`, `[this]`), `std::function`,
callbacks, `auto` en paramètre (lambdas génériques).

**Lien avec le C** : remplace les pointeurs de fonction + `void* userdata` par des lambdas
qui capturent leur contexte proprement.

**Lien moteur/ECS** : le moteur a un **plugin `event`** (pub/sub). Au lieu que `ApplyDamage`
appelle directement la mort, il **publie** un event `DeathEvent` ; d'autres systèmes y
réagissent (son, score, drop). Découplage total.

**Étapes** :
1. Réécris un système simple en **lambda** passée à `RegisterSystem`.
2. Étudie `EngineSquared/src/plugin/event/examples/` et reproduis un publish/subscribe.
3. Publie un `DeathEvent{entityId}` depuis le système de mort ; abonne un système qui logge.

**Critères "futureproof"** : ajouter une réaction à la mort (drop, son, score) = un nouvel
abonné, **sans toucher** au système qui détecte la mort.

**Pour aller plus loin** : différence entre capturer par référence `[&]` (danger si la lambda
survit au scope) et par valeur `[=]` ? Pourquoi c'est crucial pour les systèmes ?

---

## Exercice 10 — Machine à états ennemi (enum class / variant / optional)

**Objectif C++** : `enum class` + `switch`, `std::optional`, `std::variant` + `std::visit`,
`std::array` de transitions.

**Lien avec le C** : tu connais les FSM en C avec des `enum` + `switch`. Ici on monte en sûreté
(enum scopé, pas de cast implicite) et on découvre `variant`/`optional`.

**Lien moteur/ECS** : Doom modélise les monstres par des **états** (idle, chase, attack, pain,
death). Composant `AIState { EnemyState state; std::optional<EntityId> target; }`.
Système `EnemyAISystem` qui fait les transitions.

**Étapes** :
1. `enum class EnemyState { Idle, Chase, Attack, Pain, Dead };`.
2. Composant `AIState` avec l'état courant + `std::optional<EntityId> target`.
3. `EnemyAISystem(core)` : `switch (state)` → décide la transition (voit le joueur ? distance ?).
4. (Optionnel) modélise l'**action** courante avec `std::variant` et `std::visit`.

**Critères "futureproof"** : ajouter un état = une entrée d'enum + un case. `optional`
exprime proprement "pas de cible".

**Pour aller plus loin** : compare `enum class + switch` vs `std::variant + visit`. Quand
chacun est-il plus clair / plus sûr ?

---

## Exercice 11 — Gestion d'erreurs (exceptions vs valeurs de retour)

**Objectif C++** : `try`/`catch`, `throw`, hiérarchie d'exceptions, `std::optional` /
`std::expected`-like comme alternative, `noexcept`, RAII garantit la libération.

**Lien avec le C** : remplace les codes de retour `-1`/`errno` par des exceptions là où
c'est pertinent — **et savoir quand NE PAS** les utiliser (chemins chauds, erreurs prévues).

**Lien moteur/ECS** : le moteur **lève** déjà des exceptions, ex. `MissingResourceError`,
`MissingSchedulerError` (`EngineSquared/src/engine/src/exception/`). Apprends à les attraper
au chargement (WAD/textures manquantes) et à choisir exception vs `optional`.

**Étapes** :
1. Écris un loader (`LoadWadOrThrow`) qui lève si le fichier est absent/corrompu.
2. Attrape-le dans `main`/Startup et logge proprement, sans crash brutal.
3. Pour un cas "normalement échouable" (munition insuffisante), utilise un **retour
   bool/optional** plutôt qu'une exception. Justifie le choix.

**Critères "futureproof"** : erreurs d'init = exceptions (rares, fatales) ; erreurs de gameplay
= valeurs de retour (fréquentes, prévues). Aucune ressource fuit grâce au RAII.

**Pour aller plus loin** : pourquoi `noexcept` sur les move constructors (cf. exo 5) aide les
conteneurs STL ?

---

## Exercice 12 — Capstone : pipeline Vie/Dégâts/Mort complet (tout assembler)

**Objectif C++** : combiner **tout** : composants (struct), classe-valeur (`Stat`), enum class,
templates, plugin (héritage), lambdas + events, gestion d'erreurs. C'est ta **première vraie
feature de jeu** propre et autonome.

**Lien moteur/ECS** : un seul `HealthPlugin` (ou `CombatPlugin`) qui, via `Bind()`, enregistre :
- `Health { Stat hp; }`, `Armor { Stat ap; }`, `DamageEvent`, `Dead` (tag).
- Système `ApplyDamage` (armure absorbe, puis vie).
- Système `DeathSystem` : si `hp.IsEmpty()`, ajoute un tag `Dead` et **publie** `DeathEvent`.
- Système `CleanupDead` : réagit à `Dead` (drop/son/score) puis `Kill()` l'entité.

**Étapes** :
1. Rassemble les composants des exos 1–4 et 6.
2. Crée `class HealthPlugin : public Engine::APlugin`, implémente `Bind()`.
3. Câble les events (exo 9) pour la mort.
4. Teste : une entité subit des dégâts répétés → armure descend → vie descend → meurt → event.
5. Active : `core.AddPlugins<HealthPlugin>();` dans `main`.

**Critères "futureproof"** :
- Le pipeline marche pour **le joueur ET les ennemis** sans code spécifique.
- Brancher une nouvelle réaction à la mort = un abonné de plus.
- Tout est testable (les calculs sont des fonctions pures ; les systèmes sont isolés).

**Pour aller plus loin** : c'est cette feature qui sert de **patron** pour toutes les
suivantes (armes, pickups, IA). Relis-la avant d'attaquer le track *Gameplay*.

---

## Après le capstone

Tu as les bases pour attaquer le track **Gameplay** du board (mouvement joueur, armes,
ennemis, pickups…). Les composants/classes/value-types construits ici (`Stat`, `Cooldown`,
`Ammo`, `DamageEvent`, events) y sont **directement réutilisés** — c'était le but : apprendre
le C++ **en** construisant le socle réel du jeu.

Garde toujours en tête : **données dans les composants, logique dans les systèmes, extension
par plugins, découplage par events.**
