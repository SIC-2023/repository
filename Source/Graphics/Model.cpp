#include "Model.h"

#include "Mesh.h"
#include "IndexBuffer.h"

#include "imgui.h"

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
				m->OnGui();

				for(auto& n : data_.nodes_)
				{
					n.OnGui();
				}
			}
			ImGui::TreePop();
		}
	}
}
