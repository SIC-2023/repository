#pragma once

#include <DirectXMath.h>


#include "../SubSystem/Subsystem.h"

#include "../Graphics/ImGuiController.h"

#include "../Rendering/RenderContext.h"

namespace argent::editor
{
	class Editor final : public Subsystem
	{
	private:
		struct Camera
		{
			DirectX::XMFLOAT3 position_;
			DirectX::XMFLOAT3 rotation_;
			float near_z_;
			float far_z_;
			float aspect_ratio_;
			float fov_angle_;
		};

	public:
		Editor() = default;
		~Editor() override = default;

		void OnAwake() override;
		void OnShutdown() override;

		void Begin(const rendering::RenderContext& render_context);
		void End(const rendering::RenderContext& render_context);

		void Render();
	private:
		Camera editor_camera_;

		//TODO ImGuiÇÕÇ«Ç±Ç…íuÇ≠ÇÃÇ™ê≥â??
		std::unique_ptr<graphics::ImGuiController> imgui_controller_;
	};
}

