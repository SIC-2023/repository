#pragma once

#include <memory>

#include "../SubSystem/Subsystem.h"
#include "BaseScene.h"

namespace argent::graphics
{
	class RenderContext;
}
namespace argent::scene
{
	class SceneManager final:
		public Subsystem
	{
	public:
		SceneManager() = default;

		virtual ~SceneManager() = default;

		void OnAwake() override;
		void OnShutdown() override;

		void Update();

		void Render(const graphics::RenderContext& render_context);
	private:

		//todo
		std::unique_ptr<BaseScene> scene_;
	};
}