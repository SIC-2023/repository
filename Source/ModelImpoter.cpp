#include "ModelImpoter.h"

#include <filesystem>
#include <iostream>

#include "meshoptimizer.h"

#include "Graphics/StaticMeshRenderer.h"
#include "Graphics/AnimationClip.h"
#include "Graphics/Skeleton.h"

namespace argent::graphics
{
	void FetchMesh(const aiScene* ai_scene, const AssimpScene& assimp_scene, std::vector<RelayMeshData>& relay_mesh_data);
	void FetchSkeleton(const aiScene* ai_scene, const aiMesh* ai_mesh, const AssimpScene& assimp_scene, Skeleton::Data& bones);
	void FetchBoneInfluence(const aiScene* ai_scene, const aiMesh* ai_mesh, const Skeleton::Data& skeleton_data,
		std::vector<Skeleton::VertexBone>& vertex_bones);

	void FetchMaterial(const aiScene* ai_scene, std::vector<PbrMaterial::Data>& material_data, const char* filepath);
	void FetchAnimation(const aiScene* ai_scene, std::vector<AnimationClip>& animation_clips);

	void OptimizeMesh(std::vector<RelayMeshData>& mesh_data);
	void GenerateMeshlets(std::vector<RelayMeshData>& mesh_data);

	DirectX::XMFLOAT4X4 CalcRelativeTransform(const AssimpScene& assimp_scene, size_t node_index)
	{
		DirectX::XMFLOAT4X4 ret{};
		const auto start_node = assimp_scene.nodes_.at(node_index);
		DirectX::XMFLOAT4X4 parent_transform
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
		};

		if(start_node.parent_index_ > 0)
		{
			parent_transform = CalcRelativeTransform(assimp_scene, start_node.parent_index_);
		}

		DirectX::XMStoreFloat4x4(&ret, DirectX::XMLoadFloat4x4(&start_node.transform_) * DirectX::XMLoadFloat4x4(&parent_transform));

