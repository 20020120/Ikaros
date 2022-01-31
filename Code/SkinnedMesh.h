#pragma once
#include <wrl/client.h>
#include"FbxResource.h"
struct Animation
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

class SkinnedMesh
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

		std::string TextureFileName[4];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView[2];

		friend class SkinnedMesh;

		template<class T>
		void serialize(T& archive)
		{
			archive(UniqueId, Name, Ka, Kd, Ks, TextureFileName);
		}

	};

	std::unordered_map<uint64_t, Material> MaterialVec{};


	//****************************************************************
	// 
	// �A�j���[�V�����Ƀo�C���h�����t���O�ϐ�
	// 
	//****************************************************************
	struct EventFlag
	{
		std::string Name;
		int Index;  //�C�x���g�̔ԍ�
		std::string AnimName;  //�ǂ̃A�j���[�V�����Ƀo�C���h����Ă��邩
		float BeginFrame;//�A�j���[�V�����̉��t���[���ڂ���t���O���I���ɂȂ邩
		float LengthOfFrame; //�t���O���I���ɂȂ鎞��

		template<class T>
		void serialize(T& archive)
		{
			archive(Name,Index, AnimName, BeginFrame, LengthOfFrame);
		}
	};
	std::map<int, EventFlag> EventFlagMap;

	//****************************************************************
	// 
	// �A�j���[�V�����Ƀo�C���h�����T�E���h�t���O
	// 
	//****************************************************************
	struct SoundFlag
	{
		std::string Name;//���O
		std::string AnimeName; //�o�C���h����Ă���A�j���[�V�����̖��O
		std::string SoundPath;//�炷�T�E���h�܂ł̃p�X
		float BeginFrame;//���t���[������T�E���h���邩
		int key;//�T�E���h��map��key(SoundFlag�̏I�[��key�̒l�{�P)

		template<class T>
		void serialize(T& archive)
		{
			archive(Name, AnimeName, SoundPath, BeginFrame, key);
		}
	};
	std::vector<SoundFlag> SoundFlagVec;



	//****************************************************************
	// 
	// �A�j���[�V����
	// 
	//****************************************************************
	
	std::vector<Animation> AnimationVec;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
	Microsoft::WRL::ComPtr <ID3D11PixelShader> PixelShader;
	Microsoft::WRL::ComPtr <ID3D11InputLayout> InputLayout;
	Microsoft::WRL::ComPtr <ID3D11Buffer> ConstantBuffer;
	
public:
	SkinnedMesh(ID3D11Device* Device, const char* FbxFilename,float sampling_rate, bool Triangulate=false);
	virtual ~SkinnedMesh() = default;

	void f_FetchMeshVec(FbxScene* fbx_scene, std::vector<Mesh>& meshes);
	void f_CreateComObject(ID3D11Device* Device, const char* FbxFilename);
	void f_FetchMaterialVec(FbxScene* fbx_scene, std::unordered_map<uint64_t, Material>& materials);
	void f_FetchDummyMaterialVec(FbxScene* fbx_scene, std::unordered_map<uint64_t, Material>& materials);
	void f_FetchSkelton(FbxMesh* fbx_mesh, Skelton& bind_pose);
	void f_FetchAnimation(FbxScene* fbx_scene, std::vector<Animation>& animation_vec, float sampling_rate/*If this value is 0, the animation data will be sampled at the default frame rate.*/);
	void f_FetchDummyAnimation(FbxScene* fbx_scene, std::vector<Animation>& animation_vec, float sampling_rate/*If this value is 0, the animation data will be sampled at the default frame rate.*/);

	void f_FetchDummyEventFlag();
	void f_FetchDummySoundFlag();

	void f_UpdateAnimation(Animation::KeyFrame& keyframe);

	bool f_AppendAnimation(const char* animation_filename, float sampling_rate);

	void f_BlendAnimation(const Animation::KeyFrame* keyframes[2], float factor,
		Animation::KeyFrame& keyframe);
	void Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& World, const DirectX::XMFLOAT4& MaterialColor,const Animation::KeyFrame* keyframe);
	

	std::string GetFilePath() { return FilePath; }
	void f_SaveSkinnedMesh(std::string saveDir);

	void f_Serealize(std::string saveDir);
	void f_DeSerealize();
protected:
	SceneOfFbx SceneView;
	std::string FilePath{};
};