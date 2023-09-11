#pragma once

#include "../Graphics/GraphicsContext.h"
#include "RenderContext.h"

namespace argent::rendering
{
	/**
	 * \brief ポストエフェクトの実行と管理
	 */
	class PostProcessManager
	{
	public:
		PostProcessManager() = default;
		virtual ~PostProcessManager() = default;

		void OnAwake(const graphics::GraphicsContext& graphics_context);
		void OnShutdown();

		void Execute(const RenderContext& render_context, uint32_t scene_srv_heap_index);

		//TODO
		uint64_t GetSrvHeapIndex() const { return result_srv_heap_index_; }
	private:
		uint64_t result_srv_heap_index_;
	};
}

