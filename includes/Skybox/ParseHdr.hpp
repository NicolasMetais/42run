#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <array>
#include <cmath>


class parseHdr {
    public:
        parseHdr(const std::string& path);
        const std::vector<float> getData() const { return parsedHdr; };
        int getWidth() const { return width; };
        int getHeight() const { return height; };

    private:
        std::vector<float> parsedHdr;
        int width;
        int height;
};