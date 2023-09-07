#pragma once

#include "../SubSystem/Subsystem.h"

namespace argent::scene
{
	class BaseScene;
}

namespace argent::rendering
{
	class RenderContext;	
}

namespace argent::rendering
{
	class RenderingManager final: public Subsystem
	{
	public:
		RenderingManager();
		~RenderingManager() override;

		void OnAwake() override;
		void OnShutdown() override;

		void Execute(const RenderContext& render_context, scene::BaseScene* scene);
	};
}

