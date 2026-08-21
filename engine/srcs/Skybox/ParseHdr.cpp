#include <Skybox/ParseHdr.hpp>

parseHdr::parseHdr(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
		throw std::runtime_error("Cannot open HDR file");
    std::string line;

    //header parsing
    std::getline(file, line);
    if (line.find("#?RADIANCE") == std::string::npos && line.find("#?RGBE") == std::string::npos)
        throw (std::runtime_error("Wrong HDR format"));
    while (std::getline(file, line)) {
        if (line.empty() || line == "\r" ) //skipp the useless rest of the header
        break;
    }

    //size line extraction
    std::getline(file, line);
    if (sscanf(line.c_str(), "-Y %d +X %d",  &this->height, &this->width) != 2)
        throw (std::runtime_error("invalid HDR size line"));

    //scanline interpreter
    
    for(int y = 0; y < height; ++y) {
        std::vector<std::array<uint8_t, 4>> scanline(this->width);
    //scaline headers
        uint8_t header[4];
        file.read(reinterpret_cast<char*>(header), 4);
        if(header[0] != 2 && header[1] != 2)
            throw (std::runtime_error("invalid HDR scanline format"));
        int scanW = (header[2] << 8 | header[3]);
        // header[2] = octet de poids fort, header[3] = octet de poids faible
        // ex: header[2]=0x08, header[3]=0x00 → width = 2048
        if (scanW != this->width)
            throw (std::runtime_error("corrupted HDR data"));
        
        //scanline data
        //je me deplace dans la ligne je regarde le code et je deplace x + met la data comme le code me dit (raw ou run)
        for (int c = 0; c < 4; ++c) { 
            int x = 0;
            while (x < width) {
                uint8_t code;
                file.read(reinterpret_cast<char*>(&code), 1);
                if (code > 128) {
                    int len = code - 128;
                    uint8_t val;
                    file.read(reinterpret_cast<char*>(&val), 1);
                    for (int i = 0; i < len; ++i) {
                        scanline[x++][c] = val;
                    }
                } else {
                    for (int i = 0; i < code; ++i) {
                        uint8_t val;
                        file.read(reinterpret_cast<char*>(&val), 1);
                        scanline[x++][c] = val;
                    }
                }
            }
        }
        for (int x = 0; x < width; ++x) {
            uint8_t R = scanline[x][0];
            uint8_t G = scanline[x][1];
            uint8_t B = scanline[x][2];
            uint8_t E = scanline[x][3];
            if (E == 0) {
                parsedHdr.push_back(0.0f);
                parsedHdr.push_back(0.0f);
                parsedHdr.push_back(0.0f);
            } else {
                float scale = std::pow(2.0f, (float)E - 136.0f);
                parsedHdr.push_back(R * scale);
                parsedHdr.push_back(G * scale);
                parsedHdr.push_back(B * scale);
            }
        }

    }
};
