NAME = 42Run
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -Iincludes -Iloader/include -std=c++17 -g3 $(shell sdl2-config --cflags) -fPIE

ifdef DEBUG
CXXFLAGS += -DDEBUG_WHITE
endif

LDFLAGS = $(shell sdl2-config --libs) -lGL -lz -lpng -ljpeg -fPIE

SRCS =	srcs/main.cpp \
		srcs/App.cpp \
		srcs/Timer.cpp \
		srcs/Mouse.cpp \
		srcs/Keyboard.cpp \
		srcs/Window.cpp \
		srcs/Loader/GltfModel.cpp \
		srcs/Loader/GltfImporter.cpp \
		srcs/Loader/AccessorView.cpp \
		srcs/Camera.cpp \
		srcs/Loader/ObjImporter.cpp \
		srcs/Skybox.cpp \
		srcs/Shader.cpp \
		srcs/Renderer.cpp \
		srcs/Transform.cpp \
		srcs/Texture.cpp \
		srcs/TextureManager.cpp \
		srcs/Event.cpp \
		srcs/utils.cpp \
		srcs/SceneManager.cpp \
		srcs/GameScene.cpp \
		srcs/Loader/ModelLoader.cpp \
		srcs/Animation/AnimationManager.cpp \
		srcs/Systems/PhysicsSystem.cpp \
		srcs/Systems/CollisionSystem.cpp \
		srcs/Runner/ChunkManager.cpp \
		srcs/Runner/ChunkGenerator.cpp \
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
	cc -Wall -Wextra -Werror -Iincludes -Iloader -g3 -MMD -MP -c $< -o  $@ -fPIC

-include $(OBJS:.o=.d)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

debug:
	$(MAKE) DEBUG=1 re

.PHONY: all clean fclean re debug
