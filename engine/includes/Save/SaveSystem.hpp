#pragma once
#include <string>
#include <json.hpp>

/** @brief Lecture/ecriture generique d'un blob JSON sur disque. Ne connait rien du jeu :
 * chaque projet decide de sa propre structure de donnees et de quand appeler save()/load(). */
namespace SaveSystem {
    /** @brief Ecrit data dans path de facon atomique (fichier temporaire puis rename) :
     * un crash pendant l'ecriture laisse l'ancien fichier intact plutot qu'un fichier corrompu.
     * @return false si l'ecriture ou le rename ont echoue. */
    bool save(const std::string& path, const nlohmann::json& data);

    /** @brief Charge le JSON de path. Renvoie un objet vide (pas d'exception) si le fichier
     * n'existe pas encore ou si son contenu est invalide/corrompu. */
    nlohmann::json load(const std::string& path);
}
