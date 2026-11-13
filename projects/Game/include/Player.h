#pragma once

#include <UniDx.h>
#include <UniDx/Rigidbody.h>


class Player : public UniDx::Behaviour
{
public:
    virtual void OnEnable() override;
    virtual void Update() override;
    virtual void OnTriggerEnter(UniDx::Collider* other) override;
    virtual void OnTriggerStay(UniDx::Collider* other) override;
    virtual void OnTriggerExit(UniDx::Collider* other) override;
    virtual void OnCollisionEnter(const UniDx::Collision& collision) override;
    virtual void OnCollisionStay(const UniDx::Collision& collision) override;
    virtual void OnCollisionExit(const UniDx::Collision& collision) override;

    UniDx::Rigidbody* rb = nullptr;

private:
};
