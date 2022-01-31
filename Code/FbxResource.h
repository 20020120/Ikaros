#pragma once
#include<d3d11.h>

#include<DirectXMath.h>
#include<vector>
#include<string>
#include<unordered_map>
#include<fbxsdk.h>

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>


namespace DirectX
{
	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
			cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
			cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
			cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
			cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}



struct SceneOfFbx
{

	struct Node
	{
		uint64_t UniqueId{ 0 };
		std::string Name;
		FbxNodeAttribute::EType Attribute{ FbxNodeAttribute::EType::eUnknown };
		int64_t ParentIndex{ -1 };

		template<class T>
		void serialize(T& archive)
		{
			archive(UniqueId, Name, Attribute, ParentIndex);
		}

	};

	std::vector<Node> NodeVec{};
	int64_t IndexOf(uint64_t UniqueId) const
	{
		int64_t index{ 0 };
		for (const auto it : NodeVec)
		{
			if (it.UniqueId == UniqueId)
			{
				return index;
			}
			++index;
		}
		return -1;
	}

	template<class T>
	void serialize(T& archive)
	{
		archive(NodeVec);
	}

};


//****************************************************************
// 
// BoneInfluence構造体
// 
//****************************************************************

struct BoneInfluence
{
	uint32_t BoneIndex;;
	float BoneWeight;
};

using BoneInfluence_Per_ControlPoint = std::vector<BoneInfluence>;


//****************************************************************
// 
// ボーンの各情報を取得する関数
// 
//****************************************************************
inline void f_FetchBoneInfluences(const FbxMesh* fbx_mesh,
	std::vector<BoneInfluence_Per_ControlPoint>& bone_influences)
{
	const int control_points_count{ fbx_mesh->GetControlPointsCount() };
	bone_influences.resize(control_points_count);

	const int skin_count{ fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) };
	for (int skin_index = 0; skin_index < skin_count; ++skin_index)
	{
		const FbxSkin* fbx_skin
		{ static_cast<FbxSkin*>(fbx_mesh->GetDeformer(skin_index, FbxDeformer::eSkin)) };

		const int cluster_count{ fbx_skin->GetClusterCount() };
		for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
		{
			const FbxCluster* fbx_cluster{ fbx_skin->GetCluster(cluster_index) };

			const int control_point_indices_count{ fbx_cluster->GetControlPointIndicesCount() };
			for (int control_point_indices_index = 0; control_point_indices_index < control_point_indices_count;
				++control_point_indices_index)
			{
				int control_point_index{ fbx_cluster->GetControlPointIndices()[control_point_indices_index] };
				double control_point_weight
				{ fbx_cluster->GetControlPointWeights()[control_point_indices_index] };
				BoneInfluence& bone_influence{ bone_influences.at(control_point_index).emplace_back() };
				bone_influence.BoneIndex = static_cast<uint32_t>(cluster_index);
				bone_influence.BoneWeight = static_cast<float>(control_point_weight);
			}
		}
	}
}


inline DirectX::XMFLOAT4X4 f_ToXMFLOAT4x4(const FbxAMatrix& fbxamatrix)
{
	using namespace DirectX;
	XMFLOAT4X4 xmfloat4x4;
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
	return xmfloat4x4;

}

inline DirectX::XMFLOAT3 f_ToXMFLOAT3(const FbxDouble3& fbxdouble3)
{
	using namespace DirectX;

	XMFLOAT3 xmfloat3;
	xmfloat3.x = static_cast<float>(fbxdouble3[0]);
	xmfloat3.y = static_cast<float>(fbxdouble3[1]);
	xmfloat3.z = static_cast<float>(fbxdouble3[2]);
	return xmfloat3;

}

inline DirectX::XMFLOAT4 f_ToXMFLOAT4(const FbxDouble4& fbxdouble4)
{
	using namespace DirectX;

	XMFLOAT4 xmfloat4;
	xmfloat4.x = static_cast<float>(fbxdouble4[0]);
	xmfloat4.y = static_cast<float>(fbxdouble4[1]);
	xmfloat4.z = static_cast<float>(fbxdouble4[2]);
	xmfloat4.w = static_cast<float>(fbxdouble4[3]);
	return xmfloat4;

}


