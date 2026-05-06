#include "camera.h"

void Camera::_bind_methods() {}

void Camera::_ready() {

}

void Camera::set_target(Node3D *new_target) {
    target = new_target;
}
