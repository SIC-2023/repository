#pragma once

#include "../SubSystem/Subsystem.h"

#include "PostProcessManager.h"

#include "../Graphics/GraphicsUtility.h"
#include "../Graphics/FrameResource.h"
#include "../Graphics/FrameBuffer.h"
#include "../Graphics/GraphicsPipelineState.h"

#include "../Component/RendererComponent.h"

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

		void Awake() override;
		void Shutdown() override;

		void Execute(const RenderContext& render_context, scene::BaseScene* scene);

		bool GetIsPostProcessing() const { return is_post_processing_; }
		void SetIsPostProcessing(bool b) { is_post_processing_ = b; }

		void Register(component::RendererComponent* com) { renderer_components_.emplace_back(com); }
	private:
		PostProcessManager post_process_manager_;
		bool is_post_processing_ = true;

		std::unique_ptr<graphics::FrameResource> frame_resource_[graphics::kNumBackBuffers];
		std::unique_ptr<graphics::FrameBuffer> frame_buffers_[graphics::kNumBackBuffers];
		std::unique_ptr<graphics::GraphicsPipelineState> graphics_pipeline_state_{};

		std::vector<component::RendererComponent*> renderer_components_;
	};
}

