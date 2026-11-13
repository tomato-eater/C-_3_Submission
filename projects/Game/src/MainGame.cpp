// CreateDefaultScene.cpp
// デフォルトのシーンを生成します

#include <numbers>

#include <UniDx.h>
#include <UniDx/Scene.h>
#include <UniDx/PrimitiveRenderer.h>
#include <UniDx/GltfModel.h>
#include <UniDx/Canvas.h>
#include <UniDx/TextMesh.h>
#include <UniDx/Font.h>
#include <UniDx/Image.h>
#include <UniDx/LightManager.h>

#include "CameraController.h"
#include "Player.h"
#include "LightController.h"
#include "CBController.h"

#include <thread>
#include "MainGame.h"


using namespace std;
using namespace UniDx;

namespace
{
    constexpr int mapWidth = 11;
    constexpr int mapHeight = 11;

    unique_ptr<GameObject> CreateSphere(Vector3 pos)
    {
        auto sphere = make_unique<GameObject>(u8"Sphere",
            //SphereRenderer::create<VertexPN>(u8"resource/VertexTest.hlsl"),
              SphereRenderer::create<VertexPN>(u8"resource/PixelTest1.hlsl"),
//            SphereRenderer::create<VertexPNT>(u8"resource/PixelTest2.hlsl", u8"resource/wood-2.png"),
            make_unique<SphereCollider>(Vector3::zero, 1.0f),
            make_unique<Rigidbody>()
        );
        sphere->GetComponent<Rigidbody>()->gravityScale = 0.1f;
        auto renderer = sphere->GetComponent<SphereRenderer>();

        sphere->transform->localPosition = pos;
        sphere->transform->localScale = Vector3(2, 2, 2);

        return sphere;
    }

}


void MainGame::createMap()
{
    // マテリアルの作成
    auto wallMat = std::make_shared<Material>();
    auto floorMat = std::make_shared<Material>();
    auto coinMat = std::make_shared<Material>();

    // シェーダを指定してコンパイル
    wallMat->shader->compile<VertexPNT>(u8"resource/AlbedoShadeSpec.hlsl");
    floorMat->shader->compile<VertexPNT>(u8"resource/PixelTest2.hlsl");
    coinMat->shader->compile<VertexPN>(u8"resource/ShadeSpec.hlsl");
    coinMat->color = Color(1.0f, 0.9f, 0.1f);

    // 床テクスチャ作成
    auto floorTex = std::make_shared<Texture>();
    floorTex->Load(u8"resource/wood-2.png");
    floorMat->AddTexture(std::move(floorTex));

    // 壁テクスチャ作成
    auto wallTex = std::make_shared<Texture>();
    wallTex->Load(u8"resource/wall.png");
    wallMat->AddTexture(std::move(wallTex));

    // マップ作成
    auto map = make_unique<GameObject>();

    // 床
    for(int i = 0; i < mapHeight; i++)
    {
        for(int j = 0; j < mapWidth; j++)
        {
            auto rb = make_unique<Rigidbody>();
            rb->gravityScale = 0;
            rb->mass = numeric_limits<float>::infinity();
            auto floor = make_unique<GameObject>(u8"床",
                //CubeRenderer::create<VertexPN>(u8"resource/VertexTest.hlsl"),
                CubeRenderer::create<VertexPN>(u8"resource/PixelTest1.hlsl"),
//                CubeRenderer::create<VertexPNT>(floorMat),
                move(rb),
                make_unique<AABBCollider>());
            floor->transform->localScale = Vector3(2, 2, 2);
            floor->transform->localPosition = Vector3(
                i * 2 - float(mapWidth / 2) * 2,
                -1.0f,
                j * -2 + float(mapHeight / 2) * 2);

            // 親をマップにする
            Transform::SetParent(move(floor), map->transform);
        }
    }

    // 壁
    for(int i = 0; i < 4; i++)
    {
        auto rb = make_unique<Rigidbody>();
        rb->gravityScale = 0;
        rb->mass = numeric_limits<float>::infinity();
        auto wall = make_unique<GameObject>(u8"壁",
            CubeRenderer::create<VertexPNT>(wallMat),
            move(rb),
            make_unique<AABBCollider>());
        auto odd = i % 2 != 0;
        wall->transform->localScale = Vector3(
            odd ? mapWidth * 2 : 2.0f, 4, !odd ? mapHeight * 2 : 2.0f);
        wall->transform->localPosition =
            Vector3(
                odd ? 0.0f : i == 0 ? -mapWidth : mapWidth,
                2.0f,
                !odd ? 0.0f : i == 1 ? -mapHeight : mapHeight);

        // 親をマップにする
        Transform::SetParent(move(wall), map->transform);
    }

    Transform::SetParent(
        CreateSphere(Vector3(0.4f, 2.5f, 1.0f)), map->transform);

    Transform::SetParent(
        CreateSphere(Vector3(-1.0f, 3.5f, 0.0f)), map->transform);

    mapObj = move(map);
}


