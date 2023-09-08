#pragma once

#include <DirectXMath.h>


#include "../SubSystem/Subsystem.h"

#include "EditorWindow.h"

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

		void OnRender(const rendering::RenderContext& render_context);

	private:
		template<class T>
		void Register();
	private:
		Camera editor_camera_;

		graphics::ImGuiController imgui_controller_;

		std::vector<std::unique_ptr<EditorWindow>> editor_windows_;
	};

	template <class T>
	void Editor::Register()
	{
		editor_windows_.emplace_back(std::make_unique<T>());
	}
}

