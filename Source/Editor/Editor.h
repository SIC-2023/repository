#pragma once

#include <DirectXMath.h>

#include "../SubSystem/Subsystem.h"

#include "EditorWindow.h"

#include "../Graphics/ImGuiController.h"

#include "../Rendering/RenderContext.h"

namespace argent::graphics
{
	class Texture;
}

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

		void Awake() override;
		void Shutdown() override;

		void OnRender(const rendering::RenderContext& render_context, uint64_t scene_srv_heap_index);

		bool GetIsPlay() const { return is_play_; }
		bool GetIsPause() const { return is_pause_; }

		//void OnPlay() { is_play_ = true; is_pause_ = false; }
		//void OffPlay() { is_play_ = is_pause_ = false; }
		//void OnPause() { is_pause_ = true; }


		static uint64_t GetSceneSrvHeapIndex() { return scene_srv_heap_index_; }
	private:
		template<class T>
		void Register();

	private:
		Camera editor_camera_;

		graphics::ImGuiController imgui_controller_;

		std::vector<std::unique_ptr<EditorWindow>> editor_windows_;

		//TODO ï ÇÃÇ¢Ç¢ï˚ñ@ÇçlÇ¶ÇÈ
		static uint64_t scene_srv_heap_index_;

		bool is_play_;
		bool is_pause_;
		std::unique_ptr<graphics::Texture> play_texture_;
		std::unique_ptr<graphics::Texture> pause_texture_;
	};

	template <class T>
	void Editor::Register()
	{
		static_assert(std::is_base_of<EditorWindow, T>::value, "EditorWindowÇ≈ÇÕÇ»Ç¢å^Ç™éwíËÇ≥ÇÍÇ‹ÇµÇΩ");
		editor_windows_.emplace_back(std::make_unique<T>());
	}
}

