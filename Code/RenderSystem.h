#pragma once
#include"FbxResource.h"
#include<wrl.h>
//****************************************************************
// 
// モデルデータのリソース
// 
//****************************************************************
struct ModelRenderResource
{
    static const int MAXBONEVALUE{ 256 };
    static const int MAX_BONE_INFLUENCES{ 4 };

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
        DirectX::XMFLOAT4X4 BoneTransform[MAXBONEVALUE]{ { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
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
	
		Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>IndexBuffer;
		friend class SkinnedMesh;

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

		std::string TextureFileName[8];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView[8];

		friend class SkinnedMesh;

		template<class T>
		void serialize(T& archive)
		{
			archive(UniqueId, Name, Ka, Kd, Ks, TextureFileName);
		}

	};

	std::unordered_map<uint64_t, Material> MaterialVec{};

	SceneOfFbx SceneView;

	template<class T>
	void serialize(T& archive)
	{
		archive(SceneView,MeshVec,MaterialVec);
	}
};


//****************************************************************
// 
// アニメーションデータのリソース
// 
//****************************************************************
struct ModelAnimationResource
{
	std::string Name;
	float SamplingRate{ 0 };

	struct KeyFrame
	{
		struct Node
		{
			// 'global_transform' is used to convert from local space of node to global space of scene.
			DirectX::XMFLOAT4X4 GlobalTransform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

			DirectX::XMFLOAT3 Scaling{ 1,1,1 };
			DirectX::XMFLOAT4 Rotation{ 0,0,0,1 };
			DirectX::XMFLOAT3 Translation{ 0,0,0 };

			template<class T>
			void serialize(T& archive)
			{
				archive(GlobalTransform, Scaling, Rotation, Translation);
			}

		};
		std::vector<Node> NodeVec;

		template<class T>
		void serialize(T& archive)
		{
			archive(NodeVec);
		}

	};
	std::vector<KeyFrame> Sequence;

	template<class T>
	void serialize(T& archive)
	{
		archive(Name, SamplingRate, Sequence);
	}

};

//****************************************************************
// 
// モデル初期化クラス
// 
//****************************************************************
class ModelRenderInitializer
{
public:
	enum Type
	{
		MODEL,
		VERTEX,
		PIXEL,
		GEOMETRY
	};

	ModelRenderInitializer() {}
	~ModelRenderInitializer() {}
	
	//--------------------<セッター>--------------------//
	void SetALLPath(
		std::string ModelPath,
		std::string Vs,
		std::string Ps,
		std::string Gs
		);		
	void SetModelPath(std::string Path);
	void SetVsPath(std::string Path);
	void SetPsPath(std::string Path);
	void SetGsPath(std::string Path);

	//--------------------<ゲッター>--------------------//
	const std::string GetPath(unsigned short t);


private:
	std::string ModelPath{ "NoSetting" };   // モデルのパス
	std::string VS_ShaderPath{"NoSetting"}; // VertexShaderのパス
	std::string PS_ShaderPath{"NoSetting"}; // PixelShaderのパス
	std::string GS_ShaderPath{"NoSetting"}; // GeometryShaderのパス
};


class ModelRenderSystem
{

public:


	enum TextureType
	{
		BaseColor,  // ベースカラー 
		Emissive,   // エミッシブ
		Metallic,   // メタリック
		Roughness,  // ラフネス
		Normal,     // 法線
		Value, // 量
	};

	ModelRenderSystem(ID3D11Device* device,ModelRenderInitializer Ini);
	~ModelRenderSystem();

    //--------------------<描画>--------------------//
    void Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& World,
		const DirectX::XMFLOAT4& MaterialColor, const ModelAnimationResource::KeyFrame* keyframe);

	//--------------------<ゲッター>--------------------//
	auto& GetAnimeResources() { return AnimeResources; }
	auto& GetRenderResource() { return Resource; }

	void SetMateriaColor(DirectX::XMFLOAT4 Color){}
protected:
    //--------------------<.nkファイルをロード>--------------------//
    void LoadModel(std::string FilePath);

	//--------------------<Comオブジェクトを生成>--------------------//
	void CreateComObject(ID3D11Device* device, ModelRenderInitializer Ini);

	ModelRenderResource Resource;
	std::vector<ModelAnimationResource> AnimeResources;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
	Microsoft::WRL::ComPtr <ID3D11PixelShader> PixelShader;
	Microsoft::WRL::ComPtr <ID3D11GeometryShader> GeometryShader;


	Microsoft::WRL::ComPtr <ID3D11InputLayout> InputLayout;
	Microsoft::WRL::ComPtr <ID3D11Buffer> ConstantBuffer;
};



