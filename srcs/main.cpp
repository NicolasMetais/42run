#include <Mesh.hpp>
#include <App.hpp>
#include <chrono>
#include <Matrix/Matrix.hpp>
#include <Window.hpp>
#include <Transform.hpp>
#include <Renderer.hpp>
#include <Camera.hpp>
#include <Texture.hpp>
#include <Skybox.hpp>
#include <Event.hpp>
#include <GltfModel.hpp>
#include <SDL2/SDL.h>

#define HEIGHT 1500
#define WIDTH 1500

int main(int ac, char **av) {
    if (ac != 2)
    {
		std::cerr << "Error: Invalid format" << std::endl;
        return (1);
	}
	try {
		App app(WIDTH, HEIGHT, av[1]);
		GltfModel test;
		test.parseJson("resources/DamagedHelmet.gltf");
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}