#pragma once
#include <Loader/LoadedModel.hpp>
#include <unordered_set>

struct RenderComponent {
    LoadedModel* model = nullptr;
    // Visibilite par instance : le LoadedModel est partage (cache du loader),
    // ces nodes sont sautes au rendu pour cette entite seulement.
    std::unordered_set<int> hiddenNodes = {};

    /** @brief Cache tous les nodes du modèle portant ce nom (rendu uniquement). */
    void hideNode(const std::string& name) {
        if (!model) return;
        for (size_t i = 0; i < model->gltf.nodes.size(); ++i)
            if (model->gltf.nodes[i].name == name)
                hiddenNodes.insert((int)i);
    }

    /** @brief Cache tous les nodes dont le nom commence par ce prefixe (rendu uniquement). */
    void hidePrefixed(const std::string& prefix) {
        if (!model) return;
        for (size_t i = 0; i < model->gltf.nodes.size(); ++i)
            if (model->gltf.nodes[i].name.rfind(prefix, 0) == 0)
                hiddenNodes.insert((int)i);
    }
};
