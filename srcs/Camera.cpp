#include <Camera.hpp>
#include <Transform.hpp>
#include <Matrix/Matrix.hpp>
#include <utils.hpp>

Camera::Camera(float w, float h, Vector<float> pos) : cameraPos(3), target(3), up(3), U(3), V(3), N(3), mousePos(2) {
	this->cameraPos = pos;
	this->target = Vector<float>{0.0f, 0.0f, -1.0f};
	this->up = Vector<float>{0.0f, 1.0f, 0.0f};
	this->fov = 80.0f * (3.1415926f / 180.0f);
	this->cameraPos = pos;
	this->aspect = w / h;
	this->speed = 3.0f;
	this->near = 0.1f;
	this->far = 100.0f;
	cameraInit();
	cameraUpdate();
};

Camera::Camera(float w, float h, Vector<float> pos, Vector<float> target,Vector<float> up) : cameraPos(2), target(3), up(3), U(3), V(3), N(3), mousePos(2){
	this->winWidth = w;
	this->winHeight = h;
	this->cameraPos = pos;
	this->target = target;
	this->up = up;
	this->up.normalize();
	this->fov = 80.0f * (3.1415926f / 180.0f);
	this->aspect = w / h;
	this->speed = 3.0f;
	this->near = 0.1f;
	this->far = 100.0f;
	cameraInit();
	cameraUpdate();
};

void Camera::cameraInit() {
	this->angleH = -90.0f;
	this->angleV = 0.0f;
	this->upperEdge = false;
	this->leftEdge = false;
	this->rightEdge = false;
	this->lowerEdge = false;
	mousePos.x() = winWidth / 2;
	mousePos.y() = winHeight / 2;
};

void Camera::mouseActions() {
	int DeltaX = 0.0f;
	int DeltaY = 0.0f;
	SDL_GetRelativeMouseState(&DeltaX, &DeltaY);
	this->angleH += DeltaX * 0.06f;
	this->angleV -= DeltaY * 0.06f;
	if (this->angleV > 89.0f) angleV = 89.0f;
	if (this->angleV < -89.0f) angleV = -89.0f;
	cameraUpdate();
};

void Camera::cameraUpdate() {
    Vector<float> Yaxis{0.0f, 1.0f, 0.0f};
    
    target.x() = cos(utils::ToRad(angleV)) * cos(utils::ToRad(angleH));
    target.y() = sin(utils::ToRad(angleV));
    target.z() = cos(utils::ToRad(angleV)) * sin(utils::ToRad(angleH));
    target.normalize();
    
    U = cross_product(Yaxis, target);
    U.normalize();
    
    V = cross_product(target, U);
    V.normalize();
    
    N = -target;
}

Matrix<float> Camera::buildProjection() {
	return utils::perspective(fov, aspect, near, far);
};

Matrix<float> Camera::updateProjection(float w, float h) {
	aspect = w / h;
	return utils::perspective(fov, aspect, near, far);
};


Matrix<float> Camera::buildView() {
	Vector<float> center = cameraPos + target;
	return utils::view(cameraPos, center, up);
};

Matrix<float> Camera::buildViewNoTranslation() {
    Matrix<float> view = buildView();
    view[3][0] = 0.0f;
    view[3][1] = 0.0f;
    view[3][2] = 0.0f;
    return view;
}

void Camera::moveUp(float deltaTime) {
	this->cameraPos.y() += this->speed * deltaTime;
}

void Camera::moveDown(float deltaTime) {
	this->cameraPos.y() -= this->speed * deltaTime;
}

void Camera::moveForward(float deltaTime) {
    Vector<float> forwardXZ = Vector<float>{-N.x(), 0.0f, -N.z()}.normalize();
    cameraPos += forwardXZ * speed * deltaTime;
}

void Camera::moveBackward(float deltaTime) {
    Vector<float> forwardXZ = Vector<float>{-N.x(), 0.0f, -N.z()}.normalize();
    cameraPos -= forwardXZ * speed * deltaTime;
}

void Camera::moveLeft(float deltaTime) {
    Vector<float> rightXZ = cross_product(Vector<float>{0.0f, 1.0f, 0.0f}, -N).normalize();
    cameraPos += rightXZ * speed * deltaTime;
}

void Camera::moveRight(float deltaTime) {
    Vector<float> rightXZ = cross_product(Vector<float>{0.0f, 1.0f, 0.0f}, -N).normalize();
    cameraPos -= rightXZ * speed * deltaTime;
}

void Camera::speedUp() {
	this->speed += 0.1f;
	if (this->speed > 5.0f)
        this->speed = 5.0f;
	std::cout << "Speed changed to " << speed << std::endl;
}

void Camera::speedDown() {
	this->speed -= 0.1f;
	if (this->speed < 0.1f)
		this->speed = 0.1f;
	std::cout << "Speed changed to " << speed << std::endl;
}