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

		virtual ~SceneManager() override = default;

		void OnAwake() override;
		void OnShutdown() override;

		void Update();

		void Render(const rendering::RenderContext& render_context);

		BaseScene* GetCurrentScene() const { return scene_.get(); }
	private:

		//TODO •¡”ƒV[ƒ“‚Ö‚Ì‘Î‰@
		std::unique_ptr<BaseScene> scene_;
	};
}