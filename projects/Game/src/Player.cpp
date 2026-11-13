#pragma once

#include "Player.h"

#include <UniDx/Input.h>
#include <UniDx/Collider.h>
#include <UniDx/Time.h>
#include <UniDx/PrimitiveRenderer.h>

#include "MainGame.h"

using namespace DirectX;
using namespace UniDx;

namespace
{
    const StringId CoinName = StringId::intern("Coin");
}


void Player::OnEnable()
{
    rb = GetComponent<Rigidbody>();
    assert(rb != nullptr);

    GetComponent<Collider>()->bounciness = 0.0f;
}


void Player::Update()
{
    const float moveSpeed = 5;

    // 操作方向
    Vector3 cont;
    if (Input::GetKey(Keyboard::A))
    {
        cont.x = -1.0f;
    }
    else if (Input::GetKey(Keyboard::D))
    {
        cont.x = 1.0f;
    }
    else
    {
        cont.x = 0.0f;
    }

    if (Input::GetKey(Keyboard::S))
    {
        cont.z = -1.0f;
    }
    else if (Input::GetKey(Keyboard::W))
    {
        cont.z = 1.0f;
    }
    else
    {
        cont.z = 0.0f;
    }
    cont = cont.normalized();

    // カメラ方向を考慮して速度ベクトルを計算
    Vector3 camF = Camera::main->transform->forward;
    float camAngle = std::atan2(camF.x, camF.z) * UniDx::Rad2Deg;
    Vector3 velocity = cont * moveSpeed * Quaternion::AngleAxis(camAngle, Vector3::up);
    velocity.y = rb->linearVelocity.y;
    float vAngle = std::atan2(velocity.x, velocity.z) * UniDx::Rad2Deg;

    rb->linearVelocity = velocity;
    if (cont != Vector3::zero)
    {
        rb->rotation = Quaternion::Euler(0, vAngle, 0);
    }
}


void Player::OnTriggerEnter(Collider* other)
{
}


void Player::OnTriggerStay(Collider* other)
{
}


void Player::OnTriggerExit(Collider* other)
{
}


// コライダーに当たったときのコールバック
void Player::OnCollisionEnter(const Collision& collision)
{
    if (collision.collider->name == CoinName)
    {
        MainGame::getInstance()->AddScore(1);
        Destroy(collision.collider->gameObject);
    }
}


void Player::OnCollisionStay(const Collision& collision)
{
}

void Player::OnCollisionExit(const Collision& collision)
{
}

