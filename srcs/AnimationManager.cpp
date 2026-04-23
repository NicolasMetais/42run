#include <AnimationManager.hpp>
#include <utils.hpp>
#include <cmath>
#include <cstring>

static Vector<float> slerp(const Vector<float>& a, const Vector<float>& b, float t) {
    float dot = a.x()*b.x() + a.y()*b.y() + a.z()*b.z() + a.w()*b.w();

    Vector<float> b2 = b;
    if (dot < 0.0f) { dot = -dot; b2 = b * -1.0f; }

    if (dot > 0.9995f) {
        Vector<float> r = a + (b2 - a) * t;
        float len = std::sqrt(r.x()*r.x() + r.y()*r.y() + r.z()*r.z() + r.w()*r.w());
        return r * (1.0f / len);
    }

    float angle = std::acos(dot);
    float sinA  = std::sin(angle);
    return a * (std::sin((1.0f - t) * angle) / sinA) + b2 * (std::sin(t * angle) / sinA);
}

// Lit une MAT4 column-major (GLTF) et la convertit en row-major (notre Matrix)
static Matrix<float> readInverseBindMatrix(const AccessorView& acc, uint32_t index, const uint8_t* base) {
    size_t stride = acc.bufferView->byteStride != 0 ? acc.bufferView->byteStride : 64;
    const uint8_t* ptr = base + acc.bufferView->byteOffset + acc.byteOffset + index * stride;

    float raw[16];
    std::memcpy(raw, ptr, 64);

    // GLTF column-major -> notre row-major : transposer
    Matrix<float> m = identity<float>(4);
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            m[r][c] = raw[c * 4 + r];
    return m;
}

// DFS depuis les root nodes pour calculer la world matrix de chaque node
static void computeWorldMatrices(const GltfModel& model, int nodeIdx,
                                  const Matrix<float>& parentWorld,
                                  std::vector<Matrix<float>>& worldMats) {
    worldMats[nodeIdx] = parentWorld * utils::nodeLocalMatrix(model.nodes[nodeIdx]);
    for (int child : model.nodes[nodeIdx].children)
        computeWorldMatrices(model, child, worldMats[nodeIdx], worldMats);
}

void AnimationManager::setAnimation(int index) {
    currentAnim = index;
    currentTime = 0.0f;
}

void AnimationManager::sampleChannel(GltfModel& model, const Animation& anim, const Channel& channel) {
    if (channel.target.node < 0)
        return;

    const AnimationSampler& sampler = anim.samplers[channel.samplerIndex];
    const AccessorView& inputAcc    = model.accessors[sampler.inputAccessor];
    const AccessorView& outputAcc   = model.accessors[sampler.outputAccessor];
    const uint8_t* base             = model.binaryData.data();

    uint32_t count = inputAcc.count;
    float t0 = inputAcc.getScalar<float>(0, base);
    float tN = inputAcc.getScalar<float>(count - 1, base);
    float t  = std::fmod(currentTime, tN);
    if (t < t0) t = t0;

    uint32_t i = 0;
    while (i + 1 < count && inputAcc.getScalar<float>(i + 1, base) <= t)
        ++i;
    if (i + 1 >= count)
        i = count - 2;

    float tA    = inputAcc.getScalar<float>(i, base);
    float tB    = inputAcc.getScalar<float>(i + 1, base);
    float alpha = (tB > tA) ? (t - tA) / (tB - tA) : 0.0f;

    Node& node = model.nodes[channel.target.node];

    if (channel.target.path == Path::TRANSLATION) {
        Vector<float> r = outputAcc.getVector3(i, base) + (outputAcc.getVector3(i + 1, base) - outputAcc.getVector3(i, base)) * alpha;
        node.translation = {r.x(), r.y(), r.z()};
    } else if (channel.target.path == Path::SCALE) {
        Vector<float> r = outputAcc.getVector3(i, base) + (outputAcc.getVector3(i + 1, base) - outputAcc.getVector3(i, base)) * alpha;
        node.scale = {r.x(), r.y(), r.z()};
    } else if (channel.target.path == Path::ROTATION) {
        Vector<float> r = slerp(outputAcc.getVector4(i, base), outputAcc.getVector4(i + 1, base), alpha);
        node.rotation = {r.x(), r.y(), r.z(), r.w()};
    }
}

void AnimationManager::computeJointMatrices(LoadedModel& lm) {
    GltfModel& model = lm.gltf;
    if (model.skins.empty())
        return;

    // Calcule les world matrices de tous les nodes via DFS
    std::vector<Matrix<float>> worldMats(model.nodes.size(), identity<float>(4));
    const Scene& scene = model.scenes[model.defaultScene];
    for (int root : scene.rootNodes)
        computeWorldMatrices(model, root, identity<float>(4), worldMats);

    // Redimensionne jointMatrices si nécessaire
    lm.jointMatrices.resize(model.skins.size());

    for (size_t s = 0; s < model.skins.size(); s++) {
        const Skin& skin = model.skins[s];
        const AccessorView& invBindAcc = model.accessors[skin.inverseBindMatrixAccessor];
        const uint8_t* base = model.binaryData.data();

        lm.jointMatrices[s].resize(skin.joints.size());
        for (size_t j = 0; j < skin.joints.size(); j++) {
            Matrix<float> invBind = readInverseBindMatrix(invBindAcc, j, base);
            lm.jointMatrices[s][j] = worldMats[skin.joints[j]] * invBind;
        }
    }
}

void AnimationManager::update(LoadedModel& lm, float deltaTime) {
    if (lm.gltf.animations.empty())
        return;
    if (currentAnim < 0 || currentAnim >= (int)lm.gltf.animations.size())
        return;

    currentTime += deltaTime;

    const Animation& anim = lm.gltf.animations[currentAnim];
    for (const Channel& channel : anim.channels)
        sampleChannel(lm.gltf, anim, channel);

    computeJointMatrices(lm);
}
