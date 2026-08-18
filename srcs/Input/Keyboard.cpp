#include <Input/Keyboard.hpp>

#define KEY_W           0x001
#define KEY_A           0x002
#define KEY_S           0x004
#define KEY_D           0x008
#define KEY_CAM_UP      0x010
#define KEY_CAM_DOWN    0x020
#define PLAYER_LEFT     0x040
#define PLAYER_RIGHT    0x080
#define PLAYER_FORWARD  0x100
#define PLAYER_BACK     0x200
#define PLAYER_JUMP     0x400
#define MENU_UP         0x800
#define MENU_DOWN       0x1000
#define MENU_CONFIRM    0x2000
#define PLAYER_PAUSE    0x4000

void Keyboard::processMenuEvent(SDL_Event& e, bool& running) {
    (void)running; // Echap ne quitte plus le jeu depuis un menu ; seul le Quit du menu principal ferme le jeu
    if (e.type != SDL_KEYDOWN && e.type != SDL_KEYUP) return;

    bool pressed = (e.type == SDL_KEYDOWN);
    SDL_Scancode sc  = e.key.keysym.scancode;

    switch (sc) {
        case SDL_SCANCODE_UP:
            pressed ? moveFlags |= MENU_UP    : moveFlags &= ~MENU_UP; break;
        case SDL_SCANCODE_DOWN:
            pressed ? moveFlags |= MENU_DOWN    : moveFlags &= ~MENU_DOWN; break;
        case SDL_SCANCODE_RETURN:
            pressed ? moveFlags |= MENU_CONFIRM    : moveFlags &= ~MENU_CONFIRM;    break;
        case SDL_SCANCODE_KP_ENTER:
            pressed ? moveFlags |= MENU_CONFIRM    : moveFlags &= ~MENU_CONFIRM;    break;
        default: break;
    }
};


void Keyboard::processEvent(SDL_Event& e, bool& running, Camera& cam, float& fps, bool& lockCam) {
    (void)running; // Echap ouvre desormais la pause (consumePause) au lieu de quitter directement pendant le jeu
    if (e.type != SDL_KEYDOWN && e.type != SDL_KEYUP) return;

    bool pressed = (e.type == SDL_KEYDOWN);
    SDL_Scancode sc  = e.key.keysym.scancode;

    // --- touches non-lettre : scancode (layout-independent) ---
    switch (sc) {
        case SDL_SCANCODE_EQUALS: case SDL_SCANCODE_KP_PLUS:
            if (pressed) { cam.speedUp(); }   break;
        case SDL_SCANCODE_MINUS:  case SDL_SCANCODE_KP_MINUS:
            if (pressed) { cam.speedDown(); } break;
        case SDL_SCANCODE_ESCAPE:
            pressed ? moveFlags |= PLAYER_PAUSE : moveFlags &= ~PLAYER_PAUSE; break;
        case SDL_SCANCODE_LSHIFT:
            pressed ? moveFlags |= KEY_CAM_UP   : moveFlags &= ~KEY_CAM_UP;   break;
        case SDL_SCANCODE_LCTRL:
            pressed ? moveFlags |= KEY_CAM_DOWN : moveFlags &= ~KEY_CAM_DOWN; break;
        case SDL_SCANCODE_LEFT:
            pressed ? moveFlags |= PLAYER_LEFT    : moveFlags &= ~PLAYER_LEFT;    break;
        case SDL_SCANCODE_RIGHT:
            pressed ? moveFlags |= PLAYER_RIGHT   : moveFlags &= ~PLAYER_RIGHT;   break;
        case SDL_SCANCODE_UP:
            pressed ? moveFlags |= PLAYER_JUMP    : moveFlags &= ~PLAYER_JUMP;    break;
        // case SDL_SCANCODE_DOWN:
        //     pressed ? moveFlags |= PLAYER_BACK    : moveFlags &= ~PLAYER_BACK;    break;
        case SDL_SCANCODE_W:
            pressed ? moveFlags |= PLAYER_JUMP    : moveFlags &= ~PLAYER_JUMP;    break;
        // case SDL_SCANCODE_S:
        //     pressed ? moveFlags |= PLAYER_BACK    : moveFlags &= ~PLAYER_BACK;    break;
        case SDL_SCANCODE_A:
            pressed ? moveFlags |= PLAYER_LEFT    : moveFlags &= ~PLAYER_LEFT;    break;
        case SDL_SCANCODE_D:
            pressed ? moveFlags |= PLAYER_RIGHT   : moveFlags &= ~PLAYER_RIGHT;   break;
        case SDL_SCANCODE_F:
            if (pressed) { std::cout << "FPS: " << fps << std::endl; } break;
        case SDL_SCANCODE_TAB:
            if (pressed) {
                lockCam = !lockCam;
                SDL_SetRelativeMouseMode(lockCam ? SDL_TRUE  : SDL_FALSE);
                SDL_ShowCursor        (lockCam ? SDL_DISABLE : SDL_ENABLE);
            }
            break;
        default: break;
    }
}

void Keyboard::applyMovement(Camera&, Transform&, float) {
    // caméra en follow mode — le mouvement est géré par camera.follow() dans App::update()
}

bool Keyboard::consumeLeft() {
    bool val = moveFlags & PLAYER_LEFT;
    moveFlags &= ~PLAYER_LEFT;
    return val;
};

bool Keyboard::consumeRight() {
    bool val = moveFlags & PLAYER_RIGHT;
    moveFlags &= ~PLAYER_RIGHT;
    return val;
};

bool Keyboard::isForward() const { return moveFlags & PLAYER_FORWARD; }
bool Keyboard::isBack()    const { return moveFlags & PLAYER_BACK; }
bool Keyboard::isJump()    const { return moveFlags & PLAYER_JUMP; }

bool Keyboard::consumeMenuUp() {
    bool v = moveFlags & MENU_UP;
    moveFlags &= ~MENU_UP;
    return v; 
}

bool Keyboard::consumeMenuDown() {
    bool v = moveFlags & MENU_DOWN;
    moveFlags &= ~MENU_DOWN;
    return v;
}

bool Keyboard::consumeMenuConfirm() {
    bool v = moveFlags & MENU_CONFIRM;
    moveFlags &= ~MENU_CONFIRM;
    return v;
}

bool Keyboard::consumePause() {
    bool v = moveFlags & PLAYER_PAUSE;
    moveFlags &= ~PLAYER_PAUSE;
    return v;
}


