#include "pch.h"
#include "RAnimation.h"
#include "../ASFAMC/ASF.h"
#include "../ASFAMC/AMC.h"
#include "../Rendering/Skeleton.h"

pa::RAnimation::RAnimation(const ASF* acclaimSkeleton, const AMC* acclaimMotion)
{
    bool result = initializeAnimation(acclaimSkeleton, acclaimMotion);
    if (true != result)
        DebugBreak();
}

bool pa::RAnimation::initializeAnimation(const ASF* acclaimSkeleton, const AMC* acclaimMotion)
{
	using namespace DirectX;

	std::vector<size_t> fitBones;
	fitBones.reserve(acclaimMotion->_dataOrder.size());

	for (const auto& name : acclaimMotion->_dataOrder)
	{
		auto findIterator = std::find(acclaimSkeleton->_boneNameList.begin(), acclaimSkeleton->_boneNameList.end(), name);
		size_t index = std::distance(acclaimSkeleton->_boneNameList.begin(), findIterator);
		fitBones.push_back(index);
	}

	
	size_t dataIndex = 0;
	for (int frameIndex = 0; frameIndex < acclaimMotion->_frameCount; frameIndex++)
	{
		for (const int boneIndex : fitBones)
		{
			// rx, ry, rz, tx, ty, tz, l
			float dataBuffer[7] = {};
			for (int j = 0; j < 7; j++)
			{
				const ASF::Channel& channel = acclaimSkeleton->_DOFs[boneIndex].channels[j];
				if (ASF::Channel::LN == channel)
					DebugBreak();

				if (ASF::Channel::None == channel)
					break;
				
				dataBuffer[static_cast<size_t>(channel)] = acclaimMotion->getData(dataIndex++);
			}

			dataBuffer[0] *= acclaimSkeleton->_unitAngle * -1;		// rx
			dataBuffer[1] *= acclaimSkeleton->_unitAngle * -1;		// ry
			dataBuffer[2] *= acclaimSkeleton->_unitAngle;			// rz

			dataBuffer[3] *= acclaimSkeleton->_unitLength;			// tx
			dataBuffer[4] *= acclaimSkeleton->_unitLength;			// ty
			dataBuffer[5] *= acclaimSkeleton->_unitLength * -1;		// tz

			dataBuffer[6] *= acclaimSkeleton->_unitLength;			// l (length)

			XMMATRIX rotation = ASF::eulerRotation(dataBuffer, acclaimSkeleton->_axisOrders[boneIndex]);
			XMVECTOR quaternion = XMQuaternionNormalize(XMQuaternionRotationMatrix(rotation));

			// move pre-rotation data of current bone to parent bone
			size_t parentBoneIndex = acclaimSkeleton->_pSkeleton->getParentBoneIndex(boneIndex);
			if (boneIndex == 0)
			{
				// exception about root has no parent
				parentBoneIndex = 0;
			}

			RAnimation::Frame frame = {};
			frame.joint	= parentBoneIndex;
			frame.key	= frameIndex;
			XMStoreFloat4(&frame.v, quaternion);
			_rotations.push_back(frame);

			pAnimation->getPosition(frameIndex, parentBoneIndex) = XMFLOAT4(dataBuffer[3], dataBuffer[4], dataBuffer[5], 1.0f);
		}
	}

    return true;
}
