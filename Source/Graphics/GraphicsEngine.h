#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <memory>
#include <wrl.h>

#include "../SubSystem/Subsystem.h"

#include "Dx12/Descriptor.h"
#include "Dx12/DescriptorHeap.h"

#include "FrameResource.h"


//todo 別の場所に置く
#include "../SpriteRenderer.h"
#include "ImGuiController.h"
#include "RenderContext.h"



namespace argent
{
	class Application;
}

namespace argent::graphics
{
	struct GraphicsContext
	{
		ID3D12Device8* device_;
		//ID3D12CommandQueue* rendering_queue_;
		ID3D12CommandQueue* upload_queue_;
		dx12::DescriptorHeap* cbv_srv_uav_heap_;
		dx12::DescriptorHeap* dsv_heap_;
		dx12::DescriptorHeap* rtv_heap_;
	};

	//描画API統括のクラス
	//描画関連のすべてをこのクラスで管理する
	class GraphicsEngine final :
		public Subsystem
	{
	public:
		//型 / エイリアス / enum / struct
		friend Application;

	public:
		//static定数
		static constexpr int kNumBackBuffers{ 3 };	//トリプルバッファリング
		static constexpr FLOAT kClearColor[4]{ 1.0f, 0.0f, 0.0f, 1.0f };

	public:

		GraphicsEngine() = default;

		//コピー禁止を明示
		GraphicsEngine(const GraphicsEngine&) = delete;
		GraphicsEngine(const GraphicsEngine&&) = delete;
		GraphicsEngine& operator=(const GraphicsEngine&) = delete;
		GraphicsEngine& operator=(const GraphicsEngine&&) = delete;

		~GraphicsEngine() override = default;

	public:
		/**
		 * \brief 初回起動時に呼ばれる　
		 */
		void OnAwake() override;

		/**
		 * \brief アプリ終了時に呼ばれる
		 */
		void OnShutdown() override;

		/**
		 * \brief 描画開始
		 * 現在のバックバッファが描画終了するまで待機、
		 * バックバッファをレンダーターゲットとしてセット、クリア
		 * ディスクリプタヒープをセットする
		 * \return 
		 */
		RenderContext Begin() const;

		/**
		 * \brief 描画終了
		 * コマンドキューから描画命令を出す
		 * フェンスバリューを更新
		 * \param render_context 
		 */
		void End(const RenderContext& render_context);

		//DirectX12API作成関数
		HRESULT FindAdapter(IDXGIAdapter1** pp_adapter) const;
		HRESULT CreateDevice();
		HRESULT CreateSwapChain(HWND hwnd);

		//ID3D12Device8* GetDevice() const { return device_.Get(); }
		//ID3D12CommandQueue* GetCommandQueue() const { return command_queue_.Get(); }
		//ID3D12GraphicsCommandList6* GetCommandList() const { return graphics_command_lists_[current_back_buffer_index_]->GetCommandList(); }
		//dx12::DescriptorHeap* GetDescriptorHeap(dx12::DescriptorHeap::HeapType heap_type) const
		//{
		//	return descriptor_heaps_[static_cast<int>(heap_type)].get();
		//}
		D3D12_VIEWPORT GetViewport() const { return viewport_; }
		D3D12_RECT GetScissorRect() const { return scissor_rect_; }
		uint32_t GetCurrentBackBufferIndex() const { return current_back_buffer_index_; }
		uint64_t GetFenceValue() const { return fence_value_; }
		float GetWindowWidth() const { return window_width_; }
		float GetWindowHeight() const { return window_height_; }
		uint64_t GetSceneConstantHeapIndex() const { return frame_resources_[current_back_buffer_index_]->GetSceneConstantHeapIndex(); }
		void UpdateSceneConstantBuffer(const SceneConstant& scene_constant) const { frame_resources_[current_back_buffer_index_]->UpdateSceneConstantBuffer(scene_constant); }
		void UpdateFrustumConstantBuffer(const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& camera_position) const { frame_resources_[current_back_buffer_index_]->UpdateFrustumConstantBuffer(view_projection, camera_position); }

		[[nodiscard]] const GraphicsContext& GetGraphicsContext() const { return graphics_context_; }
	private:

		//dxgi関連
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgi_factory_{};
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swap_chain_{};

		//DirectX12関連
		Microsoft::WRL::ComPtr<ID3D12Device8> device_{};
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> rendering_command_queue_{};
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> upload_command_queue_{};
		std::unique_ptr<GraphicsCommandList> graphics_command_lists_[kNumBackBuffers]{};
		std::unique_ptr<dx12::DescriptorHeap> descriptor_heaps_[static_cast<int>(dx12::DescriptorHeap::HeapType::Count)]{};

		Microsoft::WRL::ComPtr<ID3D12Fence> fence_{};
		D3D12_VIEWPORT viewport_{};
		D3D12_RECT scissor_rect_{};

		//フレームリソース
		std::unique_ptr<FrameResource> frame_resources_[kNumBackBuffers]{};

		//その他必要な変数
		GraphicsContext graphics_context_;

		float window_width_{};
		float window_height_{};
		uint32_t current_back_buffer_index_{};
		uint64_t fence_value_{};
		uint64_t fence_values_[kNumBackBuffers]{};

		//TODO ImGuiはどこに置くのが正解??
		std::unique_ptr<ImGuiController> im_gui_controller_{};
	};
}

