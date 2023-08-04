// author: Changwan Yu
#pragma once
namespace pa
{
	class Skeleton;
	class Mesh;
	class SkeletonComponent
	{
		friend class SkeletonRenderingSystem;
	public:
		~SkeletonComponent();
		SkeletonComponent(const Skeleton& skeleton, DirectX::XMFLOAT4X4* boneGTs, DirectX::XMFLOAT4X4* boneToBoneGTs);
		
	private:
		static std::vector<uint32_t>							_s_ownerIDs;
		static std::vector<DirectX::XMFLOAT4X4>					_s_boneMatrixPool;
		static std::vector<DirectX::XMFLOAT4X4>					_s_boneToBoneMatrixPool;
		static std::vector<std::unique_ptr<SkeletonComponent>>	_s_components;

	public:
		static void create(SkeletonComponent** skeletonComp, const Skeleton& skeleton);
		static void destroy(SkeletonComponent** skeletonComp);
	
	public:
		void update(const Transform* pose, DirectX::XMVECTOR const& worldPosition, DirectX::XMVECTOR const& QWorldRotation);

	private:
		const Skeleton*						_skeleton				= nullptr;

		DirectX::XMFLOAT4X4*				_boneGTs;
		DirectX::XMFLOAT4X4*				_boneToBoneGTs;
	};
}

