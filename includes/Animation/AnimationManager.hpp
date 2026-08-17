#pragma once
#include <Loader/GltfModel.hpp>
#include <Loader/LoadedModel.hpp>
#include <unordered_map>

/** @brief Drives skeletal animations on a LoadedModel, advancing time and computing joint matrices. */
class AnimationManager {
    private:
        int   currentAnim = 2;   ///< Index of the currently playing animation.
        float currentTime = 0.0f; ///< Elapsed time within the current animation in seconds.
        float playbackSpeed = 1.0f; ///< Multiplicateur de vitesse de lecture de l'animation courante.
        // pose de repos (translation/rotation/scale d'origine), capturee une fois par modele :
        // sert a reinitialiser les os que l'animation courante ne touche pas, pour eviter
        // qu'ils restent figes dans la pose de l'animation precedente (ex: jambes bloquees
        // en pleine foulee pendant que le saut anime le reste du squelette)
        std::unordered_map<const GltfModel*, std::vector<Node>> bindPoses;

        /** @brief Samples a single animation channel and writes the interpolated transform to the target node. */
        void sampleChannel(GltfModel& model, const Animation& anim, const Channel& channel);
        /** @brief Recomputes all joint matrices for the skin of @p lm from current node transforms. */
        void computeJointMatrices(LoadedModel& lm);

    public:
        AnimationManager() = default;

        /** @brief Advances the animation by @p deltaTime and updates all node transforms and joint matrices. */
        void update(LoadedModel& lm, float deltaTime);
        /** @brief Switches to animation at @p index and resets the playback timer. */
        void setAnimation(int index, float speed = 1.0f);
        /** @brief Returns the index of the currently active animation. */
        int  getCurrentAnim() const { return currentAnim; }
};
