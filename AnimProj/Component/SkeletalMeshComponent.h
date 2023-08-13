// author: Changwan Yu
#pragma once
namespace pa
{
	class World;
	class Actor;
	class Skeleton;
	class SkeletalMeshComponent
	{
	public:
		SkeletalMeshComponent() = default;
		SkeletalMeshComponent(const Skeleton& skeleton);
	public:
		void setSkeleton(const Skeleton& skeleton);
		void onUpdate(World& world, Actor& owner, float deltaTime);

	public:
		const Skeleton*						_skeleton		= nullptr;
		std::vector<DirectX::XMFLOAT4X4>	_boneGTs;
		std::vector<DirectX::XMFLOAT4X4>	_boneToBoneGTs;
	};
}

