#pragma once

#include "Scene/BaseScene.h"

class DemoScene:
	public argent::scene::BaseScene
{
public:
	DemoScene():
		BaseScene("Demo")
	{}

	void Awake() override;
};

