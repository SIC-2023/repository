#pragma once
#include <memory>

#include "RendererComponent.h"

namespace argent::rendering
{
	class StaticMeshRenderer;
}
namespace argent::component
{
	class IStaticMeshRenderer final :
		public RendererComponent
	{
	public:
		IStaticMeshRenderer();

		~IStaticMeshRenderer() override = default;;

		void Update() override;
		void OnRender(const rendering::RenderContext& render_context) override;

		void OnGui() override;
		
	private:
		std::unique_ptr<rendering::StaticMeshRenderer> static_mesh_renderer_;
	};
}

