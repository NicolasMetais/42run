#include <GameScene.hpp>

GameScene GameScene::fromJson(const std::string& path, ModelLoader& loader) {
	nlohmann::json level;
    std::ifstream file(path);
    if (!file.is_open())
        throw (std::runtime_error("Level files cannot be openned"));
    file >> level;

    GameScene scene;
    for (auto& [filename, data] : level["level"]["model"].items()) {
        LoadedModel lm = loader.load("resources/" + filename);
        auto pos = data["position"];
        lm.transform.setPosition(pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>());
        lm.transform.setScale(data["scale"].get<float>());
        scene.models.push_back(std::move(lm));
    };
    return scene;
};