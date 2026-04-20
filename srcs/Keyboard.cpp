#include <Keyboard.hpp>

#define MOVE_MESH_FORWARD 0x400
#define MOVE_MESH_BACKWARD 0x800

#define MOVE_MESH_UP 0x200
#define MOVE_MESH_DOWN 0x100
#define MOVE_MESH_LEFT 0x80
#define MOVE_MESH_RIGHT 0x40

#define KEY_DOWN 0x20
#define KEY_UP 0x10
#define KEY_W 0x8
#define KEY_A 0x4
#define KEY_S 0x2
#define KEY_D 0x1


void Keyboard::processEvent(SDL_Event& e, bool& running, Camera& cam, float& fps, bool& lockCam) {
switch (e.type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		bool pressed = (e.type == SDL_KEYDOWN);
		switch (e.key.keysym.scancode) {
			case SDL_SCANCODE_EQUALS:
			case SDL_SCANCODE_KP_PLUS:
				if (pressed)
					cam.speedUp();
			break;
			case SDL_SCANCODE_MINUS:
			case SDL_SCANCODE_KP_MINUS:
				if (pressed)
					cam.speedDown();
			break;
			case SDL_SCANCODE_ESCAPE:
				if (pressed) running = false;
			break ;
			case SDL_SCANCODE_W:
				pressed ? moveFlags |= KEY_W : moveFlags &= ~KEY_W;
			break ;
			case SDL_SCANCODE_A:
				pressed ? moveFlags |= KEY_A : moveFlags &= ~KEY_A;
			break ;
			case SDL_SCANCODE_S:
				pressed ? moveFlags |= KEY_S : moveFlags &= ~KEY_S;
			break ;
			case SDL_SCANCODE_D:
				pressed ? moveFlags |= KEY_D : moveFlags &= ~KEY_D;
			break ;
			case SDL_SCANCODE_LSHIFT: 
			case SDL_SCANCODE_SPACE:
				pressed ? moveFlags |= KEY_UP : moveFlags &= ~KEY_UP;
			break ;
			case SDL_SCANCODE_LCTRL:
				pressed ? moveFlags |= KEY_DOWN : moveFlags &= ~KEY_DOWN;
			break ;
			case SDL_SCANCODE_UP:
				pressed ? moveFlags |= MOVE_MESH_UP : moveFlags &= ~MOVE_MESH_UP;
			break ;
			case SDL_SCANCODE_DOWN:
				pressed ? moveFlags |= MOVE_MESH_DOWN : moveFlags &= ~MOVE_MESH_DOWN;
			break ;
			case SDL_SCANCODE_LEFT:
				pressed ? moveFlags |= MOVE_MESH_LEFT : moveFlags &= ~MOVE_MESH_LEFT;
			break ;
			case SDL_SCANCODE_RIGHT:
				pressed ? moveFlags |= MOVE_MESH_RIGHT : moveFlags &= ~MOVE_MESH_RIGHT;
			break ;
			case SDL_SCANCODE_PAGEUP:
				pressed ? moveFlags |= MOVE_MESH_FORWARD : moveFlags &= ~MOVE_MESH_FORWARD;
			break ;
			case SDL_SCANCODE_PAGEDOWN:
				pressed ? moveFlags |= MOVE_MESH_BACKWARD : moveFlags &= ~MOVE_MESH_BACKWARD;
			break ;
			case SDL_SCANCODE_F:
				if (pressed)
					std::cout << "FPS: " << fps << std::endl;
				break ;
			case SDL_SCANCODE_Q:
				if (pressed)
				{
					if (lockCam) {
						SDL_SetRelativeMouseMode(SDL_FALSE);
						SDL_ShowCursor(SDL_ENABLE);
					}
					else {
						SDL_SetRelativeMouseMode(SDL_TRUE);
						SDL_ShowCursor(SDL_DISABLE);
					}
					lockCam = !lockCam;
				}
				break ;
			default:
				break ;
		}
	}
};

void Keyboard::applyMovement(Camera& camera, Transform& transform, float deltaTime) {
	if (moveFlags & KEY_W) camera.moveForward(deltaTime);
	if (moveFlags & KEY_S) camera.moveBackward(deltaTime);
	if (moveFlags & KEY_A) camera.moveLeft(deltaTime);
	if (moveFlags & KEY_D) camera.moveRight(deltaTime);
	if (moveFlags & KEY_UP) camera.moveUp(deltaTime);
	if (moveFlags & KEY_DOWN) camera.moveDown(deltaTime);
	if (moveFlags & MOVE_MESH_UP) transform.move(Vector<float>{0.0f,3.0f * deltaTime, 0.0f});
	if (moveFlags & MOVE_MESH_DOWN) transform.move(Vector<float>{0.0f,-3.0f * deltaTime, 0.0f});
	if (moveFlags & MOVE_MESH_LEFT) transform.move(Vector<float>{-3.0f * deltaTime,0.0f, 0.0f});
	if (moveFlags & MOVE_MESH_RIGHT) transform.move(Vector<float>{3.0f* deltaTime,0.0f, 0.0f});
	if (moveFlags & MOVE_MESH_FORWARD) transform.move(Vector<float>{0.0f,0.0f, -3.0f * deltaTime});
	if (moveFlags & MOVE_MESH_BACKWARD) transform.move(Vector<float>{0.0f,0.0f, 3.0f * deltaTime});
};