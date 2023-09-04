#pragma once
#include <d3d12.h>

namespace argent::graphics::dx12
{
	/**
	 * \brief �f�v�X�X�e���V���o�b�t�@�ƃr���[���쐬����
	 * \param device d3d12�̃f�o�C�X
	 * \param width �o�b�t�@�̕�
	 * \param height �o�b�t�@�̍���
	 * \param pp_depth_stencil_resource �쐬�������\�[�X���i�[���� 
	 * \param cpu_descriptor_handle ���\�[�X��cpu�n���h��
	 * \return �����������ǂ���
	 */
	HRESULT CreateDepthStencilBufferAndView(ID3D12Device* device, UINT64 width, UINT height,
	                                        ID3D12Resource** pp_depth_stencil_resource, D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor_handle);

	//todo �u���ꏊ�Ԉ���Ă�Ǝv��
	/**
	 * \brief �g�����W�V�������\�[�X�o���A�𒣂�
	 * \param command_list ���߂��悹��R�}���h���X�g
	 * \param state_before ���݂̃��\�[�X�X�e�[�g
	 * \param state_after �ύX��̃��\�[�X�X�e�[�g
	 * \param resource �Ώۂ̃��\�[�X
	 */
	void SetTransitionResourceBarrier(ID3D12GraphicsCommandList* command_list,
	                                  D3D12_RESOURCE_STATES state_before, D3D12_RESOURCE_STATES state_after,
	                                  ID3D12Resource* resource);

	//---------------------------------�u�����h���[�h---------------------------------//

	enum class BlendMode { Alpha, Add };
	D3D12_BLEND_DESC GetBlendDesc(BlendMode blend_mode, bool alpha_to_coverage_enable = false, bool independent_blend_enable = false);

	/**
	 * \brief �u�����h�f�X�N���쐬
	 * \param num_render_targets �����_�[�^�[�Q�b�g�̐�
	 * \param blend_mode �����_�[�^�[�Q�b�g�̐������K�v
	 * \param alpha_to_coverage_enable [in]
	 * \param independent_blend_enable [in]
	 * \return D3D12_BLEND_DESC
	 */
	D3D12_BLEND_DESC GetBlendDesc(UINT num_render_targets, const BlendMode blend_mode[],
		bool alpha_to_coverage_enable = false, bool independent_blend_enable = false);

	//---------------------------------���X�^���C�U�X�e�[�g---------------------------------//

	enum class RasterizerMode { CullNoneSolid, CullNoneWireFrame, CullBackSolid,
		CullBackWireFrame, CullFrontSolid, CullFrontWireFrame };
	D3D12_RASTERIZER_DESC GetRasterizerDesc(RasterizerMode rasterizer_mode, bool front_counter_clockwise = true);

	//---------------------------------�f�v�X�X�e���V���X�e�[�g---------------------------------//

	enum class DepthMode { TestOnWriteOn, TestOnWriteOff, TestOffWriteOn, TestOffWriteOff };
	D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc(DepthMode depth_mode);


	//---------------------------------�V�F�[�_�[���t���N�V����---------------------------------//

	DXGI_FORMAT GetDxgiFormat(D3D_REGISTER_COMPONENT_TYPE type, BYTE mask);


}