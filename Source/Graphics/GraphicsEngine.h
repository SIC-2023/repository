#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

#include <memory>

#include "../SubSystem/Subsystem.h"

#include "GraphicsUtility.h"
#include "GraphicsContext.h"


struct SceneConstant;

//Forward Declaration
namespace argent
{
	class Application;
}

namespace argent::rendering
{
	class RenderContext;
}
//Forward Declaration
namespace argent::graphics
{
	class GraphicsCommandList;
	class FrameResource;
	class ImGuiController;
}

//Forward Declaration
namespace argent::graphics::dx12
{
	class DescriptorHeap;
}

namespace argent::graphics
{
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
		//static constexpr int kNumBackBuffers{ 3 };	//�g���v���o�b�t�@�����O
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
		rendering::RenderContext Begin() const;

		/**
		 * \brief �`��I��
		 * �R�}���h�L���[����`�施�߂��o��
		 * �t�F���X�o�����[���X�V
		 * \param render_context 
		 */
		void End(const rendering::RenderContext& render_context);

		//DirectX12API�쐬�֐�
		HRESULT FindAdapter(IDXGIAdapter1** pp_adapter) const;
		HRESULT CreateDevice();
		HRESULT CreateSwapChain(HWND hwnd);

		uint64_t GetFenceValue() const { return fence_value_; }
		void UpdateSceneConstantBuffer(const SceneConstant& scene_constant) const; 
		void UpdateFrustumConstantBuffer(const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& camera_position) const;

		[[nodiscard]] const GraphicsContext& GetGraphicsContext() const { return graphics_context_; }

		//TODO �ʂ̏ꏊ�Ɉڂ�����
		ID3D12PipelineLibrary* GetPipelineLibrary() const { return pipeline_library_.Get(); }
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

		//TODO ��p�̃p�C�v���C���}�l�[�W���[�����ׂ���
		Microsoft::WRL::ComPtr<ID3D12PipelineLibrary> pipeline_library_;
	};
}

