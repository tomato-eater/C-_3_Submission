#pragma once

#include <UniDx.h>


class ShpereController : public UniDx::Behaviour
{
public:
    virtual void Update() override;

private:
    float pitch;
    float yaw;
    float length;
};