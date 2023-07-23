// author: Changwan Yu
#include "pch.h"
#include "AcclaimImporter.h"
#include "RawAnimation.h"
#include "Skeleton.h"
#include "../AcclaimMotionCapture/AcclaimSkeleton.h"
#include "../AcclaimMotionCapture/AcclaimMotion.h"

void pa::AcclaimImporter::createRawAnimation(Acclaim::Motion const& acclaimMotion, RawAnimation* rawAnimation)
{
	using namespace DirectX;
	for (int i = 0; i < acclaimMotion._boneIDs.size(); i++)
	{
		uint16_t boneID = acclaimMotion._boneIDs[i];
		std::vector<Acclaim::DOF> const& dofs = acclaimMotion._skeleton->_boneData[boneID].dof;

		std::vector<float> const& motionData = acclaimMotion._data[i];
		size_t motionDataCursor = 0;
		for (uint16_t frame = 0; frame < acclaimMotion._frameCount; frame++)
		{
			float buffer[7] = {};
			bool hasTranslation = false;
			bool hasRotation = false;
			for (Acclaim::DOF dof : dofs)
			{
				const float value = motionData[motionDataCursor++];
				uint16_t index = static_cast<uint16_t>(dof);
				buffer[index] = value;
				if (index < 3)
					hasTranslation = true;
				else if (index < 6)
					hasRotation = true;
				else
					DebugBreak();
			}

			if (hasTranslation)
			{
				//TODO
			}
			if (hasRotation)
			{
				//TODO
			}
		}
	}
}

void pa::AcclaimImporter::destroyRawAnimation(RawAnimation* rawAnimation)
{
	if (nullptr != rawAnimation)
		delete rawAnimation;
}

void pa::AcclaimImporter::createSkeleton(Acclaim::Skeleton const& acclaimSkeleton, Skeleton* skeleton)
{
	using namespace DirectX;

	skeleton->_parents = acclaimSkeleton._parents;
	skeleton->_boneMatrices.resize(acclaimSkeleton._boneData.size());

	for (uint16_t boneID : acclaimSkeleton._hierarchy)
	{
		skeleton->_hierarchy.push_back(boneID);

		// find all children of this bone
		std::vector<uint16_t> childs = acclaimSkeleton.getChildrenID(boneID);

		XMVECTOR Q = DirectX::XMQuaternionIdentity();
		if (childs.size() == 1)
			Q = acclaimSkeleton.getBoneRotation(childs.front());

		const XMMATRIX Mrotation = XMMatrixRotationQuaternion(Q);

		const XMVECTOR V = acclaimSkeleton.getBoneTranslation(boneID);
		const XMMATRIX Mtranslation = XMMatrixTranslationFromVector(V);

		XMStoreFloat4x4(&skeleton->_boneMatrices[boneID], Mrotation * Mtranslation);

		if (childs.size() <= 1)
			continue;

		for (uint16_t childID : childs)
		{
			const uint16_t dummyID = static_cast<uint16_t>(skeleton->_boneMatrices.size());
			skeleton->_parents[childID] = dummyID;

			skeleton->_hierarchy.push_back(dummyID);
			skeleton->_parents.push_back(boneID);

			const XMVECTOR Q = acclaimSkeleton.getBoneRotation(childID);
			const XMMATRIX M = XMMatrixRotationQuaternion(Q);

			skeleton->_boneMatrices.emplace_back();
			XMStoreFloat4x4(&skeleton->_boneMatrices.back(), M);

			assert(skeleton->_boneMatrices.size() == skeleton->_parents.size());
		}
	}

	assert(skeleton->_boneMatrices.size() == skeleton->_hierarchy.size());
}

void pa::AcclaimImporter::destroySkeleton(Skeleton* skeleton)
{
	if (nullptr != skeleton)
		delete skeleton;
}
