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

	skeleton->_hierarchy	= acclaimSkeleton._hierarchy;
	skeleton->_parents		= acclaimSkeleton._parents;

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
