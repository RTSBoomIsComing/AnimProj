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

		std::vector<uint8_t> hierarchy;
		for (size_t currentBonIndex : pSkeleton->getDFSPath())
		{
			hierarchy.push_back(currentBonIndex);

			// find all children of this bone
			std::vector<size_t> childs;
			auto it = pSkeleton->_parentList.begin();
			while ((it = std::find(it, pSkeleton->_parentList.end(), static_cast<uint8_t>(currentBonIndex))) != pSkeleton->_parentList.end())
			{
				childs.push_back(std::distance(pSkeleton->_parentList.begin(), it));
				it++;
			}

			if (childs.size() == 1)
			{
				pSkeleton->_boneList[currentBonIndex].rotation = pSkeleton->getBone(childs.front()).rotation;
				continue;
			}

			for (size_t childBoneIndex : childs)
			{
				Skeleton::Bone dummyBone;
				dummyBone.rotation = pSkeleton->getBone(childBoneIndex).rotation;
				pSkeleton->_boneList[childBoneIndex].rotation = XMFLOAT4{ 0.f, 0.f, 0.f, 1.f };
				
				const size_t dummyBoneIndex = pSkeleton->getBoneCount();
				
				pSkeleton->_boneList.push_back(dummyBone);
				pSkeleton->_parentList.push_back(currentBonIndex);
				hierarchy.push_back(dummyBoneIndex);

				pSkeleton->_parentList[childBoneIndex] = dummyBoneIndex;
			}
		}

		pSkeleton->_DFSPath = hierarchy;

	}
}