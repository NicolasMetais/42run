#include <Texture.hpp>

void Texture::loadTexture(const std::string& path) {
	if (path.size() >= 4 && path.substr(path.size() - 4) == ".bmp")
		loadBMP(path);
	else if (path.size() >= 4 && path.substr(path.size() - 4) == ".png")
		loadPNG(path);
	else if ((path.size() >= 4 && path.substr(path.size() - 4) == ".jpg")
			|| (path.size() >= 4 && path.substr(path.size() - 5) == ".jpeg"))
		loadJPEG(path);
	else
		throw std::runtime_error("Error: wrong extension file. Only bmp and png are handled for textures files here");
};

void Texture::loadPNG(const std::string& path) {
    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp) throw std::runtime_error("Cannot open PNG file");

    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        fclose(fp);
        throw std::runtime_error("Not a PNG file");
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) { fclose(fp); throw std::runtime_error("png_create_read_struct failed"); }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) { png_destroy_read_struct(&png_ptr, nullptr, nullptr); fclose(fp); throw std::runtime_error("png_create_info_struct failed"); }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        throw std::runtime_error("Error during PNG read");
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    // Conversion en RGBA 8 bits
    if (bit_depth == 16) png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY) png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    bpp = 4;
    inputFormat = GL_RGBA;
	png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    data.resize(rowbytes * height * 4);
    std::vector<png_bytep> row_pointers(height);
    for (int y = 0; y < (int)height; y++)
        row_pointers[y] = data.data() + y * width * 4;

    png_read_image(png_ptr, row_pointers.data());
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);
}

void Texture::loadBMP(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("bmp file opening error");
	unsigned char header[54];
	file.read(reinterpret_cast<char*>(header), 54);
	width = *reinterpret_cast<u_int32_t*>(&header[18]);
	height = *reinterpret_cast<u_int32_t*>(&header[22]);
	short bpp = *reinterpret_cast<short*>(&header[28]);
	int offset = *reinterpret_cast<int*>(&header[10]);
	
	if (bpp != 24 && bpp != 32)
		throw std::runtime_error("only BMP 24 & 32 bits suppported");
		
	this->bpp = bpp / 8;
	int imgSize = width * height * this->bpp;
	
	file.seekg(offset, std::ios::beg);
	std::vector<unsigned char> tmpData(imgSize);
	file.read(reinterpret_cast<char*>(tmpData.data()), imgSize);
	for (int y = 0; y < (int)height / 2; ++y)
	{
		for (int x = 0; x < (int)width * this->bpp; ++x)
			std::swap(tmpData[y * width * this->bpp + x], 
					tmpData[(height - 1 - y) * width * this->bpp + x]);
	}
	file.close();
	
	this->data = std::move(tmpData);
	this->inputFormat = (this->bpp == 4) ? GL_BGRA : GL_BGR;
};

void Texture::loadJPEG(const std::string& path) {
	FILE* fp = fopen(path.c_str(), "rb");
    if (!fp) throw std::runtime_error("Cannot open JPG file");
	
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	width = cinfo.output_width;
	height = cinfo.output_height;
	int channels = cinfo.output_components;

	std::vector<unsigned char> tmpData(width * height * channels);
	while (cinfo.output_scanline < height) {
		unsigned char* rowptr = tmpData.data() + cinfo.output_scanline * width * channels;
		jpeg_read_scanlines(&cinfo, &rowptr, 1);
	}

	std::vector<unsigned char> rgbaData(width * height * 4);
	for(int i = 0; i < (int)width * (int)height; ++i) {
		rgbaData[i * 4 + 0] = tmpData[i * 3 + 0];
		rgbaData[i * 4 + 1] = tmpData[i * 3 + 1];
		rgbaData[i * 4 + 2] = tmpData[i * 3 + 2];
		rgbaData[i * 4 + 3] = 255;

	}
	data = std::move(rgbaData);
	bpp = 4;
	inputFormat = GL_RGBA;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);
}

void Texture::openGl2DTextureGen() {
	glGenTextures(1, &this->id);

	glBindTexture(GL_TEXTURE_2D, this->id);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//sert a repeter la texture plusieurs fois 
	//si elle est trop petite par rapport au model
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//gestion de la texture si elle est scale en plus petit
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//gestion de la texture si elle est scale en plus grand
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum internalFormat = (bpp == 4) ? GL_RGBA8 : GL_RGB8;
	GLenum dataFormat = inputFormat;

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
             width, height, 0, dataFormat, GL_UNSIGNED_BYTE, this->data.data());

	glGenerateMipmap(GL_TEXTURE_2D);
}

