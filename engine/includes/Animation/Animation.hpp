#pragma once
#include <iostream>
#include <unordered_map>

/** @brief Interpolation method between keyframes. */
enum class Interpolation {
    LINEAR,
    STEP,
    CUBICSPLINE
};

/** @brief Animated property targeted by a channel. */
enum class Path {
    TRANSLATION,
    ROTATION,
    SCALE,
    WEIGHTS
};

/** @brief Pairs input (time) and output (value) accessors with an interpolation mode. */
struct AnimationSampler {
    int inputAccessor = -1;   ///< Accessor index for keyframe timestamps.
    int outputAccessor = -1;  ///< Accessor index for keyframe values.

    Interpolation interpolation = Interpolation::LINEAR;

    std::string name;

    std::unordered_map<std::string, nlohmann::json> extensions;
    std::unordered_map<std::string, nlohmann::json> extras;
};

/** @brief Identifies which node property a channel animates. */
struct Target {
    int node = -1;          ///< Index of the target node (-1 = no node).
    Path path = Path::TRANSLATION; ///< Property to animate.
};

/** @brief Binds an animation sampler to a target node property. */
struct Channel {
    int samplerIndex; ///< Index into the parent Animation's sampler list.

    Target target;

    std::unordered_map<std::string, nlohmann::json> extensions;
    std::unordered_map<std::string, nlohmann::json> extras;
};

/** @brief A named animation composed of samplers and channels. */
struct Animation {
    std::string name;

    std::vector<AnimationSampler> samplers;
    std::vector<Channel> channels;

    std::unordered_map<std::string, nlohmann::json> extensions;
    std::unordered_map<std::string, nlohmann::json> extras;
};
