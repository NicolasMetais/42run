#include <Material.hpp>
#include <Texture.hpp>
#include <unordered_map>

Material::Material() {
	textureMap["map_Ka"] = &map_Ka;
	textureMap["map_Kd"] = &map_Kd;
	textureMap["map_Ks"] = &map_Ks;
	textureMap["map_Ns"] = &map_Ns;
	textureMap["map_d"] = &map_d;
	textureMap["bump"] = &bump;
	Ka = {0.0f, 0.0f, 0.0f};
	Kd = {0.8f, 0.8f, 0.8f};
	Ks = {0.0f, 0.0f, 0.0f};
	Ns = 0;
	Ni = 1.0f;
	d = 1.0f;
	illum = 2;
};

void Material::setTexture(const std::string& path, Texture texture) {
	auto it = textureMap.find(path);
	if (it == textureMap.end())
		throw std::runtime_error("Unknown texture key: " + path);
	*(it->second) = std::move(texture);
};

