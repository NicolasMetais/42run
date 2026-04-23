#pragma once
#include <GltfModel.hpp>
#include <LoadedModel.hpp>

class AnimationManager {
    private:
        int   currentAnim = 0;
        float currentTime = 0.0f;

        void sampleChannel(GltfModel& model, const Animation& anim, const Channel& channel);
        void computeJointMatrices(LoadedModel& lm);

    public:
        AnimationManager() = default;

        void update(LoadedModel& lm, float deltaTime);
        void setAnimation(int index);
        int  getCurrentAnim() const { return currentAnim; }
};