		return ret;
	}

	void ConvertMeshData(std::vector<MeshData>& mesh_data, const std::vector<RelayMeshData>& relay_mesh_data);

	void Traverse(const aiScene* ai_scene, const aiNode* ai_node, AssimpScene& assimp_scene, uint32_t parent_index)
	{
		const auto index = assimp_scene.nodes_.size();
		auto& node = assimp_scene.nodes_.emplace_back();
		node.index_ = static_cast<uint32_t>(index);
		node.name_ = ai_node->mName.C_Str();
		node.parent_index_ = parent_index;
		node.transform_ = ToFloat4x4(ai_node->mTransformation);
		node.offset_transform_ = node.transform_;
		node.mesh_counts_ = ai_node->mNumMeshes;

		for(int32_t mesh_index = 0; mesh_index < node.mesh_counts_; ++mesh_index)
		{
			node.mesh_indices_.emplace_back(ai_node->mMeshes[mesh_index]);
		}

		for(UINT child_index = 0; child_index < ai_node->mNumChildren; ++child_index)
		{
			Traverse(ai_scene, ai_node->mChildren[child_index], assimp_scene, node.index_);
		}
	};

	Model::Data ImportModel(const char* filename)
	{
		Model::Data ret{};

		const unsigned int import_flag =
			aiProcess_Triangulate
			| aiProcess_CalcTangentSpace
			| aiProcess_GenSmoothNormals
			| aiProcess_RemoveRedundantMaterials
			| aiProcess_GenUVCoords
			| aiProcess_OptimizeMeshes
		;

		Assimp::Importer importer{};
		const aiScene* ai_scene = importer.ReadFile(filename, import_flag);

		//読み込めたかチェック
		if(!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode)
		{
			std::cout << filename << " : " << "モデルのインポートに失敗しました" << std::endl;
			std::cout << importer.GetErrorString() << std::endl;
			_ASSERT_EXPR(FALSE, L"モデルのインポートに失敗しました");
		}

		AnimationClip animation_data;

		//ノードパース
		AssimpScene assimp_scene{};
		std::vector<RelayMeshData> relay_mesh_data;
		Traverse(ai_scene, ai_scene->mRootNode, assimp_scene, -1/*, relay_mesh_data*/);

		FetchMesh(ai_scene, assimp_scene, relay_mesh_data);
		
		//ノードからメッシュとマテリアルを取得
		FetchMaterial(ai_scene, ret.material_data_, filename);

		//メッシュ最適化
		//TODO ボーンを持っている場合、アクセスが狂う可能性があるので無効化
		//OptimizeMesh(relay_mesh_data);

		//メッシュレットの作成
		GenerateMeshlets(relay_mesh_data);

		//データの格納先を変更
		ret.mesh_data_.clear();
		ConvertMeshData(ret.mesh_data_, relay_mesh_data);

		//TODO アニメーション
	//	FetchAnimation(ai_scene, ret.animation_clips_);

		for(auto& m : ret.mesh_data_)
		{
			ret.num_polygons_ += m.num_polygon_;
		}

		return ret;
	}

	void FetchMesh(const aiScene* ai_scene, const AssimpScene& assimp_scene, std::vector<RelayMeshData>& relay_mesh_data)
	{
		for (UINT mesh_index = 0; mesh_index < ai_scene->mNumMeshes; ++mesh_index)
		{
			aiMesh* ai_mesh = ai_scene->mMeshes[mesh_index];
			auto& mesh_data = relay_mesh_data.emplace_back();
			mesh_data.name_ = ai_mesh->mName.C_Str();

			mesh_data.vertex_bones_.resize(ai_mesh->mNumVertices);

			const UINT vertices_counts = ai_mesh->mNumVertices;

			//マテリアルのインデックス
			mesh_data.material_index_ = ai_mesh->mMaterialIndex < ai_scene->mNumMaterials ? ai_mesh->mMaterialIndex : 0;

			//頂点情報の格納
			for (UINT vertices_index = 0; vertices_index < vertices_counts; ++vertices_index)
			{

				auto& vertex = mesh_data.vertices_.emplace_back();

				//頂点座標
				vertex.position_.x = ai_mesh->mVertices[vertices_index].x;
				vertex.position_.y = ai_mesh->mVertices[vertices_index].y;
				vertex.position_.z = ai_mesh->mVertices[vertices_index].z;

				//法線
				vertex.normal_.x = ai_mesh->mNormals[vertices_index].x;
				vertex.normal_.y = ai_mesh->mNormals[vertices_index].y;
				vertex.normal_.z = ai_mesh->mNormals[vertices_index].z;

				//接線
				if (ai_mesh->mTangents)
				{
					vertex.tangent_.x = ai_mesh->mTangents[vertices_index].x;
					vertex.tangent_.y = ai_mesh->mTangents[vertices_index].y;
					vertex.tangent_.z = ai_mesh->mTangents[vertices_index].z;
				}

				//陪法線
				if (ai_mesh->mBitangents)
				{
					vertex.binormal_.x = ai_mesh->mBitangents[vertices_index].x;
					vertex.binormal_.y = ai_mesh->mBitangents[vertices_index].y;
					vertex.binormal_.z = ai_mesh->mBitangents[vertices_index].z;
				}

				//UV座標
				if (ai_mesh->mTextureCoords[0])
				{
					vertex.texcoord_.x = ai_mesh->mTextureCoords[0][vertices_index].x;
					vertex.texcoord_.y = ai_mesh->mTextureCoords[0][vertices_index].y;
				}
			}

			//TODO ボーンデータを格納すること
			//FetchSkeleton(ai_scene, ai_mesh, assimp_scene, mesh_data.skeleton_data_);
			//FetchBoneInfluence(ai_scene, ai_mesh, mesh_data.skeleton_data_, mesh_data.vertex_bones_);

			//インデックスの格納
			for (UINT i = 0; i < ai_mesh->mNumFaces; ++i)
			{
				const aiFace ai_face = ai_mesh->mFaces[i];
				for (UINT j = 0; j < ai_face.mNumIndices; ++j)
				{
					mesh_data.indices_.push_back(ai_face.mIndices[j]);
				}
			}

			for(size_t i = 0; i < assimp_scene.nodes_.size(); ++i)
			{
				for(const auto& m_index : assimp_scene.nodes_.at(i).mesh_indices_)
				{
					if(m_index == mesh_index)
					{
						mesh_data.transform_ = CalcRelativeTransform(assimp_scene, i);
					}
				}
			}
			mesh_data.num_polygons_ = ai_mesh->mNumFaces;
		}
	}

	//頂点のボーン影響度を格納
	void FetchBoneInfluence(const aiScene* ai_scene, const aiMesh* ai_mesh, const Skeleton::Data& skeleton_data,
		std::vector<Skeleton::VertexBone>& vertex_bones)
	{
		if (!ai_mesh->HasBones()) return;

		//TODO vertex offset
		const uint32_t vertex_offset = 0u;

		const uint32_t bone_counts = ai_mesh->mNumBones;
		for(uint32_t bone_index = 0; bone_index < bone_counts; ++bone_index)
		{
			const auto ai_bone = ai_mesh->mBones[bone_index];

			const uint32_t weight_counts = ai_bone->mNumWeights;
			for(uint32_t weight_index = 0; weight_index < weight_counts; ++weight_index)
			{
				const auto ai_weight = ai_bone->mWeights[weight_index];
				auto& vertex_bone = vertex_bones.at(ai_weight.mVertexId + vertex_offset);
				//TODO vertex bone index
				for(int i = 0; i < 4; ++i)
				{
					if(vertex_bone.weights_[i] < FLT_EPSILON)
					{
						vertex_bone.weights_[i] = ai_weight.mWeight;
						vertex_bone.bone_index_[i] = skeleton_data.IndexOf(ai_bone->mName.C_Str());
						break;
					}
				}
			}
		}
	}

	//ボーンのデータを格納する用
	void FetchSkeleton(const aiScene* ai_scene, const aiMesh* ai_mesh, const AssimpScene& assimp_scene, Skeleton::Data& bones)
	{
		if (!ai_mesh->HasBones()) return;

		const uint32_t bone_counts = ai_mesh->mNumBones;

		for(uint32_t bone_index = 0; bone_index < bone_counts; ++bone_index)
		{
			const auto ai_bone = ai_mesh->mBones[bone_index];

			Skeleton::Data::Bone bone;
			bone.name_ = ai_bone->mName.C_Str();
			bone.node_index_ = assimp_scene.IndexOf(bone.name_);
			bone.parent_node_index_ = assimp_scene.nodes_.at(bone.node_index_).parent_index_;
			bone.parent_name_ = bone.parent_node_index_ < 0 ? "No Parent" :  assimp_scene.nodes_.at(bone.parent_node_index_).name_;
			bones.Add(bone);
		}
	}

	void FetchMaterial(const aiScene* ai_scene, std::vector<PbrMaterial::Data>& material_data, const char* filepath)
	{
		const UINT material_counts = ai_scene->mNumMaterials;

		for(UINT material_index = 0; material_index < material_counts; ++material_index)
		{
			const aiMaterial* ai_material = ai_scene->mMaterials[material_index];
			auto& data = material_data.emplace_back();

			data.name_ = ai_material->GetName().C_Str();

			//ファイルネームを削除
			std::filesystem::path directory_path = filepath;
			directory_path.remove_filename();

			//テクスチャの取得

			//ベースカラー
			if(ai_material->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
			{
				aiString texture_filepath{};
				const aiReturn ai_return = ai_material->GetTexture(aiTextureType_BASE_COLOR, 0u, &texture_filepath);
				if(ai_return != aiReturn_SUCCESS)
				{
					std::cout << "モデル : " << filepath << "でテクスチャの取得に失敗しました" << std::endl;
					_ASSERT_EXPR(FALSE, L"テクスチャ取得失敗");
				}
				data.texture_data_[PbrMaterial::TextureUsage::BaseColor] = directory_path.string() + texture_filepath.C_Str();
			}
			else
			{
				data.texture_data_[PbrMaterial::TextureUsage::BaseColor] = "Dummy_Base";
			}

			//ノーマルマップ
			if(ai_material->GetTextureCount(aiTextureType_NORMALS) > 0)
			{
				aiString texture_filepath{};
				const aiReturn ai_return = ai_material->GetTexture(aiTextureType_NORMALS, 0u, &texture_filepath);
				if(ai_return != aiReturn_SUCCESS)
				{
					std::cout << "モデル : " << filepath << "でテクスチャの取得に失敗しました" << std::endl;
					_ASSERT_EXPR(FALSE, L"テクスチャ取得失敗");
				}
				data.texture_data_[PbrMaterial::TextureUsage::Normal] = directory_path.string() + texture_filepath.C_Str();
			}
			else
			{
				data.texture_data_[PbrMaterial::TextureUsage::Normal] = "Dummy_Normal";
			}

			//メタリック
			if(ai_material->GetTextureCount(aiTextureType_METALNESS) > 0)
			{
				aiString texture_filepath{};
				const aiReturn ai_return = ai_material->GetTexture(aiTextureType_METALNESS, 0u, &texture_filepath);
				if(ai_return != aiReturn_SUCCESS)
				{
					std::cout << "モデル : " << filepath << "でテクスチャの取得に失敗しました" << std::endl;
					_ASSERT_EXPR(FALSE, L"テクスチャ取得失敗");
				}
				data.texture_data_[PbrMaterial::TextureUsage::Metallic] = directory_path.string() + texture_filepath.C_Str();
			}

			//TODO ラフネスも取れるようにしてほしい
			if(ai_material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
			{
				aiString texture_filepath{};
				const aiReturn ai_return = ai_material->GetTexture(aiTextureType_METALNESS, 0u, &texture_filepath);
				if (ai_return != aiReturn_SUCCESS)
				{
					std::cout << "モデル : " << filepath << "でテクスチャの取得に失敗しました" << std::endl;
					_ASSERT_EXPR(FALSE, L"テクスチャ取得失敗");
				}
				data.texture_data_[PbrMaterial::TextureUsage::Roughness] = directory_path.string() + texture_filepath.C_Str();
			}
		}
	}

	void FetchAnimation(const aiScene* ai_scene, std::vector<AnimationClip>& animation_clips)
	{
		if (!ai_scene->HasAnimations()) return;

		const uint32_t animation_counts = ai_scene->mNumAnimations;
		animation_clips.resize(animation_counts);

		for(uint32_t animation_index = 0; animation_index < animation_counts; ++animation_index)
		{
			const auto ai_animation = ai_scene->mAnimations[animation_index];

			//チャンネル == ノード　ノードの中にキーフレーム数がある??
			const UINT node_counts = ai_animation->mNumChannels;
			const UINT key_counts = ai_animation->mChannels[0]->mNumPositionKeys;	//TODO わんちゃんここだと数が足りない説がある

			auto& animation_clip = animation_clips.at(animation_index);
			animation_clip.sequence_.resize(key_counts);
			for (UINT key_index = 0; key_index < key_counts; ++key_index)
			{
				auto& key = animation_clip.sequence_.at(key_index);
				key.nodes_.resize(node_counts);

				for (UINT node_index = 0; node_index < node_counts; ++node_index)
				{
					const auto ai_node_anim = ai_animation->mChannels[node_index];
					auto& node = key.nodes_.at(node_index);
					node.name_ = ai_node_anim->mNodeName.C_Str();

					const UINT position_index = ai_node_anim->mNumPositionKeys < key_index ? key_index : 0;
					const UINT scaling_index = ai_node_anim->mNumScalingKeys < key_index ? key_index : 0;
					const UINT rotation_index = ai_node_anim->mNumRotationKeys < key_index ? key_index : 0;

					key.nodes_.at(node_index).translation_ = ToFloat3(ai_node_anim->mPositionKeys[position_index].mValue);
					key.nodes_.at(node_index).scaling_ = ToFloat3(ai_node_anim->mScalingKeys[scaling_index].mValue);
					key.nodes_.at(node_index).rotation_ = ToFloat4(ai_node_anim->mRotationKeys[rotation_index].mValue);
				}
			}
		}
	}

	void OptimizeMesh(std::vector<RelayMeshData>& relay_mesh_data)
	{
		//最適化
		for (auto& m : relay_mesh_data)
		{
			std::vector<uint32_t> remap(m.indices_.size());

			// 重複データを削除するための再マッピング用インデックスを生成.
			const auto vertex_count = meshopt_generateVertexRemap(
				remap.data(),
				m.indices_.data(),
				m.indices_.size(),
				m.vertices_.data(),
				m.vertices_.size(),
				sizeof(Vertex));

			std::vector<Vertex> vertices(vertex_count);
			std::vector<uint32_t> indices(m.indices_.size());

			// 頂点インデックスを再マッピング.
			meshopt_remapIndexBuffer(
				indices.data(),
				m.indices_.data(),
				m.indices_.size(),
				remap.data());

			// 頂点データを再マッピング.
			meshopt_remapVertexBuffer(
				vertices.data(),
				m.vertices_.data(),
				m.vertices_.size(),
				sizeof(Vertex),
				remap.data());

			// 不要になったメモリを解放.
			remap.clear();
			remap.shrink_to_fit();

			// 最適化したサイズにメモリ量を減らす.
			m.vertices_.resize(vertices.size());
			m.indices_.resize(indices.size());

			// 頂点キャッシュ最適化.
			meshopt_optimizeVertexCache(
				m.indices_.data(),
				indices.data(),
				indices.size(),
				vertex_count);

			// 不要になったメモリを解放.
			indices.clear();
			indices.shrink_to_fit();

			// 頂点フェッチ最適化.
			meshopt_optimizeVertexFetch(
				m.vertices_.data(),
				m.indices_.data(),
				m.indices_.size(),
				vertices.data(),
				vertices.size(),
				sizeof(Vertex));

			// 不要になったメモリを解放.
			vertices.clear();
			vertices.shrink_to_fit();
		}
	}

	void GenerateMeshlets(std::vector<RelayMeshData>& mesh_data)
	{
		//メッシュレットの作成
		for(auto& m : mesh_data)
		{
			const size_t kMaxVertices = 64;
			const size_t kMaxPrimitives = 124;
			const size_t kMaxMeshlets = meshopt_buildMeshletsBound(m.indices_.size(),
				kMaxVertices,
				kMaxPrimitives);

			//最大数で予約
			std::vector<meshopt_Meshlet> meshopt_meshlets(kMaxMeshlets);
			std::vector<unsigned int> meshlet_vertices(kMaxMeshlets * kMaxVertices);
			std::vector<unsigned char> meshlet_triangles(kMaxMeshlets * kMaxPrimitives * 3);;

			const size_t meshlet_count = meshopt_buildMeshlets(meshopt_meshlets.data(),
				meshlet_vertices.data(), meshlet_triangles.data(), m.indices_.data(),
				m.indices_.size(), &m.vertices_.data()->position_.x, m.vertices_.size(), sizeof(Vertex), kMaxVertices, kMaxPrimitives, 0.0f);

			//使っているサイズに変更
			meshopt_meshlets.resize(meshlet_count);


			//プリミティブの方は予めリサイズ
			auto opt_rbegin = (*meshopt_meshlets.rbegin());

			m.vertex_index_.resize(opt_rbegin.vertex_count + opt_rbegin.vertex_offset);
			m.primitive_index_.resize(opt_rbegin.triangle_offset + opt_rbegin.triangle_count * 3);


			//メッシュレットの格納
			for(const auto& meshopt_meshlet : meshopt_meshlets)
			{
				auto vertex_offset = kMaxVertices * meshlet_count; 
				auto primitive_offset = kMaxPrimitives * meshlet_count;

				Meshlet& meshlet = m.meshlets_.emplace_back();

				meshlet.vertex_counts_ = meshopt_meshlet.vertex_count;
				meshlet.vertex_offsets_ = meshopt_meshlet.vertex_offset;


				//トライアングルのカウントとオフセットがマッチしない　数に対してのオフセットが多い(*3以上のオフセット)
				meshlet.primitive_counts_ = meshopt_meshlet.triangle_count;
				meshlet.primitive_offsets_ = meshopt_meshlet.triangle_offset;

				//頂点数
				//なんか数がめちゃくちゃ少ないfor分の
				int vertex_index_counts = meshopt_meshlet.vertex_offset + meshopt_meshlet.vertex_count;

				auto bounding = m.vertices_.at(meshlet_vertices.at(meshopt_meshlet.vertex_offset)).position_;
				meshlet.bounding_min_ = meshlet.bounding_max_ =
					DirectX::XMFLOAT4(bounding.x, bounding.y, bounding.z, 0.0f);


				for(int i = meshopt_meshlet.vertex_offset; i < vertex_index_counts; ++i)
				{
					/*m.vertex_index_.emplace_back(meshlet_vertices.at(i));*/
					m.vertex_index_.at(i) = meshlet_vertices.at(i);

					//バウンディングボックス
					auto bounding_position = m.vertices_.at(meshlet_vertices.at(i)).position_;

					if(bounding_position.x < meshlet.bounding_min_.x)
					{
						meshlet.bounding_min_.x = bounding_position.x;
					}
					else if(bounding_position.x > meshlet.bounding_max_.x)
					{
						meshlet.bounding_max_.x = bounding_position.x;
					}

					if(bounding_position.y < meshlet.bounding_min_.y)
					{
						meshlet.bounding_min_.y = bounding_position.y;
					}
					else if(bounding_position.y > meshlet.bounding_max_.y)
					{
						meshlet.bounding_max_.y = bounding_position.y;
					}

					if(bounding_position.z < meshlet.bounding_min_.z)
					{
						meshlet.bounding_min_.z = bounding_position.z;
					}
					else if(bounding_position.z > meshlet.bounding_max_.z)
					{
						meshlet.bounding_max_.z = bounding_position.z;
					}

				}

				int primitive_index_counts = meshopt_meshlet.triangle_offset + meshopt_meshlet.triangle_count * 3;
				int packed_primitive_data_index = meshopt_meshlet.triangle_offset;
				for(int i = meshopt_meshlet.triangle_offset; i < primitive_index_counts; i += 3)
				{
					PackedPrimitiveData& packed_primitive_data = m.primitive_index_.at(packed_primitive_data_index);
					//PackedPrimitiveData& packed_primitive_data = m.primitive_index_.emplace_back();
					packed_primitive_data.index0 = meshlet_triangles.at(i + 0);
					packed_primitive_data.index1 = meshlet_triangles.at(i + 1);
					packed_primitive_data.index2 = meshlet_triangles.at(i + 2);
					++packed_primitive_data_index;
				}

			}
		}
	}

	void ConvertMeshData(std::vector<MeshData>& mesh_data, const std::vector<RelayMeshData>& relay_mesh_data)
	{
		for (const auto& r_mesh : relay_mesh_data)
		{
			auto& m = mesh_data.emplace_back();

			const size_t vertex_count = r_mesh.vertices_.size();
			m.position_.resize(vertex_count);
			m.normal_.resize(vertex_count);
			m.tangent_.resize(vertex_count);
			m.binormal_.resize(vertex_count);
			m.texcoord_.resize(vertex_count);
			m.bone_vertices_.resize(vertex_count);
			for (size_t vertex_index = 0; vertex_index < r_mesh.vertices_.size(); ++vertex_index)
			{
				m.position_.at(vertex_index) = r_mesh.vertices_.at(vertex_index).position_;
				m.normal_.at(vertex_index) = r_mesh.vertices_.at(vertex_index).normal_;
				m.tangent_.at(vertex_index) = r_mesh.vertices_.at(vertex_index).tangent_;
				m.binormal_.at(vertex_index) = r_mesh.vertices_.at(vertex_index).binormal_;
				m.texcoord_.at(vertex_index) = r_mesh.vertices_.at(vertex_index).texcoord_;
				m.bone_vertices_.at(vertex_index).bone_index_[0] = r_mesh.vertices_.at(vertex_index).bone_index_[0];
				m.bone_vertices_.at(vertex_index).bone_index_[1] = r_mesh.vertices_.at(vertex_index).bone_index_[1];
				m.bone_vertices_.at(vertex_index).bone_index_[2] = r_mesh.vertices_.at(vertex_index).bone_index_[2];
				m.bone_vertices_.at(vertex_index).bone_index_[3] = r_mesh.vertices_.at(vertex_index).bone_index_[3];
				m.bone_vertices_.at(vertex_index).bone_weights_[0] = r_mesh.vertices_.at(vertex_index).bone_weights_[0];
				m.bone_vertices_.at(vertex_index).bone_weights_[1] = r_mesh.vertices_.at(vertex_index).bone_weights_[1];
				m.bone_vertices_.at(vertex_index).bone_weights_[2] = r_mesh.vertices_.at(vertex_index).bone_weights_[2];
				m.bone_vertices_.at(vertex_index).bone_weights_[3] = r_mesh.vertices_.at(vertex_index).bone_weights_[3];
			}

			m.indices_ = r_mesh.indices_;
			m.vertex_index_ = r_mesh.vertex_index_;
			m.meshlets_ = r_mesh.meshlets_;
			m.primitive_index_ = r_mesh.primitive_index_;
			m.material_index = r_mesh.material_index_;
			m.num_polygon_ = r_mesh.num_polygons_;
			m.transform_ = r_mesh.transform_;
		}
	}
}
