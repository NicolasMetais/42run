#pragma once
#include <iostream>
#include <unordered_map>

enum class Interpolation {
    LINEAR,
    STEP,
    CUBICSPLINE
};

enum class Path {
    TRANSLATION,
    ROTATION,
    SCALE,
    WEIGHTS
};

struct AnimationSampler {
    int inputAccessor = -1;
    int outputAccessor = -1;

    Interpolation interpolation = Interpolation::LINEAR;

    std::string name;

    std::unordered_map<std::string, nlohmann::json> extensions;
    std::unordered_map<std::string, nlohmann::json> extras;
};

struct Target {
    int node = -1;

    Path path = Path::TRANSLATION;
};

struct Channel {
    int samplerIndex;

    Target target;

    std::unordered_map<std::string, nlohmann::json> extensions;
    std::unordered_map<std::string, nlohmann::json> extras;
};

struct Animation {
    std::string name;

    std::vector<AnimationSampler> samplers;
    std::vector<Channel> channels;

    std::unordered_map<std::string, nlohmann::json> extensions;
    std::unordered_map<std::string, nlohmann::json> extras;
};