#include "DemoScene.h"

#include "Input/InputManager.h"
#include "Core/Engine.h"

#include "Graphics/GraphicsEngine.h"
#include "../Assets/Shader/Constant.h"
#include "Rendering/RenderContext.h"
#include "ModelImpoter.h"
#include "imgui.h"

#include "Component/ISpriteRenderer.h"
#include "Component/IStaticMeshRenderer.h"
#include "Rendering/StaticMeshRenderer.h"

void DemoScene::Awake()
{
	BaseScene::Awake();

	auto game_object = AddObject("Mountain");
	game_object->AddComponent<argent::component::IStaticMeshRenderer>();
}