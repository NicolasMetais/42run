#pragma once
#include <iostream>
#include <unordered_map>
#include <Renderer.hpp>
#include <GltfImporter.hpp>
#include <TextureManager.hpp>
#include <LoadedModel.hpp>

class ModelLoader {
    private:
		GltfImporter gltf;
		Renderer& renderer;
		TextureManager& textureManager;
		std::unordered_map<std::string, LoadedModel> cache;
    public:
        ModelLoader(Renderer& r, TextureManager& tm) : renderer(r), textureManager(tm) {};
        LoadedModel& load(const std::string& path);
};