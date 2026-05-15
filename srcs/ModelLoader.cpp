#include <ModelLoader.hpp>

LoadedModel ModelLoader::load(const std::string& path) {
    LoadedModel lm;
    lm.gltf.parseJson(path);
    lm.meshes.reserve(lm.gltf.meshes.size());
    for (auto& mesh : lm.gltf.meshes) {
        lm.meshes.push_back(gltf.buildMeshData(lm.gltf, mesh));
        utils::prepareMats(lm.meshes.back(), textureManager);
        for (auto& sub: lm.meshes.back().submeshes)
            renderer.InitMesh(sub);
    }
    return lm;
};
