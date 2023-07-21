// author: Changwan Yu
#include "pch.h"
#include "AcclaimImporter.h"
#include "RawAnimation.h"
#include "Skeleton.h"
#include "../AcclaimMotionCapture/AcclaimSkeleton.h"

void pa::AcclaimImporter::createRawAnimation(Acclaim::Motion const& acclaimMotion, RawAnimation* rawAnimation)
{

}

void pa::AcclaimImporter::destroyRawAnimation(RawAnimation* rawAnimation)
{
	delete rawAnimation;
}

void pa::AcclaimImporter::createSkeleton(Acclaim::Skeleton const& acclaimSkeleton, Skeleton* skeleton)
{
	using namespace DirectX;

	skeleton->_parents		= acclaimSkeleton._parents;
	skeleton->_boneMatrices.resize(acclaimSkeleton._boneData.size());

	for (uint16_t boneID : acclaimSkeleton._hierarchy)
	{
		skeleton->_hierarchy.push_back(boneID);

		// find all children of this bone
		std::vector<uint16_t> childs;
		auto it = acclaimSkeleton._parents.begin();
		while ((it = std::find(it, acclaimSkeleton._parents.end(), boneID)) != acclaimSkeleton._parents.end())
		{
			const uint16_t child = static_cast<uint16_t>(std::distance(acclaimSkeleton._parents.begin(), it));
			childs.push_back(child);
			it++;
		}

		XMVECTOR Q = DirectX::XMQuaternionIdentity();
		if (childs.size() == 1)
		{
			Q = acclaimSkeleton.getBoneRotation(childs.front());
			continue;
		}
		const XMVECTOR V = 

		for (uint16_t childID : childs)
		{
			const uint16_t dummyID = static_cast<uint16_t>(skeleton->_parents.size());
			skeleton->_parents[childID] = dummyID;

			skeleton->_hierarchy.push_back(dummyID);
			skeleton->_parents.push_back(boneID);

			const XMVECTOR Q = acclaimSkeleton.getBoneRotation(childID);
			const XMMATRIX M = XMMatrixRotationQuaternion(Q);

			skeleton->_boneMatrices.emplace_back();
			XMStoreFloat4x4(&skeleton->_boneMatrices.back(), M);
		}
	}

	for (uint16_t boneID = 0; boneID < acclaimSkeleton._boneData.size(); boneID++)
	{
		Acclaim::Skeleton::Bone const& boneData = acclaimSkeleton._boneData[boneID];
		uint16_t parentID = acclaimSkeleton.getParentID(boneID);

		XMVECTOR Q = acclaimSkeleton.getBoneRotation(boneID);
		XMVECTOR translation = acclaimSkeleton.getBoneTranslation(boneID);

		//XMMATRIX Mtranslation = XMMatrixTranslationFromVector(translation);
		//XMMATRIX boneMatrix = rotation * Mtranslation;

		//skeleton->_boneMatrices.emplace_back();
		//XMStoreFloat4x4(&skeleton->_boneMatrices.back(), boneMatrix);
	}
}

void pa::AcclaimImporter::destroySkeleton(Skeleton* skeleton)
{
	delete skeleton;
}
