// author: Changwan Yu
#pragma once
namespace pa
{
	class Skeleton;
	class Mesh;
	class SkeletonComponent
	{
	public:
		~SkeletonComponent();
		SkeletonComponent(ID3D11Device* device, const Skeleton& skeleton, DirectX::XMFLOAT4X4* boneGTs, DirectX::XMFLOAT4X4* boneToBoneGTs);
		
	public:
		static std::vector<ComPtr<ID3D11Buffer>>				s_boneWorldCBuffer;
		static std::vector<ComPtr<ID3D11Buffer>>				s_boneToBoneWorldCBuffer;

	private:
		static std::vector<uint32_t>							_s_ownerIDs;
		static std::vector<DirectX::XMFLOAT4X4>					_s_boneMatrixPool;
		static std::vector<DirectX::XMFLOAT4X4>					_s_boneToBoneMatrixPool;
		static std::vector<std::unique_ptr<SkeletonComponent>>	_s_components;

	public:
		static void create(ID3D11Device* device, SkeletonComponent** skeletonComp, const Skeleton& skeleton);
		static void destroy(SkeletonComponent** skeletonComp);
	
	public:
		void update(ID3D11DeviceContext* deviceContext, const Transform* pose, DirectX::XMVECTOR const& worldPosition, DirectX::XMVECTOR const& QWorldRotation);
		void render(ID3D11DeviceContext* deviceContext, const Mesh* boneMesh, const Mesh* boneToBoneMesh);

	private:
		const Skeleton*						_skeleton				= nullptr;

		DirectX::XMFLOAT4X4*				_boneGTs;
		DirectX::XMFLOAT4X4*				_boneToBoneGTs;
	};
}

