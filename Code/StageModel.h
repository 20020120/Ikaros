#pragma once
#include"FbxResource.h"
class Stage_ModelResourse
{
public:

	static const int MAX_BONE_INFLUENCES{ 4 };
	static const int MAX_BONES{ 256 };

	struct Vertex
	{
		DirectX::XMFLOAT3 Position{};
		DirectX::XMFLOAT3 Nomal{};
		DirectX::XMFLOAT4 Tangent{};
		DirectX::XMFLOAT2 Texcoord{};
		float BoneWeights[MAX_BONE_INFLUENCES]{ 1, 0, 0, 0 };
		uint32_t BoneIndeices[MAX_BONE_INFLUENCES]{};

		template<class T>
		void serialize(T& archive)
		{
			archive(Position, Nomal, Tangent, Texcoord, BoneWeights, BoneIndeices);
		}


	};

	struct Skelton
	{
		struct Bone
		{
			uint64_t UniqueId{ 0 };
			std::string Name;
			// 'parent_index' is index that refers to the parent bone's position in the array that contains itself.
			int64_t ParentIndex{ -1 }; // -1 : the bone is orphan
			// 'node_index' is an index that refers to the node array of the scene.
			int64_t NodeIndex{ 0 };

			// 'offset_transform' is used to convert from model(mesh) space to bone(node) scene.
			DirectX::XMFLOAT4X4 offset_transform{ 
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1 };

			bool is_orphan() const { return ParentIndex < 0; };

			template<class T>
			void serialize(T& archive)
			{
				archive(UniqueId, Name, ParentIndex, NodeIndex, offset_transform);
			}

		};
		std::vector<Bone> BoneVec;

		int64_t indexof(uint64_t unique_id) const
		{
			int64_t index{ 0 };
			for (const Bone& bone : BoneVec)
			{
				if (bone.UniqueId == unique_id)
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
			archive(BoneVec);
		}


	};


	struct Constants
	{
		DirectX::XMFLOAT4X4 World{};
		DirectX::XMFLOAT4 MaterialColor{};
		DirectX::XMFLOAT4X4 BoneTransform[MAX_BONES]{ { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
	};

	struct Mesh
	{
		uint64_t UniqueId{ 0 };
		std::string Name;

		Skelton BindPose;
		//'node_index' is an index that refers to the node array of the scene.
		int64_t NodeIndex{ 0 };

		std::vector<Vertex>VertexVec;
		std::vector<uint32_t> IndicesVec;

		struct Subset
		{
			uint64_t MaterialUniqueId{ 0 };
			std::string MaterialName;
			uint32_t StartIndexLocation{ 0 };
			uint32_t IndexCount{ 0 };

			template<class T>
			void serialize(T& archive)
			{
				archive(MaterialUniqueId, MaterialName, StartIndexLocation, IndexCount);
			}

		};
		std::vector<Subset> SubsetVec;

		DirectX::XMFLOAT3 BoundingBox[2]
		{
			{ +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX },
            { -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX }
		};
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>IndexBuffer;
		friend class Stage_ModelResourse;

		DirectX::XMFLOAT4X4 DefaultGlobalTransform{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 
		};

	public:
		template<class T>
		void serialize(T& archive)
		{
			archive(UniqueId, Name, NodeIndex, SubsetVec, DefaultGlobalTransform,
				BindPose, BoundingBox, VertexVec, IndicesVec);
		}

	};

	std::vector<Mesh> MeshVec;

	struct Material
	{
		uint64_t UniqueId{ 0 };
		std::string Name;
		DirectX::XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };

		std::string TextureFileName[4];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView[2];

		friend class Stage_ModelResourse;

		template<class T>
		void serialize(T& archive)
		{
			archive(UniqueId, Name, Ka, Kd, Ks, TextureFileName);
		}

	};

	std::unordered_map<uint64_t, Material> MaterialVec{};

	

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
	Microsoft::WRL::ComPtr <ID3D11PixelShader> PixelShader;
	Microsoft::WRL::ComPtr <ID3D11InputLayout> InputLayout;
	Microsoft::WRL::ComPtr <ID3D11Buffer> ConstantBuffer;
	
public:
	Stage_ModelResourse(ID3D11Device* Device, const char* FbxFilename,float sampling_rate, bool Triangulate=false);
	virtual ~Stage_ModelResourse() = default;

	void f_FetchMeshVec(FbxScene* fbx_scene, std::vector<Mesh>& meshes);
	void f_CreateComObject(ID3D11Device* Device, const char* FbxFilename);
	void f_FetchMaterialVec(FbxScene* fbx_scene, std::unordered_map<uint64_t, Material>& materials);
	void f_FetchDummyMaterialVec(FbxScene* fbx_scene, std::unordered_map<uint64_t, Material>& materials);
	void f_FetchSkelton(FbxMesh* fbx_mesh, Skelton& bind_pose);
	
	
	void Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& World, const DirectX::XMFLOAT4& MaterialColor);
	
protected:
	SceneOfFbx SceneView;
};