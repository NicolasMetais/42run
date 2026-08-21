NAME = 42Run
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -Iengine/includes -Igame/includes -Iloader/include -std=c++17 -g3 $(shell sdl2-config --cflags) -fPIE

ifdef DEBUG
CXXFLAGS += -DDEBUG_WHITE
endif

ifdef ASAN
CXXFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
LDFLAGS  += -fsanitize=address,undefined -no-pie
endif

LDFLAGS = $(shell sdl2-config --libs) -lGL -lz -lpng -ljpeg -fPIE

SRCS =	game/srcs/main.cpp \
		game/srcs/App.cpp \
		engine/srcs/Timer.cpp \
		engine/srcs/Window.cpp \
		engine/srcs/Camera.cpp \
		engine/srcs/Shader.cpp \
		engine/srcs/utils.cpp \
		engine/srcs/Input/Mouse.cpp \
		engine/srcs/Input/Keyboard.cpp \
		engine/srcs/Input/Event.cpp \
		engine/srcs/Texture/Texture.cpp \
		engine/srcs/Texture/TextureManager.cpp \
		engine/srcs/Scene/Transform.cpp \
		engine/srcs/Scene/SceneManager.cpp \
		engine/srcs/Scene/GameScene.cpp \
		engine/srcs/Skybox/Skybox.cpp \
		engine/srcs/Renderer/Renderer.cpp \
		engine/srcs/Loader/GltfModel.cpp \
		engine/srcs/Loader/GltfImporter.cpp \
		engine/srcs/Loader/AccessorView.cpp \
		engine/srcs/Loader/ObjImporter.cpp \
		engine/srcs/Loader/ModelLoader.cpp \
		engine/srcs/Animation/AnimationManager.cpp \
		engine/srcs/Systems/PhysicsSystem.cpp \
		engine/srcs/Systems/CollisionSystem.cpp \
		engine/srcs/Save/SaveSystem.cpp \
		game/srcs/Runner/ChunkManager.cpp \
		game/srcs/Runner/ChunkGenerator.cpp \
		engine/srcs/Font/FontManager.cpp \
		engine/srcs/Font/TextRenderer.cpp \
		engine/srcs/UI/UIRenderer.cpp \
		game/srcs/UI/MainMenu.cpp \
		game/srcs/UI/PauseMenu.cpp \
		game/srcs/UI/SkinMenu.cpp \
		game/srcs/UI/GameOverMenu.cpp \
		engine/srcs/Skybox/ParseHdr.cpp \
		loader/src/glad.c \

OBJ_DIR = obj

OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(filter %.cpp, $(SRCS))) \
       $(patsubst %.c,$(OBJ_DIR)/%.o,$(filter %.c,$(SRCS)))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) -o $(NAME) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o  $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	cc -Wall -Wextra -Werror -Iengine/includes -Igame/includes -Iloader -g3 -MMD -MP -c $< -o  $@ -fPIC $(if $(ASAN),-fsanitize=address -fsanitize=undefined)

-include $(OBJS:.o=.d)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

debug:
	$(MAKE) DEBUG=1 re

.PHONY: all clean fclean re debug
