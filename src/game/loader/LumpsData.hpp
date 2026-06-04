#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace game::loader {

// ============================================================================
// A. Geometrie de niveau
// ============================================================================

// Paire de coordonnees (x, y) : le lump VERTEXES, et reutilise pour toute vraie
// paire de coordonnees (position d'un Thing, offset d'un Sidedef).
struct Vertex {
    std::int16_t x = 0;
    std::int16_t y = 0;
};

// THINGS (10 octets) : monstres, objets, points de spawn.
struct Thing {
    Vertex position;        // coordonnees monde (vraie paire x,y)
    std::int16_t angle = 0; // orientation en degres (0=Est)
    std::int16_t type = 0;  // doomednum (3004=zombie, 2001=fusil...)
    std::int16_t flags = 0; // skills, deaf/ambush, multi-only
};

// LINEDEFS (14 octets) : les lignes/murs.
struct Linedef {
    std::int16_t startVertex = 0;  // INDICE dans VERTEXES
    std::int16_t endVertex = 0;    // INDICE dans VERTEXES
    std::int16_t flags = 0;        // blocking, two-sided, secret...
    std::int16_t special = 0;      // action declenchee (porte, ascenseur)
    std::int16_t tag = 0;          // lie aux secteurs de meme tag
    std::int16_t frontSidedef = 0; // INDICE dans SIDEDEFS
    std::int16_t backSidedef = 0;  // INDICE dans SIDEDEFS, 0xFFFF = aucun
};

// SIDEDEFS (30 octets) : textures + offsets d'un cote de linedef.
struct Sidedef {
    Vertex offset;           // (xOffset, yOffset) : vraie paire de decalage
    std::string upper;       // char[8] : texture haute
    std::string lower;       // char[8] : texture basse
    std::string middle;      // char[8] : texture mediane
    std::int16_t sector = 0; // INDICE dans SECTORS
};

// SECTORS (26 octets) : sol/plafond d'une zone.
struct Sector {
    std::int16_t floorHeight = 0;
    std::int16_t ceilingHeight = 0;
    std::string floorTexture;    // char[8] : flat de sol
    std::string ceilingTexture;  // char[8] : flat de plafond
    std::int16_t lightLevel = 0; // 0-255
    std::int16_t special = 0;    // lumiere clignotante, degats...
    std::int16_t tag = 0;        // lie au tag des linedefs
};

// ============================================================================
// B. BSP (genere, lecture seule pour le rendu)
// ============================================================================

// SEGS (12 octets) : segment de ligne issu du BSP.
struct Seg {
    std::int16_t startVertex = 0; // INDICE dans VERTEXES
    std::int16_t endVertex = 0;   // INDICE dans VERTEXES
    std::int16_t angle = 0;       // angle binaire (BAM)
    std::int16_t linedef = 0;     // INDICE dans LINEDEFS (ligne parente)
    std::int16_t direction = 0;   // 0 = meme sens que le linedef, 1 = oppose
    std::int16_t offset = 0;      // distance le long du linedef jusqu'au start
};

// SSECTORS (4 octets) : sous-secteur (feuille du BSP).
struct SubSector {
    std::int16_t segCount = 0; // nombre de segs
    std::int16_t firstSeg = 0; // INDICE du premier seg
};

// Boite englobante (8 octets) : reutilisee 2x dans Node.
struct BBox {
    std::int16_t top = 0;
    std::int16_t bottom = 0;
    std::int16_t left = 0;
    std::int16_t right = 0;
};

// NODES (28 octets) : noeud de l'arbre BSP.
struct Node {
    std::int16_t x = 0; // origine de la ligne de partition
    std::int16_t y = 0;
    std::int16_t dx = 0; // vecteur de la partition
    std::int16_t dy = 0;
    BBox rightBox;                // boite du sous-arbre droit
    BBox leftBox;                 // boite du sous-arbre gauche
    std::uint16_t rightChild = 0; // bit 15 = 1 -> sous-secteur, sinon node
    std::uint16_t leftChild = 0;  // idem
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
    std::int16_t originX = 0;
    std::int16_t originY = 0;
    std::int16_t columns = 0;
    std::int16_t rows = 0;
    std::vector<std::uint16_t> offsets;                // columns * rows offsets
    std::vector<std::vector<std::int16_t>> blocklists; // indices linedef par bloc
};

// ============================================================================
// D. Definitions de textures murales
// ============================================================================

// PNAMES : liste des noms de patches references par les textures.
using Pnames = std::vector<std::string>; // count:int32 puis count x char[8]

// Un patch place dans une texture (mappatch_t, 10 octets).
struct TexturePatch {
    std::int16_t originX = 0;
    std::int16_t originY = 0;
    std::int16_t patchIndex = 0; // INDICE dans PNAMES
    std::int16_t stepDir = 0;    // inutilise (legacy)
    std::int16_t colormap = 0;   // inutilise (legacy)
};

// TEXTURE1 / TEXTURE2 (maptexture_t, taille variable).
struct TextureDef {
    std::string name; // char[8]
    std::int16_t width = 0;
    std::int16_t height = 0;
    std::vector<TexturePatch> patches;
};

// ============================================================================
// E. Graphismes
// ============================================================================

// Format "picture" : sprites, patches muraux, menu, status bar... tous identiques.
struct Picture {
    std::int16_t width = 0;
    std::int16_t height = 0;
    std::int16_t leftOffset = 0;      // point d'ancrage X a l'affichage
    std::int16_t topOffset = 0;       // point d'ancrage Y
    std::vector<std::uint8_t> pixels; // width*height indices palette (transparence geree au decode)
};

// Flat (sol/plafond) : 64x64 indices palette, sans en-tete.
struct Flat {
    std::array<std::uint8_t, 64 * 64> pixels{};
};

// ============================================================================
// F. Couleurs
// ============================================================================

struct Rgb {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
};

// Une palette de 256 couleurs. PLAYPAL = 14 Palette.
struct Palette {
    std::array<Rgb, 256> colors{};
};

// Une table d'assombrissement (index -> index). COLORMAP = 34 Colormap.
struct Colormap {
    std::array<std::uint8_t, 256> map{};
};

// ============================================================================
// Agregat : un niveau complet charge en memoire (familles A + B + C)
// ============================================================================

struct Level {
    std::vector<Thing> things;
    std::vector<Linedef> linedefs;
    std::vector<Sidedef> sidedefs;
    std::vector<Vertex> vertexes;
    std::vector<Seg> segs;
    std::vector<SubSector> subsectors;
    std::vector<Node> nodes;
    std::vector<Sector> sectors;
    Reject reject;
    Blockmap blockmap;
};

} // namespace game::loader
