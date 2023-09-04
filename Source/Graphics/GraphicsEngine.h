#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <memory>
#include <wrl.h>

#include "../SubSystem/Subsystem.h"

#include "Dx12/Descriptor.h"
#include "Dx12/DescriptorHeap.h"

#include "FrameResource.h"


//todo �ʂ̏ꏊ�ɒu��
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

	//�`��API�����̃N���X
	//�`��֘A�̂��ׂĂ����̃N���X�ŊǗ�����
	class GraphicsEngine final :
		public Subsystem
	{
	public:
		//�^ / �G�C���A�X / enum / struct
		friend Application;

	public:
		//static�萔
		static constexpr int kNumBackBuffers{ 3 };	//�g���v���o�b�t�@�����O
		static constexpr FLOAT kClearColor[4]{ 1.0f, 0.0f, 0.0f, 1.0f };

	public:

		GraphicsEngine() = default;

		//�R�s�[�֎~�𖾎�
		GraphicsEngine(const GraphicsEngine&) = delete;
		GraphicsEngine(const GraphicsEngine&&) = delete;
		GraphicsEngine& operator=(const GraphicsEngine&) = delete;
		GraphicsEngine& operator=(const GraphicsEngine&&) = delete;

		~GraphicsEngine() override = default;

	public:
		/**
		 * \brief ����N�����ɌĂ΂��@
		 */
		void OnAwake() override;

		/**
		 * \brief �A�v���I�����ɌĂ΂��
		 */
		void OnShutdown() override;

		/**
		 * \brief �`��J�n
		 * ���݂̃o�b�N�o�b�t�@���`��I������܂őҋ@�A
		 * �o�b�N�o�b�t�@�������_�[�^�[�Q�b�g�Ƃ��ăZ�b�g�A�N���A
		 * �f�B�X�N���v�^�q�[�v���Z�b�g����
		 * \return 
		 */
		RenderContext Begin() const;

		/**
		 * \brief �`��I��
		 * �R�}���h�L���[����`�施�߂��o��
		 * �t�F���X�o�����[���X�V
		 * \param render_context 
		 */
		void End(const RenderContext& render_context);

		//DirectX12API�쐬�֐�
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

		//dxgi�֘A
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgi_factory_{};
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swap_chain_{};

		//DirectX12�֘A
		Microsoft::WRL::ComPtr<ID3D12Device8> device_{};
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> rendering_command_queue_{};
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> upload_command_queue_{};
		std::unique_ptr<GraphicsCommandList> graphics_command_lists_[kNumBackBuffers]{};
		std::unique_ptr<dx12::DescriptorHeap> descriptor_heaps_[static_cast<int>(dx12::DescriptorHeap::HeapType::Count)]{};

		Microsoft::WRL::ComPtr<ID3D12Fence> fence_{};
		D3D12_VIEWPORT viewport_{};
		D3D12_RECT scissor_rect_{};

		//�t���[�����\�[�X
		std::unique_ptr<FrameResource> frame_resources_[kNumBackBuffers]{};

		//���̑��K�v�ȕϐ�
		GraphicsContext graphics_context_;

		float window_width_{};
		float window_height_{};
		uint32_t current_back_buffer_index_{};
		uint64_t fence_value_{};
		uint64_t fence_values_[kNumBackBuffers]{};

		//TODO ImGui�͂ǂ��ɒu���̂�����??
		std::unique_ptr<ImGuiController> im_gui_controller_{};
	};
}

