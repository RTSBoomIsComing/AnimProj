// author: Changwan Yu
#pragma once
namespace pa
{
	class Skeleton;
	class SkeletonRenderer
	{
	public:
		SkeletonRenderer(const Skeleton& skeleton);
		~SkeletonRenderer() = default;

		void getBoneToBoneGTs(const DirectX::XMFLOAT4X4* boneGTs, DirectX::XMFLOAT4X4* boneToBoneGTs);

	private:
		const Skeleton&						_skeleton;
		std::vector<DirectX::XMFLOAT4X4>	_boneToBoneLTs;
	};
}