// void Texture::loadPNG(const std::string& path) {
// 	uint8_t bitDepth;
// 	uint8_t colorType;
// 	uint8_t compressionMethod;
// 	uint8_t filterMethod;
// 	uint8_t interlaceMethod;

// 	std::ifstream file(path, std::ios::binary);
// 	if (!file.is_open())
// 		throw std::runtime_error("png file opening error");
// 	std::array<unsigned char, 8> signature{};
// 	file.read(reinterpret_cast<char *>(signature.data()), 8);
// 	if (file.gcount() != 8)
// 		throw std::runtime_error("Invalid signature");
// 	signCheck(signature);
// 	int i = 0;
// 	std::vector<unsigned char> compressedData;
// 	while (true) {
// 		uint32_t length = readUint32FromFile(file); //calcul de la lenght de data
// 		char type[5] = {0};
// 		file.read(type, 4); //description du type de data et de ce que je doit en faire
// 		if (file.gcount() != 4)
// 			throw std::runtime_error("Invalid chunk type");
// 		if (i == 0 && std::strncmp(type, "IHDR", 4) != 0)
// 			throw std::runtime_error("IHDR must be the first chunk");
// 		std::vector<unsigned char> TmpData(length);
// 		file.read(reinterpret_cast<char*>(TmpData.data()), length);
// 		if (file.gcount() != length)
// 			throw std::runtime_error("invalid chunk");
// 		if (std::strncmp(type, "IDAT", 4) == 0)
// 			compressedData.insert(compressedData.end(), TmpData.begin(), TmpData.end());
// 		else if (std::strncmp(type, "IHDR", 4) == 0 && i == 0)
// 		{
// 			this->width = readUint32FromBuff(TmpData.data());
// 			this->height = readUint32FromBuff(TmpData.data() + 4);
// 			bitDepth = TmpData[8];
// 			colorType = TmpData[9];
// 			compressionMethod = TmpData[10];
// 			filterMethod = TmpData[11];
// 			interlaceMethod = TmpData[12];
// 			if (compressionMethod != 0)
// 				throw std::runtime_error("Unsupported compression method");
// 			if (filterMethod != 0)
// 				throw std::runtime_error("Unsupported filter method");
// 			if (interlaceMethod > 1)
// 				throw std::runtime_error("Unsupported interlace method");
// 		}
// 		else if (std::strncmp(type, "IEND", 4) == 0)
// 			break ;
// 		// else
// 		// 	std::cout << type << std::endl; //TYPE PRINTER
// 		crcCheck(file, type, TmpData);
// 		i++;
// 	}
// 	if (compressedData.empty())
//    		throw std::runtime_error("No data found");
// 	this->bpp = (bitDepth * samplePerPixel(colorType) + 7) / 8;
// 	size_t stride = 1 + width * this->bpp;
// 	size_t expectedSize = stride * height;
// 	std::vector<unsigned char> decompressed = inflatePNG(compressedData, expectedSize);
// 	std::vector<unsigned char> textures;
// 	scanlineInterpreter(textures, decompressed, this->bpp);
// 	this->data = std::move(textures);
// 	this->inputFormat = (this->bpp == 4) ? GL_RGBA : GL_RGB;
// };

// uint32_t readUint32FromFile(std::ifstream &file) {
// 	unsigned char bytes[4];
// 	file.read(reinterpret_cast<char *>(bytes), 4);
// 	if (file.gcount() != 4)
// 		throw std::runtime_error("Invalid uint read");
// 	return ((uint32_t(bytes[0]) << 24) 
// 			| (uint32_t(bytes[1]) << 16)
// 			| (uint32_t(bytes[2]) << 8) 
// 			| (uint32_t(bytes[3])));
// };

// uint32_t readUint32FromBuff(unsigned char *data) {
// 	return ((uint32_t(data[0]) << 24) 
// 			| (uint32_t(data[1]) << 16)
// 			| (uint32_t(data[2]) << 8) 
// 			| (uint32_t(data[3])));
// };

// std::vector<unsigned char> inflatePNG(const std::vector<unsigned char>& compressedData, int size) {
// 	std::vector<unsigned char> out(size);

// 	z_stream stream;
// 	std::memset(&stream, 0, sizeof(stream));

