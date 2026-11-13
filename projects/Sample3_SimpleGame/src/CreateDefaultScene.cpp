// CreateDefaultScene.cpp
// デフォルトのシーンを生成します

#include <UniDx.h>
#include <UniDx/Scene.h>
#include <UniDx/TextMesh.h>

#include "MainGame.h"


unique_ptr<Scene> CreateDefaultScene()
{
    MainGame::create();
    return MainGame::getInstance()->CreateScene();
}


void DestroyDefaultScene()
{
    MainGame::destroy();
}