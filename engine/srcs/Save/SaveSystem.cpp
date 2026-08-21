#include <Save/SaveSystem.hpp>
#include <fstream>
#include <cstdio>
#include <iostream>

bool SaveSystem::save(const std::string& path, const nlohmann::json& data) {
    std::string tmpPath = path + ".tmp";

    std::ofstream file(tmpPath);
    if (!file.is_open()) {
        std::cerr << "SaveSystem: impossible d'ouvrir " << tmpPath << " en ecriture" << std::endl;
        return false;
    }
    file << data.dump(4);
    file.close();

    // rename() est atomique sur un meme systeme de fichiers : un crash pendant l'ecriture
    // laisse le .tmp partiel orphelin plutot que de corrompre le fichier de sauvegarde existant
    if (std::rename(tmpPath.c_str(), path.c_str()) != 0) {
        std::cerr << "SaveSystem: rename de " << tmpPath << " vers " << path << " a echoue" << std::endl;
        return false;
    }
    return true;
}

nlohmann::json SaveSystem::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        return nlohmann::json::object();

    try {
        nlohmann::json data;
        file >> data;
        return data;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "SaveSystem: " << path << " illisible (" << e.what() << "), ignore" << std::endl;
        return nlohmann::json::object();
    }
}
