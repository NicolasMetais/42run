#include <ModelLoader.hpp>

LoadedModel& ModelLoader::load(const std::string& path) {
    auto it = cache.find(path);
    if (it != cache.end())
        return it->second;

    LoadedModel lm;
    lm.gltf.parseJson(path);
    lm.meshes.reserve(lm.gltf.meshes.size());
    for (auto& mesh : lm.gltf.meshes) {
        lm.meshes.push_back(gltf.buildMeshData(lm.gltf, mesh));
        utils::prepareMats(lm.meshes.back(), textureManager);
        for (auto& sub: lm.meshes.back().submeshes)
            renderer.InitMesh(sub);
    }
    auto [inserted, _] = cache.emplace(path, std::move(lm));
    return inserted->second;
};
