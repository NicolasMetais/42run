NAME = 42Run
CC = c++
CFLAGS = -Wall -Wextra -Werror -Iincludes -Iloader -std=c++17 -g3 $(shell sdl2-config --cflags) -fPIE 
LDFLAGS = $(shell sdl2-config --libs) -lGL -lz -lpng -ljpeg -fPIE

SRCS =	srcs/main.cpp \
		srcs/App.cpp \
		srcs/Timer.cpp \
		srcs/Mouse.cpp \
		srcs/Keyboard.cpp \
		srcs/Window.cpp \
		srcs/GltfModel.cpp \
		srcs/GltfImporter.cpp \
		srcs/AccessorView.cpp \
		srcs/Camera.cpp \
		srcs/ObjImporter.cpp \
		srcs/Skybox.cpp \
		srcs/Shader.cpp \
		srcs/Renderer.cpp \
		srcs/Transform.cpp \
		srcs/Texture.cpp \
		srcs/Event.cpp \
		srcs/utils.cpp \
		loader/src/glad.c \

OBJ_DIR = obj

OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(filter %.cpp, $(SRCS))) \
       $(patsubst %.c,$(OBJ_DIR)/%.o,$(filter %.c,$(SRCS)))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o  $@

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	cc -Wall -Wextra -Werror -Iincludes -Iloader -g3 -MMD -MP -c $< -o  $@ -fPIC

-include $(OBJS:.o=.d)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
