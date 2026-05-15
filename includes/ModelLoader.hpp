#pragma once
#include <iostream>
#include <Renderer.hpp>
#include <GltfImporter.hpp>
#include <TextureManager.hpp>
#include <LoadedModel.hpp>

class ModelLoader {
    private: 
		GltfImporter gltf;
		Renderer& renderer;
		TextureManager& textureManager;
    public:
        ModelLoader(Renderer& r, TextureManager& tm) : renderer(r), textureManager(tm) {};
        LoadedModel load(const std::string& path);
};