#include "GraphicsEngine.h"

#include <d3d12.h>
#include <iostream>

#include <string>

#include "../Core/Engine.h"
#include "../Core/Window.h"

#include "../Utility/Misc.h"
#include "Dx12/DescriptorHeap.h"

namespace argent::graphics
{
	void GraphicsEngine::OnAwake()
	{
		const auto& window = GetEngine()->GetSubsystemLocator().Get<Window>();

		if(!window)
		{
			_ASSERT_EXPR(FALSE, L"Windowがnullptrでした");
		}

		window_width_ = static_cast<float>(window->GetWindowWidth());
		window_height_ = static_cast<float>(window->GetWindowHeight());
		const HWND hwnd = window->GetHwnd();

		HRESULT hr{};

		//ファクトリの作成
		UINT factory_creation_flags{ 0 };
#ifdef _DEBUG
		factory_creation_flags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		Microsoft::WRL::ComPtr<IDXGIFactory2> tmp_factory;
		hr = CreateDXGIFactory2(factory_creation_flags, IID_PPV_ARGS(tmp_factory.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);
		hr = tmp_factory->QueryInterface(IID_PPV_ARGS(dxgi_factory_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);

		//デバイスの作成
		hr = CreateDevice();
		_ARGENT_ASSERT_EXPR(hr);
		device_->SetName(L"Dx12Device");

		//デバッグ表示用
#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12DebugDevice2> debug_device;
		hr = device_->QueryInterface(IID_PPV_ARGS(debug_device.ReleaseAndGetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		debug_device->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
#endif

		//コマンドの作成
		D3D12_COMMAND_QUEUE_DESC command_queue_desc{};
		command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		command_queue_desc.NodeMask = 0u;
		command_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		hr = device_->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(rendering_command_queue_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);
		hr = device_->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(upload_command_queue_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);
		rendering_command_queue_->SetName(L"RenderingCommandQueue");
		upload_command_queue_->SetName(L"ResourceUploadCommandQueue");

		for(int i = 0; i < kNumBackBuffers; ++i)
		{
			graphics_command_lists_[i] = std::make_unique<GraphicsCommandList>(device_.Get());
		}

		//フェンスの作成
		hr = device_->CreateFence(0u, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);

		//スワップチェーンの作成
		hr = CreateSwapChain(hwnd);
		_ARGENT_ASSERT_EXPR(hr);

		//デスクリプタヒープの作成
		constexpr int kNumDescriptors[static_cast<int>(dx12::DescriptorHeap::HeapType::Count)]{ 10000, 20, 100, 200 };
		for(int i = 0; i < static_cast<int>(dx12::DescriptorHeap::HeapType::Count); ++i)
		{
			descriptor_heaps_[i] = std::make_unique<dx12::DescriptorHeap>(device_.Get(),
				static_cast<dx12::DescriptorHeap::HeapType>(i), kNumDescriptors[i]);
		}

		//フレームリソースの作成
		for(int i = 0; i < kNumBackBuffers; ++i)
		{
			frame_resources_[i] = std::make_unique<FrameResource>(device_.Get(),
				swap_chain_.Get(), i, 
				descriptor_heaps_[static_cast<int>(dx12::DescriptorHeap::HeapType::Rtv)]->PopDescriptor(),
				descriptor_heaps_[static_cast<int>(dx12::DescriptorHeap::HeapType::Dsv)]->PopDescriptor(), 
				descriptor_heaps_[static_cast<int>(dx12::DescriptorHeap::HeapType::CbvSrvUav)]->PopDescriptor(),
				descriptor_heaps_[static_cast<int>(dx12::DescriptorHeap::HeapType::CbvSrvUav)]->PopDescriptor());
		}

		//ビューポート、レクト、その他変数
		viewport_ = D3D12_VIEWPORT(0.f, 0.f, window_width_, window_height_, 0.f, 1.f);
		scissor_rect_ = D3D12_RECT(0u, 0u, static_cast<LONG>(window_width_), static_cast<LONG>(window_height_));
		current_back_buffer_index_ = 0u;
		fence_value_ = 0u;

		graphics_context_.device_ = device_.Get();
		//graphics_context_.rendering_queue_ = rendering_command_queue_.Get();
		graphics_context_.upload_queue_ = upload_command_queue_.Get();
		graphics_context_.cbv_srv_uav_heap_ = descriptor_heaps_[static_cast<int>(dx12::DescriptorHeap::HeapType::CbvSrvUav)].get();
		graphics_context_.dsv_heap_ = descriptor_heaps_[static_cast<int>(dx12::DescriptorHeap::HeapType::Dsv)].get();
		graphics_context_.rtv_heap_ = descriptor_heaps_[static_cast<int>(dx12::DescriptorHeap::HeapType::Rtv)].get();

		//ここからはオプション
		im_gui_controller_ = std::make_unique<ImGuiController>();
		//auto* descriptor_heap = GetDescriptorHeap(dx12::DescriptorHeap::HeapType::CbvSrvUav);
		auto* descriptor_heap = descriptor_heaps_[static_cast<int>(dx12::DescriptorHeap::HeapType::CbvSrvUav)].get();
		im_gui_controller_->Initialize(hwnd, device_.Get(), 
			descriptor_heap->PopDescriptor(), 
			descriptor_heap->GetIncrementSize(), descriptor_heap->GetGpuHandleStart());


		//TODO 別の場所に
		hr = device_->CreatePipelineLibrary(nullptr, 0, IID_PPV_ARGS(pipeline_library_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);
	}

	void GraphicsEngine::OnShutdown()
	{
		//TODO ImguiはEditorとして別のサブシステムにするべきかも

		//すべての描画が終了するまで待機する
		uint64_t fence_value = 0u;
		for(UINT i = 0; i < kNumBackBuffers; ++i)
		{
			//フェンスバリューの最大値を格納
			if(fence_value < fence_values_[i])
			{
				fence_value = fence_values_[i];
			}
		}

		//描画終了するまで待機
		if(fence_->GetCompletedValue() < fence_value)
		{
			const HANDLE fence_event{};
			fence_->SetEventOnCompletion(fence_value, fence_event);
			WaitForSingleObject(fence_event, INFINITE);
		}

		im_gui_controller_->Finalize();
	}

	RenderContext GraphicsEngine::Begin() const
	{
		//描画終了まで待機
		if(fence_->GetCompletedValue() < fence_values_[current_back_buffer_index_])
		{
			const HANDLE fence_event{};
			fence_->SetEventOnCompletion(fence_value_, fence_event);
			WaitForSingleObject(fence_event, INFINITE);
		}

		const RenderContext render_context(this, current_back_buffer_index_, 
			static_cast<uint32_t>(frame_resources_[current_back_buffer_index_]->GetSceneConstantHeapIndex()),
			static_cast<uint32_t>(frame_resources_[current_back_buffer_index_]->GetFrustumConstantHeapIndex()),
			graphics_command_lists_[current_back_buffer_index_].get(),
			viewport_);
		render_context.Begin();

		frame_resources_[current_back_buffer_index_]->Begin(graphics_command_lists_[current_back_buffer_index_]->GetCommandList(), viewport_, scissor_rect_);

		//ディスクリプタヒープをセットする
		ID3D12DescriptorHeap* descriptor_heaps[]
		{
			descriptor_heaps_[static_cast<int>(dx12::DescriptorHeap::HeapType::CbvSrvUav)]->GetD3d12DescriptorHeap()
		};
		render_context.GetCommandList()->SetDescriptorHeaps(1u, descriptor_heaps);


		//TODO ImGui
		im_gui_controller_->Begin(window_width_, window_height_);


		return render_context;
	}

	void GraphicsEngine::End(const RenderContext& render_context)
	{
		im_gui_controller_->End(device_.Get(), render_context.GetCommandList(),
			current_back_buffer_index_);

		frame_resources_[current_back_buffer_index_]->End(render_context.GetCommandList());
		render_context.End(rendering_command_queue_.Get());

		rendering_command_queue_->Signal(fence_.Get(), ++fence_value_);
		fence_values_[current_back_buffer_index_] = fence_value_;
		swap_chain_->Present(0u, 0u);
		++current_back_buffer_index_;
		if(current_back_buffer_index_ >= kNumBackBuffers)
		{
			current_back_buffer_index_ = 0u;
		}
	}

	//ヘルパー系
	HRESULT GraphicsEngine::FindAdapter(IDXGIAdapter1** pp_adapter) const
	{
		HRESULT hr{ S_OK };
		UINT adapter_index = 0;
		while (true)
		{
			hr = dxgi_factory_->EnumAdapterByGpuPreference(adapter_index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(pp_adapter));
			if (FAILED(hr)) break;

			DXGI_ADAPTER_DESC1 adapterDesc;
			(*pp_adapter)->GetDesc1(&adapterDesc);

			// ソフトウェアアダプター
			if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

			// デバイスを生成できるかチェック	shader model 6_6 は12_2以上のみらしい
			hr = D3D12CreateDevice(*pp_adapter, D3D_FEATURE_LEVEL_12_2, _uuidof(ID3D12Device), nullptr);  // NOLINT(clang-diagnostic-language-extension-token)
			if (SUCCEEDED(hr)) return hr;

			adapter_index++;
		}

		// アダプタの取得
		if (*pp_adapter == nullptr)
		{
			hr = dxgi_factory_->EnumAdapters1(0, pp_adapter);
			if (FAILED(hr))
			{
				// 取得できない場合はWarpアダプタを取得
				return dxgi_factory_->EnumWarpAdapter(IID_PPV_ARGS(pp_adapter));
			}
		}
		return E_FAIL;
	}

	HRESULT GraphicsEngine::CreateDevice()
	{
		HRESULT hr{ S_OK };
		//デバイスの作成
		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
		hr = FindAdapter(adapter.ReleaseAndGetAddressOf());
		_ARGENT_ASSERT_EXPR(hr);

		DXGI_ADAPTER_DESC1 adapter_desc{};
		adapter->GetDesc1(&adapter_desc);

		//バインドレス　shader model 6_6対応のもの
		//ノートパソコンだとフィーチャーレベルを下げても動いた　多分GPU12_2に対応してるから
		hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(device_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);
		return hr;
	}

	HRESULT GraphicsEngine::CreateSwapChain(HWND hwnd)
	{
		HRESULT hr{ S_OK };
		//スワップチェーンの作成
		DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1{};
		swap_chain_desc1.Width = static_cast<UINT>(window_width_);
		swap_chain_desc1.Height = static_cast<UINT>(window_height_);
		swap_chain_desc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc1.Stereo = FALSE;
		swap_chain_desc1.SampleDesc.Count = 1;
		swap_chain_desc1.SampleDesc.Quality = 0;
		swap_chain_desc1.BufferUsage = DXGI_USAGE_BACK_BUFFER;
		swap_chain_desc1.BufferCount = kNumBackBuffers;
		swap_chain_desc1.Scaling = DXGI_SCALING_STRETCH;
		swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swap_chain_desc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swap_chain_desc1.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> tmp_swap_chain;
		hr = dxgi_factory_->CreateSwapChainForHwnd(rendering_command_queue_.Get(), hwnd,
			&swap_chain_desc1, nullptr, nullptr,
			tmp_swap_chain.ReleaseAndGetAddressOf());
		_ARGENT_ASSERT_EXPR(hr);
		hr = tmp_swap_chain->QueryInterface(IID_PPV_ARGS(swap_chain_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);
		return hr;
	}
}
