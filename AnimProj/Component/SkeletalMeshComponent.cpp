// author: Changwan Yu
#include "pch.h"
#include "SkeletalMeshComponent.h"
#include "SceneComponent.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
#include "../Animation/Skeleton.h"
namespace pa
{
	SkeletalMeshComponent::SkeletalMeshComponent(const Skeleton& skeleton)
		: _skeleton(&skeleton)
	{
		_boneGTs.resize(_skeleton->getBoneCount());
		_boneToBoneGTs.resize(_skeleton->getBoneCount());
	}

	void SkeletalMeshComponent::setSkeleton(const Skeleton& skeleton)
	{
		_skeleton = &skeleton;
		_boneGTs.resize(_skeleton->getBoneCount());
		_boneToBoneGTs.resize(_skeleton->getBoneCount());
	}

	void SkeletalMeshComponent::onUpdate(World& world, std::weak_ptr<Actor> owner, float deltaTime)
	{
		using namespace DirectX;

		for (const size_t boneID : _skeleton->getHierarchy())
		{
			const size_t	parentID = _skeleton->getParentBoneID(boneID);

			XMMATRIX parentWorldTransform = {};
			if (boneID == 0)
			{
				XMVECTOR V = XMLoadFloat3(&owner.lock()->getComponent<SceneComponent>(world).position);
				
				parentWorldTransform = XMMatrixTranslationFromVector(V);
			}
			else
			{
				parentWorldTransform = XMLoadFloat4x4(&_boneGTs[parentID]);
			}

			XMMATRIX boneMatrix = _skeleton->getBoneMatrix(boneID);
			XMStoreFloat4x4(&_boneGTs[boneID], boneMatrix * parentWorldTransform);
		}

		for (const size_t boneID : _skeleton->getHierarchy())
		{
			const size_t parentID = _skeleton->getParentBoneID(boneID);

			const XMMATRIX boneToBoneLT = _skeleton->getBoneToBoneLT(boneID);
			const XMMATRIX parentGT = (boneID != 0) ?
				XMLoadFloat4x4(&_boneGTs[parentID]) : XMMatrixIdentity();

			XMStoreFloat4x4(&_boneToBoneGTs[boneID],
				boneToBoneLT * parentGT);
		}
		world.boneMatrixPool.insert(world.boneMatrixPool.end(), _boneGTs.begin(), _boneGTs.end());
		world.boneToBoneMatrixPool.insert(world.boneToBoneMatrixPool.end(), _boneToBoneGTs.begin(), _boneToBoneGTs.end());
	}
}