unique_ptr<UniDx::Scene> MainGame::CreateScene()
{
    // -- CBController --
    auto cbControllerObj = make_unique<GameObject>(u8"CBController",
        make_unique<CBController>()
    );

    // -- プレイヤー --
    auto playerObj = make_unique<GameObject>(u8"プレイヤー",
        make_unique<GltfModel>(),
        make_unique<Rigidbody>(),
        make_unique<SphereCollider>(Vector3(0, 0.5f, 0)),
        make_unique<Player>()
        );
    auto model = playerObj->GetComponent<GltfModel>();
    model->Load<VertexPNT>(
        u8"resource/ModularCharacterPBR.glb",
        u8"resource/PixelTest2.hlsl");
    playerObj->transform->localPosition = Vector3(0, 0, 0);
    playerObj->transform->localRotation = Quaternion::Euler(0, 180, 0);

    // -- カメラ --
    auto cameraBehaviour = make_unique<CameraController>();
    cameraBehaviour->player = playerObj->GetComponent<Player>();

    // -- ライト --
    LightManager::getInstance()->ambientColor = Color(0.4f, 0.4f, 0.4f, 1.0f);
    auto lights = make_unique<GameObject>(u8"ライト群");
    auto light = make_unique<GameObject>(u8"ディレクショナルライト",
        make_unique<Light>(), make_unique<LightController>());
    light->transform->localPosition = Vector3(4, 3, -3);
    light->transform->localRotation = Quaternion::Euler(30, 60, 0.0f);
    Transform::SetParent(move(light), lights->transform);
    for(int i = 0; i < 2; ++i)
    {
        for(int j = 0; j < 2; ++j)
        {
            auto l = make_unique<Light>();
            l->type = LightType_Point;
            l->range = 5.0f;
            auto light = make_unique<GameObject>(u8"ポイントライト",
                move(l), make_unique<LightController>());
            light->transform->localPosition = Vector3(8.0f * j - 3.0f, 3, 8.0f * i - 3.0f);
            Transform::SetParent(move(light), lights->transform);
        }
    }

    // -- UI --
    auto font = make_shared<Font>();
    font->Load(u8"resource/M PLUS 1.spritefont");
    auto textMesh = make_unique<TextMesh>();
    textMesh->font = font;
    textMesh->text = u8"WASD:いどう\nIJKL:カメラ";

    auto textObj = make_unique<GameObject>(u8"テキスト", textMesh);
    textObj->transform->localPosition = Vector3(100, 20, 0);
    textObj->transform->localScale = Vector3(0.6f, 0.6f, 1.0f);

    auto scoreMesh = make_unique<TextMesh>();
    scoreMesh->font = font;
    scoreMesh->text = u8"0";
    scoreTextMesh = scoreMesh.get();

    auto scoreTextObj = make_unique<GameObject>(u8"スコア", scoreMesh);
    scoreTextObj->transform->localPosition = Vector3(480, 20, 0);

    auto canvas = make_unique<Canvas>();
    canvas->LoadDefaultMaterial(u8"resource");

    // -- マップデータ --
    createMap();

    // シーンを作って戻す
    return make_unique<Scene>(

        move(cbControllerObj),
 
        make_unique<GameObject>(u8"オブジェクトルート",
            move(playerObj),
            move(mapObj)
        ),

        move(lights),

        make_unique<GameObject>(u8"カメラルート", Vector3(0, 3, -5),
            make_unique<Camera>(),
            move(cameraBehaviour)
        ),

        make_unique<GameObject>(u8"UI",
            move(canvas),
            move(textObj),
            move(scoreTextObj)
        )
    );
}


MainGame::~MainGame()
{
}


void MainGame::AddScore(int n)
{
    score += n;
    scoreTextMesh->text = ToString(score);
}
