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

	if (nullptr == acclaimSkeleton)
		return false;

	if (nullptr == acclaimMotion)
		return false;

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
		for (size_t boneIndex : fitBones)
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

			// 0 : rx, 1 : ry, 2 : rz
			// 3 : tx, 4 : ty, 5 : tz, 6 : l (length)
			dataBuffer[0] *= acclaimSkeleton->_unitAngle * -1;		
			dataBuffer[1] *= acclaimSkeleton->_unitAngle * -1;		
			dataBuffer[2] *= acclaimSkeleton->_unitAngle;			

			dataBuffer[3] *= acclaimSkeleton->_unitLength;			
			dataBuffer[4] *= acclaimSkeleton->_unitLength;			
			dataBuffer[5] *= acclaimSkeleton->_unitLength * -1;		

			dataBuffer[6] *= acclaimSkeleton->_unitLength;			

			// process position data
			XMVECTOR position = XMVECTOR{ dataBuffer[3], dataBuffer[4], dataBuffer[5], 1.0f };
			if (dataBuffer[3] != 0.0f && dataBuffer[4] != 0.0f && dataBuffer[5] != 0.0f)
			{
				RAnimation::Frame frame = {};
				frame.joint	= boneIndex;
				frame.key = frameIndex;
				XMStoreFloat4(&frame.v, position);
				_positions.push_back(frame);
			}

			// process rotation data
			XMMATRIX rotation = ASF::eulerRotation(dataBuffer, acclaimSkeleton->_axisOrders[boneIndex]);
			XMVECTOR quaternion = XMQuaternionNormalize(XMQuaternionRotationMatrix(rotation));

			// move pre-rotation data of current bone to parent bone
			size_t preRotationPropagateBoneIndex = acclaimSkeleton->_pSkeleton->getParentBoneIndex(boneIndex);
			if (boneIndex == 0)
			{
				// exception about root has no parent
				preRotationPropagateBoneIndex = 0;
			}

			RAnimation::Frame frame = {};
			frame.joint	= preRotationPropagateBoneIndex;
			frame.key	= frameIndex;
			XMStoreFloat4(&frame.v, quaternion);
			_rotations.push_back(frame);
		}
	}

    return true;
}
