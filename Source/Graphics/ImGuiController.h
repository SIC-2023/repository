#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "Dx12/Descriptor.h"
#include "GraphicsPipelineState.h"

namespace argent
{
	class Application;
}

namespace argent::graphics
{
	
	class ImGuiController
	{
	public:
		ImGuiController();
		virtual ~ImGuiController();

	public:

		static void UpdateImGuiMouseData();
	public:
		static HWND hwnd_;
		static int mouse_tracked_area_;
		static int mouse_buttons_down_;
		static bool want_update_monitors_;
		static const int MaxNumVertex = 10000;
		static const int MaxNumIndex = 30000;
	

		static LRESULT ImGuiWndProcHandler(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

	public:
		void Awake(HWND hwnd, ID3D12Device* device, const dx12::Descriptor& srv_descriptor,
				UINT descriptor_increment_size, SIZE_T descriptor_heap_pointer_start);
		void OnShutdown();

		void Begin(float window_width, float window_height);
		void End(ID3D12GraphicsCommandList* command_list, UINT current_back_buffer_index);

	private:



	private:
		HRESULT CreateFontTexture(ID3D12Device* device);
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> font_texture_resource_;
		dx12::Descriptor font_srv_descriptor_;
		std::unique_ptr<GraphicsPipelineState> graphics_pipeline_state_;

		//頂点バッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> vertex_buffer_[3];
		//UINT vertex_buffer_size_;
		Microsoft::WRL::ComPtr<ID3D12Resource> index_buffer_[3];
		//UINT index_buffer_size_;
		UINT descriptor_increment_size_;
		SIZE_T descriptor_heap_gpu_pointer_start_;
	};
}
