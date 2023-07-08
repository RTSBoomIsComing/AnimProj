#include "pch.h"
#include "Rendering/Skeleton.h"

namespace pa
{
	void SandboxModifySkeleton(Skeleton* pSkeleton)
	{
		using namespace DirectX;

		// Convert Global space to Local Space 
		std::vector<XMFLOAT4> convertedQuats(pSkeleton->getBoneCount(), XMFLOAT4{ 0.f, 0.f, 0.f, 1.f });
		for (size_t i = 1; i < pSkeleton->getBoneCount(); i++)
		{
			const Skeleton::Bone& bone = pSkeleton->getBone(i);
			const Skeleton::Bone& parentBone = pSkeleton->getBone(pSkeleton->getParentBoneIndex(i));
			
			XMVECTOR dir = XMVector3Transform(XMLoadFloat4(&bone.direction), XMMatrixRotationQuaternion(
				XMQuaternionInverse(XMLoadFloat4(&bone.rotation))));
			XMStoreFloat4(&pSkeleton->_boneList[i].direction, dir);

			XMVECTOR quat = XMQuaternionMultiply(XMLoadFloat4(&bone.rotation), 
				XMQuaternionInverse(XMLoadFloat4(&parentBone.rotation)));

			XMStoreFloat4(&convertedQuats[i], quat);
		}

		for (size_t i = 1; i < pSkeleton->getBoneCount(); i++)
		{
			pSkeleton->_boneList[i].rotation = convertedQuats[i];
		}
		//std::vector<XMFLOAT4X4> dummyBone;
		//for (size_t i : pSkeleton->getDFSPath())
		//{
		//	const Skeleton::Bone& bone = pSkeleton->getBone(i);
		//	XMMATRIX transform = 

		//}

	}
}