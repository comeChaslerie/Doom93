#pragma once

#include <array>
#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <vector>

namespace game::loader {

// ============================================================================
// A. Geometrie de niveau
// Coordonnees stockees en glm::ivec2 (int32) : conversion sans perte depuis int16
// disque, et directement compatible Transform/moteur.
// Les INDICES (startVertex, frontSidedef...) restent en int32_t — semantique
// differente des coordonnees, pas de vecteur.
// ============================================================================

// THINGS (10 octets) : monstres, objets, points de spawn.
struct Thing {
    glm::ivec2 position{0, 0}; // coordonnees monde
    std::int32_t angle = 0;    // orientation en degres (0=Est)
    std::int32_t type = 0;     // doomednum (3004=zombie, 2001=fusil...)
    std::int32_t flags = 0;    // skills, deaf/ambush, multi-only
};

// LINEDEFS (14 octets) : les lignes/murs.
struct Linedef {
    std::int32_t startVertex = 0;  // INDICE dans VERTEXES
    std::int32_t endVertex = 0;    // INDICE dans VERTEXES
    std::int32_t flags = 0;        // blocking, two-sided, secret...
    std::int32_t special = 0;      // action declenchee (porte, ascenseur)
    std::int32_t tag = 0;          // lie aux secteurs de meme tag
    std::int32_t frontSidedef = 0; // INDICE dans SIDEDEFS
    std::int32_t backSidedef = 0;  // INDICE dans SIDEDEFS, 0xFFFF = aucun
};

// SIDEDEFS (30 octets) : textures + offsets d'un cote de linedef.
struct Sidedef {
    glm::ivec2 offset{0, 0}; // (xOffset, yOffset) : decalage de texture
    std::string upper;       // char[8] : texture haute
    std::string lower;       // char[8] : texture basse
    std::string middle;      // char[8] : texture mediane
    std::int32_t sector = 0; // INDICE dans SECTORS
};

// SECTORS (26 octets) : sol/plafond d'une zone.
struct Sector {
    std::int32_t floorHeight = 0;
    std::int32_t ceilingHeight = 0;
    std::string floorTexture;    // char[8] : flat de sol
    std::string ceilingTexture;  // char[8] : flat de plafond
    std::int32_t lightLevel = 0; // 0-255
    std::int32_t special = 0;    // lumiere clignotante, degats...
    std::int32_t tag = 0;        // lie au tag des linedefs
};

// ============================================================================
// B. BSP (genere, lecture seule pour le rendu)
// ============================================================================

// SEGS (12 octets) : segment de ligne issu du BSP.
struct Seg {
    std::int32_t startVertex = 0; // INDICE dans VERTEXES
    std::int32_t endVertex = 0;   // INDICE dans VERTEXES
    std::int32_t angle = 0;       // angle binaire (BAM)
    std::int32_t linedef = 0;     // INDICE dans LINEDEFS (ligne parente)
    std::int32_t direction = 0;   // 0 = meme sens que le linedef, 1 = oppose
    std::int32_t offset = 0;      // distance le long du linedef jusqu'au start
};

// SSECTORS (4 octets) : sous-secteur (feuille du BSP).
struct SubSector {
    std::int32_t segCount = 0; // nombre de segs
    std::int32_t firstSeg = 0; // INDICE du premier seg
};

// Boite englobante : reutilisee 2x dans Node.
// glm::ivec2 min = (left, bottom), max = (right, top) — conventions Doom (Y vers le haut).
struct BBox {
    glm::ivec2 min{0, 0}; // (left, bottom)
    glm::ivec2 max{0, 0}; // (right, top)
};

// NODES (28 octets) : noeud de l'arbre BSP.
struct Node {
    glm::ivec2 partition{0, 0};   // origine de la ligne de partition
    glm::ivec2 direction{0, 0};   // vecteur (dx, dy) de la partition
    BBox rightBox;                // boite englobante du sous-arbre droit
    BBox leftBox;                 // boite englobante du sous-arbre gauche
    std::uint32_t rightChild = 0; // bit 15 = 1 -> sous-secteur, sinon noeud
    std::uint32_t leftChild = 0;  // idem
};

// ============================================================================
// C. Lumps speciaux
// ============================================================================

// REJECT : matrice de visibilite secteur x secteur, (numSectors^2) bits packes.
struct Reject {
    std::vector<std::uint8_t> bits; // acces par bit : sectorA * numSectors + sectorB
};

// BLOCKMAP : grille d'acceleration des collisions.
struct Blockmap {
    glm::ivec2 origin{0, 0}; // coin bas-gauche du quadrillage
    std::int32_t columns = 0;
    std::int32_t rows = 0;
    std::vector<std::uint16_t> offsets;                // columns * rows offsets
    std::vector<std::vector<std::int32_t>> blocklists; // indices linedef par bloc
};

// ============================================================================
// D. Definitions de textures murales
// ============================================================================

// PNAMES : liste des noms de patches references par les textures.
using Pnames = std::vector<std::string>; // count:int32 puis count x char[8]

// Un patch place dans une texture (mappatch_t, 10 octets).
struct TexturePatch {
    glm::ivec2 origin{0, 0};     // position du patch dans la texture composite
    std::int32_t patchIndex = 0; // INDICE dans PNAMES
    std::int32_t stepDir = 0;    // inutilise (legacy)
    std::int32_t colormap = 0;   // inutilise (legacy)
};

// TEXTURE1 / TEXTURE2 (maptexture_t, taille variable).
struct TextureDef {
    std::string name; // char[8]
    std::int32_t width = 0;
    std::int32_t height = 0;
    std::vector<TexturePatch> patches;
};

// ============================================================================
// E. Graphismes
// Pixels stockes en glm::u8vec4 (RGBA) : format natif du moteur (Graphic::Resource::Image).
// La conversion palette WAD (indices uint8) -> RGBA se fait au parsing via PLAYPAL.
// ============================================================================

// Format "picture" : sprites, patches muraux, menu, status bar... tous identiques.
struct Picture {
    std::string name; // nom du lump (ex. "TROOA1", "WALL03_1")
    std::int32_t width = 0;
    std::int32_t height = 0;
    glm::ivec2 offset{0, 0};         // point d'ancrage (leftOffset, topOffset)
    std::vector<glm::u8vec4> pixels; // RGBA, taille = width * height
};

// Flat (sol/plafond) : 64x64 pixels RGBA.
struct Flat {
    std::string name; // nom du lump (ex. "FLOOR5_1")
    static constexpr std::size_t WIDTH = 64;
    static constexpr std::size_t HEIGHT = 64;
    std::array<glm::u8vec4, WIDTH * HEIGHT> pixels{};
};

// ============================================================================
// F. Couleurs
// ============================================================================

// PLAYPAL : 14 palettes de 256 couleurs RGB disque.
// Stockees en glm::u8vec4 (alpha = 255) pour etre directement utilisables
// comme LUT de conversion index -> RGBA.
struct Palette {
    std::array<glm::u8vec4, 256> colors{}; // RGBA (alpha fixe a 255 au parsing)
};

// COLORMAP : 34 tables d'assombrissement (index -> index palette remapped).
struct Colormap {
    std::array<std::uint8_t, 256> map{};
};

// ============================================================================
// Agregat : un niveau complet charge en memoire (familles A + B + C)
// ============================================================================

struct Level {
    std::string name;                 // "E1M1", "MAP01"...
    std::vector<glm::ivec2> vertexes; // VERTEXES : coordonnees des sommets
    std::vector<Thing> things;
    std::vector<Linedef> linedefs;
    std::vector<Sidedef> sidedefs;
    std::vector<Seg> segs;
    std::vector<SubSector> subsectors;
    std::vector<Node> nodes;
    std::vector<Sector> sectors;
    Reject reject;
    Blockmap blockmap;
};

// ============================================================================
// Agregat de tous les assets WAD parses, retourne par WadLoader.
// WadInfo reste interne au loader (metadonnees + annuaire brut).
// ============================================================================

struct LumpData {
    std::vector<Level> levels;        // niveaux (E?M? / MAP??)
    std::vector<Palette> palettes;    // PLAYPAL  (14 palettes en general)
    std::vector<Colormap> colormaps;  // COLORMAP (34 tables en general)
    Pnames pnames;                    // PNAMES : noms des patches references
    std::vector<TextureDef> textures; // TEXTURE1 / TEXTURE2
    std::vector<Flat> flats;          // F_START ... F_END
    std::vector<Picture> pictures;    // S_START / P_START ... sprites + patches
};

} // namespace game::loader
