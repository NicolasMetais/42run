#include <Scene/GameScene.hpp>
#include <utils.hpp>
#include <cfloat>

void GameScene::collectBoxes(
    const GltfModel& gltf,
    const std::vector<MeshData>& meshes,
    int nodeIdx,
    const Matrix<float>& parentWorld,
    ColliderComponent& col,
    const std::unordered_set<int>& hiddenNodes)
{
    if (hiddenNodes.count(nodeIdx)) return;
    const Node& node = gltf.nodes[nodeIdx];
    Matrix<float> world = parentWorld * utils::nodeLocalMatrix(node);

    if (node.mesh >= 0 && node.mesh < (int)meshes.size()) {
        for (auto& sub : meshes[node.mesh].submeshes) {
            float xs[2] = {sub.min.x(), sub.max.x()};
            float ys[2] = {sub.min.y(), sub.max.y()};
            float zs[2] = {sub.min.z(), sub.max.z()};

            Vector<float> wMin = {FLT_MAX,  FLT_MAX,  FLT_MAX};
            Vector<float> wMax = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

            for (int xi = 0; xi < 2; xi++)
                for (int yi = 0; yi < 2; yi++)
                    for (int zi = 0; zi < 2; zi++) {
                        Vector<float> corner = {xs[xi], ys[yi], zs[zi], 1.0f};
                        Vector<float> wc = world * corner;
                        if (wc.x() < wMin.x()) wMin.x() = wc.x();
                        if (wc.y() < wMin.y()) wMin.y() = wc.y();
                        if (wc.z() < wMin.z()) wMin.z() = wc.z();
                        if (wc.x() > wMax.x()) wMax.x() = wc.x();
                        if (wc.y() > wMax.y()) wMax.y() = wc.y();
                        if (wc.z() > wMax.z()) wMax.z() = wc.z();
                    }

            col.boxes.push_back({wMin, wMax});
        }
    }

    for (int child : node.children)
        collectBoxes(gltf, meshes, child, world, col, hiddenNodes);
}

ColliderComponent GameScene::colliderFromModel(const LoadedModel& lm, const std::unordered_set<int>& hiddenNodes) {
    ColliderComponent col;
    Matrix<float> root = identity<float>(4);
    const Scene& scene = lm.gltf.scenes[lm.gltf.defaultScene];
    for (int rootIdx : scene.rootNodes)
        collectBoxes(lm.gltf, lm.meshes, rootIdx, root, col, hiddenNodes);
    return col;
}

GameScene GameScene::fromJson(const std::string& path, ModelLoader& loader) {
    nlohmann::json level;
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Level files cannot be openned");
    file >> level;

    GameScene scene;
    for (auto& [filename, data] : level["level"]["model"].items()) {
        LoadedModel& lm = loader.load("resources/" + filename);
        EntityId id = scene.createEntity();

        auto pos = data["position"];
        Transform t;
        t.setPosition(pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>());
        t.setScale(data["scale"].get<float>());
        scene.transforms[id] = t;
        scene.renders[id]    = { &lm };

        if (data.contains("collider"))
            scene.colliders[id] = scene.colliderFromModel(lm);

        if (data.contains("rigidbody"))
            scene.rigidbodies[id] = {};

        if (data.contains("trigger"))
            scene.triggers[id] = {};
    }
    return scene;
}

void GameScene::loadPlayer(const std::string& path, ModelLoader& loader) {
    nlohmann::json player;
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Player file cannot be opened: " + path);
    file >> player;

    LoadedModel& lm = loader.load("resources/" + player["model"].get<std::string>());
    EntityId id = createEntity();
    playerId = id;

    auto pos = player["position"];
    Transform t;
    t.setPosition(pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>());
    t.setScale(player["scale"].get<float>());
    transforms[id] = t;
    renders[id]    = { &lm };
    colliders[id]  = colliderFromModel(lm);
    rigidbodies[id] = {};
    triggers[id] = {};

}

void GameScene::destroyEntity(EntityId id) {
    transforms.erase(id);
    renders.erase(id);
    colliders.erase(id);
    rigidbodies.erase(id);
    triggers.erase(id);

};

