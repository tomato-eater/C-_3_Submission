#include "CBController.h"

#include <UniDx/D3DManager.h>
#include <UniDx/Input.h>
#include <UniDx/Time.h>

using namespace UniDx;

namespace
{
    // 定数バッファ（型定義自体はシェーダーに伝わらないことに注意）
    struct CBParam // register(b0)
    {
        Color baseColor;
        Vector4 myData;
    };

    constexpr int CBSlot = 0;

    Color colorTable[] =
    {
        Color(1, 1, 1),
        Color(1, 1, 0),
        Color(1, 0, 1),
        Color(0, 1, 1),
        Color(0.5, 0, 0),
        Color(0, 0.5, 0),
        Color(0, 0, 0.5),
        Color(0, 0, 0),
        Color(0, 0, 1),
        Color(0, 1, 0),
        Color(1, 0, 0),
    };
    constexpr int tableSize = sizeof(colorTable) / sizeof(Color);
}


void CBController::Start()
{
    D3D11_BUFFER_DESC desc{};
    desc.ByteWidth = sizeof(CBParam);       // 構造体のサイズを指定
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;

    // 定数バッファを確保
    D3DManager::getInstance()->GetDevice()->CreateBuffer(&desc, nullptr, constantBuffer.GetAddressOf());
}


void CBController::Update()
{
    if(Input::GetKeyDown(Keyboard::Down))
    {
        colorIndex = (colorIndex + 1) % tableSize;
    }

    if(Input::GetKeyDown(Keyboard::Up))
    {
        colorIndex--;
        if (colorIndex < 0) {
            colorIndex = tableSize - 1;
        }
    }
    if (Input::GetKey(Keyboard::Space)) {
        colorIndex = (colorIndex + 1) % tableSize;
    }

    if (Input::GetKeyDown(Keyboard::Right) && alpha > 0.1f)
    {
        alpha -= 0.1f;
    }
    if (Input::GetKeyDown(Keyboard::Left) && alpha < 1.0f) {
        alpha += 0.1f;
    }

    if (Input::GetKey(Keyboard::E)) {

        timeScale -= Time::deltaTime;
    }
    if (Input::GetKey(Keyboard::Q)) {
        timeScale += Time::deltaTime;
    }

    CBParam cb;

    // カラーテーブルから現在のインデクスの色を設定
    cb.baseColor = colorTable[colorIndex];
    cb.myData.x = timeScale;
    cb.myData.y = alpha;

    // 定数バッファを更新
    D3DManager::getInstance()->GetContext()->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);

    // シェーダーへ設定
    ID3D11Buffer* cbs[1] = { constantBuffer.Get() };
    D3DManager::getInstance()->GetContext()->VSSetConstantBuffers(CBSlot, 1, cbs);
    D3DManager::getInstance()->GetContext()->PSSetConstantBuffers(CBSlot, 1, cbs);

}