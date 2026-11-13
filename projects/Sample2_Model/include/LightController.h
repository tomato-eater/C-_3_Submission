#pragma once

#include <UniDx.h>

class LightController : public UniDx::Behaviour
{
public:
    virtual void OnEnable() override;
    virtual void Update() override;

private:
    float pitch;
    float yaw;
    float length;
};

