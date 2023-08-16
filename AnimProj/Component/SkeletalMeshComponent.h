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
		void onUpdate(World& world, Actor& owner, float deltaTime);

	public:
		void setSkeleton(const Skeleton& skeleton);

	public:
		const Skeleton*						_skeleton		= nullptr;
		std::vector<DirectX::XMFLOAT4X4>	_boneGTs;
		std::vector<DirectX::XMFLOAT4X4>	_boneToBoneGTs;

		std::vector<Transform>	_pose;

		struct Socket
		{
			uint16_t  boneID = std::numeric_limits<uint16_t>::max();
			Transform offset = {};
		};

		static Socket weaponSocket;
	};
}

