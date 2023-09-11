#pragma once

#include "../SubSystem/Subsystem.h"

#include "PostProcessManager.h"

#include "../Graphics/GraphicsUtility.h"
#include "../Graphics/FrameResource.h"
#include "../Graphics/FrameBuffer.h"
#include "../Graphics/GraphicsPipelineState.h"

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

	private:
		PostProcessManager post_process_manager_;

		std::unique_ptr<graphics::FrameResource> frame_resource_[graphics::kNumBackBuffers];
		std::unique_ptr<graphics::FrameBuffer> frame_buffers_[graphics::kNumBackBuffers];
		std::unique_ptr<graphics::GraphicsPipelineState> graphics_pipeline_state_{};
	};
}

