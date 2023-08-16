// author: Changwan Yu
#include "pch.h"
#include "SkeletalMeshComponent.h"
#include "SceneComponent.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
#include "../Animation/Skeleton.h"
namespace pa
{
	SkeletalMeshComponent::Socket SkeletalMeshComponent::weaponSocket = {};

	SkeletalMeshComponent::SkeletalMeshComponent(const Skeleton& skeleton)
		: _skeleton(&skeleton)
	{
		assert(_skeleton);
		_boneGTs.resize(_skeleton->getBoneCount());
		_boneToBoneGTs.resize(_skeleton->getBoneCount());
	}

	void SkeletalMeshComponent::setSkeleton(const Skeleton& skeleton)
	{
		_skeleton = &skeleton;
		_boneGTs.resize(_skeleton->getBoneCount());
		_boneToBoneGTs.resize(_skeleton->getBoneCount());
		_pose.resize(_skeleton->getBoneCount());
	}

	void SkeletalMeshComponent::onUpdate(World& world, Actor& owner, float deltaTime)
	{
		using namespace DirectX;

		SceneComponent* sceneComp = owner.getComponent<SceneComponent>();
		assert(sceneComp);

		if (sceneComp->isCulled())
			return;


		for (const size_t boneID : _skeleton->getHierarchy())
		{
			const size_t parentID = _skeleton->getParentBoneID(boneID);

			XMMATRIX parentWorldTransform = {};
			if (boneID == 0)
			{
				parentWorldTransform = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&sceneComp->eulerAngle));
				const XMVECTOR V	 = XMLoadFloat3(&sceneComp->position);
				parentWorldTransform *= XMMatrixTranslationFromVector(V);
			}
			else
			{
				parentWorldTransform = XMLoadFloat4x4(&_boneGTs[parentID]);
			}

			const XMMATRIX boneMatrix = _skeleton->getBoneMatrix(boneID);

			const XMVECTOR S			   = XMLoadFloat4(&_pose[boneID].scale);
			const XMVECTOR R			   = XMLoadFloat4(&_pose[boneID].rotation);
			const XMVECTOR T			   = XMLoadFloat4(&_pose[boneID].translation);
			const XMMATRIX animationMatrix = XMMatrixAffineTransformation(S, XMVectorZero(), R, T);

			XMStoreFloat4x4(&_boneGTs[boneID], animationMatrix * boneMatrix * parentWorldTransform);
		}

		for (const size_t boneID : _skeleton->getHierarchy())
		{
			const size_t parentID = _skeleton->getParentBoneID(boneID);

			const XMMATRIX boneToBoneLT = _skeleton->getBoneToBoneLT(boneID);
			const XMMATRIX parentGT		= (boneID != 0) ? XMLoadFloat4x4(&_boneGTs[parentID]) : XMMatrixIdentity();

			XMStoreFloat4x4(&_boneToBoneGTs[boneID],
							boneToBoneLT * parentGT);
		}
		world.boneMatrixPool.insert(world.boneMatrixPool.end(), _boneGTs.begin(), _boneGTs.end());
		world.boneToBoneMatrixPool.insert(world.boneToBoneMatrixPool.end(), _boneToBoneGTs.begin(), _boneToBoneGTs.end());

		if (weaponSocket.boneID < _skeleton->getBoneCount())
		{
			XMFLOAT4X4 weaponGT;
			XMStoreFloat4x4(&weaponGT, weaponSocket.offset.getMatrix() * XMLoadFloat4x4(&_boneGTs[weaponSocket.boneID]));
			world.boneToBoneMatrixPool.push_back(weaponGT);
		}
	}
}