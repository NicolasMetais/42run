#pragma once
#include <string>

/** @brief Un skin joueur achetable : fichier modele, prix en coins, possede/equipe ou non. */
struct SkinInfo {
    std::string file;
    std::string label;
    unsigned int price;
    bool owned;
    bool equipped;
};
