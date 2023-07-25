// author: Changwan Yu
#include "pch.h"
#include "AcclaimImporter.h"
#include "Skeleton.h"
#include "../AcclaimMotionCapture/AcclaimSkeleton.h"
#include "../AcclaimMotionCapture/AcclaimMotion.h"

void pa::AcclaimImporter::createRawAnimation(Acclaim::Motion const& acclaimMotion, const Skeleton& skeleton, RawAnimation* rawAnimation)
{
	using namespace DirectX;

	std::vector<uint16_t> times(acclaimMotion._frameCount);
	for (uint16_t frame = 0; frame < acclaimMotion._frameCount; frame++)
	{
		times[frame] = frame;
	}

	for (int i = 0; i < acclaimMotion._boneIDs.size(); i++)
	{
		uint16_t boneID = acclaimMotion._boneIDs[i];
		Acclaim::Skeleton::Bone const& bone = acclaimMotion._skeleton->_boneData[boneID];

		bool hasTranslation = false;
		bool hasRotation = false;
		for (Acclaim::DOF dof : bone.dof)
		{
			if (static_cast<uint16_t>(dof) < 3)
				hasTranslation = true;
			else if (static_cast<uint16_t>(dof) < 6)
				hasRotation = true;
			else
				DebugBreak();
		}

		std::vector<XMFLOAT4> translations;
		std::vector<XMFLOAT4> rotations;

		std::vector<float> const& motionData = acclaimMotion._data[i];
		size_t motionDataCursor = 0;
		for (uint16_t frame = 0; frame < acclaimMotion._frameCount; frame++)
		{
			// 0, 1, 2 -> translation, 3, 4, 5 -> rotation, 6 -> length
			float buffer[7] = {};

			for (Acclaim::DOF dof : bone.dof)
			{
				uint16_t index = static_cast<uint16_t>(dof);
				buffer[index] = motionData[motionDataCursor++];
			}

			if (hasTranslation)
			{
				XMVECTOR V = XMVectorSet(buffer[0], buffer[1], buffer[2], 0.0f);
				V = V * acclaimMotion._skeleton->_unit.length;

				// convert right-handed to left-handed
				V = V * XMVectorSet(1.0f, 1.0f, -1.0f, 0.0f);

				XMFLOAT4 translation = {};
				translations.push_back(translation);
			}

			if (hasRotation)
			{
				XMVECTOR Euler = XMVectorSet(buffer[3], buffer[4], buffer[5], 0.0f);
				Euler = Euler * acclaimMotion._skeleton->_unit.angle;

				// convert right-handed to left-handed
				Euler = Euler * XMVectorSet(-1.0f, -1.0f, 1.0f, 0.0f);

				const XMVECTOR Q = Acclaim::getQuaternionFromAxis(Euler, bone.axisOrder);

				XMFLOAT4 quaternion = {};
				XMStoreFloat4(&quaternion, Q);
				rotations.push_back(quaternion);
			}
		}

		if (hasTranslation)
		{
			AnimationTrack track = {};
			track.boneID = boneID;
			track.type = AnimationTrackType::Translation;
			track.values = std::move(translations);
			track.times = times;

			rawAnimation->_tracks.push_back(track);
		}

		if (hasRotation)
		{
			AnimationTrack track = {};
			track.boneID = skeleton.getParentBoneID(boneID);
			track.type = AnimationTrackType::Rotation;
			track.values = std::move(rotations);
			track.times = times;

			rawAnimation->_tracks.push_back(track);
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
