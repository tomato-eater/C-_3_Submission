#include "LightController.h"

#include <UniDx/Input.h>

using namespace UniDx;


void LightController::OnEnable()
{
    auto pos = transform->localPosition.get();
    float planar = std::sqrt(pos.x * pos.x + pos.z * pos.z);
    length = pos.magnitude();
    yaw = std::atan2(pos.x, pos.z) * Rad2Deg + 180.0f;
    pitch = std::atan2(pos.y, planar) * Rad2Deg * 1.0f;
}


void LightController::Update()
{
    const float angleSpeed = 120.0f;
    if (Input::GetKey(Keyboard::O))
    {
        yaw += angleSpeed * Time::deltaTime;
    }
    if (Input::GetKey(Keyboard::P))
    {
        yaw -= angleSpeed * Time::deltaTime;
    }
    transform->localRotation = Quaternion::Euler(pitch, yaw, 0.0f);
    transform->localPosition = transform->forward * -length;
}
