#include "Model.h"

#include "../Core/Engine.h"

#include "imgui.h"


#include "GraphicsEngine.h"
#include "Mesh.h"
#include "IndexBuffer.h"


namespace argent::graphics
{
	void Model::Node::OnGui()
	{
		if(ImGui::TreeNode(name_.c_str()))
		{
			for(auto& c : children_)
			{
				c.OnGui();
			}
			ImGui::TreePop();
		}
	}

	//Model::Model(ID3D12Device* device, const Data& model_data)
	//{
	//	data_ = model_data;
	//	//TODO C³
	//	auto graphics = GetEngine()->GetSubsystemLocator().Get<GraphicsEngine>();
	//	ID3D12CommandQueue* command_queue = graphics->GetCommandQueue();

	//	for(const auto& d : model_data.mesh_data_)
	//	{
	//		meshes_.emplace_back(std::make_shared<Mesh>(device, d));
	//	}

	//	for(const auto& d : model_data.material_data_)
	//	{
	//		materials_.emplace_back(std::make_shared<PbrMaterial>(device, command_queue, graphics->GetDescriptorHeap(dx12::DescriptorHeap::HeapType::CbvSrvUav), d));
	//	}
	//}

	Model::Model(const GraphicsContext& graphics_context, const Data& model_data)
	{
		data_ = model_data;
		for (const auto& d : model_data.mesh_data_)
		{
			meshes_.emplace_back(std::make_shared<Mesh>(graphics_context, d));
		}

		for (const auto& d : model_data.material_data_)
		{
			materials_.emplace_back(std::make_shared<PbrMaterial>(graphics_context, d));
		}
	}

	void Model::OnGui()
	{
		if(ImGui::TreeNode("Model"))
		{
			for(auto& m : materials_)
			{
				uint64_t base_index = static_cast<uint64_t>(m->GetBaseIndex());
				uint64_t normal_index = static_cast<uint64_t>(m->GetNormalIndex());
				ImGui::Image(reinterpret_cast<ImTextureID>(base_index), ImVec2(256, 256));
				ImGui::Image(reinterpret_cast<ImTextureID>(normal_index), ImVec2(256, 256));

				for(auto& n : data_.nodes_)
				{
					n.OnGui();
				}
			}
			ImGui::TreePop();
		}
	}
}
