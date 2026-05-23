#include <Loader/ObjImporter.hpp>
#include <App.hpp>
#include <chrono>
#include <Matrix/Matrix.hpp>
#include <Window.hpp>
#include <Scene/Transform.hpp>
#include <Renderer/Renderer.hpp>
#include <Camera.hpp>
#include <Texture/Texture.hpp>
#include <Skybox/Skybox.hpp>
#include <Input/Event.hpp>
#include <Loader/GltfModel.hpp>
#include <SDL2/SDL.h>

#define HEIGHT 1500
#define WIDTH 1500

int main(int ac, char **av) {
	(void)av;
    if (ac != 1)
    {
		std::cerr << "Error: Invalid format" << std::endl;
        return (1);
	}
	try {
		App app(WIDTH, HEIGHT);
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}