// 	stream.next_in = const_cast<Bytef*>(compressedData.data());
// 	stream.avail_in = compressedData.size();

// 	stream.next_out = out.data();
// 	stream.avail_out = out.size();

// 	if (inflateInit(&stream) != Z_OK)
// 		throw std::runtime_error("inflateInit failed");
	
// 	int ret = inflate(&stream, Z_FINISH);
// 	if (ret != Z_STREAM_END)
// 	{
// 		inflateEnd(&stream);
// 		throw std::runtime_error("inflate failed");
// 	}
// 	inflateEnd(&stream);
// 	return out;
// }

// int samplePerPixel(uint8_t colorType) {
// 	switch (colorType)
// 	{
// 	case 0: //GrayScale
// 		return 1; 
// 	case 2: //RGB
// 		return 3;
// 	case 3: //Indexed-color (une palette)
// 		return 1;
// 	case 4: //Grayscale + alpha
// 		return 2;
// 	case 6: //RGBA
// 		return 4;
// 	default:
// 		throw std::runtime_error("Invalid PNG colorType");
// 	}
// }

// unsigned char paethPredictor(unsigned char a, unsigned char b, unsigned char c)
// {
//     int p = (int)a + (int)b - (int)c;
//     int pa = std::abs(p - a);
//     int pb = std::abs(p - b);
//     int pc = std::abs(p - c);
//     if (pa <= pb && pa <= pc) return a;
//     else if (pb <= pc) return b;
//     else return c;
// }

// void Texture::scanlineInterpreter(std::vector<unsigned char>& textures, std::vector<unsigned char>& decompressed, size_t bpp) {
// 	std::vector<unsigned char>prev_line(this->width * bpp, 0);
// 	textures.reserve(this->width * this->height * bpp);
// 	int stride = 1 + this->width * bpp; //taille reel d'une scanline +1 byte pour le byte de filtre
// 	for(size_t i = 0; i < this->height; i++) {
// 		unsigned char *scanline = decompressed.data() + i * stride;
// 		unsigned char filter = scanline[0];
// 		std::vector<unsigned char> line(this->width * bpp, 0);
// 		for (size_t j = 0; j < this->width * bpp; j++) {
// 			unsigned char left = 0;
// 			unsigned char above = 0;
// 			unsigned char upper_left = 0;

// 			if (j >= bpp)
// 				left = line[j - bpp];
// 			if (i > 0)
// 				above = prev_line[j];
// 			if (i > 0 && j >= bpp)
// 				upper_left = prev_line[j - bpp];
// 			switch (filter)
// 			{
// 				case 0:
// 					line[j] = static_cast<unsigned char>(scanline[j + 1]);
// 					break ;
// 				case 1:
// 					line[j] = static_cast<unsigned char>(scanline[j + 1] + left);
// 					break ;
// 				case 2:
// 					line[j] = static_cast<unsigned char>(scanline[j + 1] + above);
// 					break ;
// 				case 3:
// 					line[j] = static_cast<unsigned char>(scanline[j + 1] + ((left + above) / 2));
// 					break ;
// 				case 4:
// 					line[j] = static_cast<unsigned char>(scanline[j + 1] + paethPredictor(left, above, upper_left));
// 					break ;
// 				default:
// 					throw std::runtime_error("Unknown filter type");
// 			}
// 		}
// 		textures.insert(textures.end(), line.begin(), line.end());
// 		prev_line = std::move(line);
// 	}
// }

// void signCheck(std::array<unsigned char, 8>& signature) {
// 	unsigned char expected[8] = {
// 			0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A
// 		}; //signature attendu sur tout les png
// 	if (!std::equal(std::begin(signature), std::end(signature), std::begin(expected)))
// 		throw std::runtime_error("Invalid signature");
// }

// void crcCheck(std::ifstream& file, char type[5], std::vector<unsigned char>& data) {
// 	uint32_t crc = readUint32FromFile(file); //Je stock le crc. il sert a verif l'integrite des donne apres loading
// 	uint32_t calc_crc = crc32(0L, Z_NULL, 0); //init d'un crc vide
// 	calc_crc = crc32(calc_crc, reinterpret_cast<const Bytef*>(type), 4); //je met le type
// 	calc_crc = crc32(calc_crc, data.data(), data.size()); //je met la data
// 	if (calc_crc != crc) // je compare si les deux crc sont les meme
// 		throw std::runtime_error("CRC mismatch"); //check la validite de la data avec crc
// }