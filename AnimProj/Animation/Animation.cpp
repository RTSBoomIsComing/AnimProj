#include "pch.h"
#include "Animation.h"
#include "../ASFAMC/ASF.h"
#include "../ASFAMC/AMC.h"
#include "../Rendering/Skeleton.h"

pa::Animation::Animation(const ASF* acclaimSkeleton, const AMC* acclaimMotion)
{
	bool result = initializeAnimation(acclaimSkeleton, acclaimMotion);
	if (true != result)
		DebugBreak();
}

bool pa::Animation::initializeAnimation(const ASF* acclaimSkeleton, const AMC* acclaimMotion)
{
	using namespace DirectX;

	if (nullptr == acclaimSkeleton)
		return false;

	if (nullptr == acclaimMotion)
		return false;

	_duration = acclaimMotion->getFrameCount();
	_boneAnimation.resize(acclaimSkeleton->_pSkeleton->getBoneCount());


	std::vector<size_t> fitBones;
	fitBones.reserve(acclaimMotion->_dataOrder.size());

	for (const auto& name : acclaimMotion->_dataOrder)
	{
		auto findIterator = std::find(acclaimSkeleton->_boneNameList.begin(), acclaimSkeleton->_boneNameList.end(), name);
		size_t index = std::distance(acclaimSkeleton->_boneNameList.begin(), findIterator);
		fitBones.push_back(index);
	}


	size_t dataIndex = 0;
	for (int frameIndex = 0; frameIndex < acclaimMotion->getFrameCount(); frameIndex++)
	{
		for (size_t boneIndex : fitBones)
		{
			bool hasTranslation = false;
			bool hasRotation = false;

			// rx, ry, rz, tx, ty, tz, l
			float dataBuffer[7] = {};
			for (int j = 0; j < 7; j++)
			{
				const ASF::Channel& channel = acclaimSkeleton->_DOFs[boneIndex].channels[j];
				if (ASF::Channel::LN == channel)
					DebugBreak();

				if (ASF::Channel::None == channel)
					break;

				if (ASF::Channel::TX == channel || ASF::Channel::TY == channel || ASF::Channel::TZ == channel)
					hasTranslation = true;

				if (ASF::Channel::RX == channel || ASF::Channel::RY == channel || ASF::Channel::RZ == channel)
					hasRotation = true;


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

			if (hasTranslation)
			{
				XMVECTOR position = XMVECTOR{ dataBuffer[3], dataBuffer[4], dataBuffer[5], 1.0f };
				Animation::Frame frame = {};
				frame.key = frameIndex;
				XMStoreFloat4(&frame.v, position);
				_boneAnimation[boneIndex].position.push_back(frame);
			}

			if (hasRotation)
			{
				XMMATRIX rotation = ASF::eulerRotation(dataBuffer, acclaimSkeleton->_axisOrders[boneIndex]);
				XMVECTOR quaternion = XMQuaternionNormalize(XMQuaternionRotationMatrix(rotation));

				// move pre-rotation data of current bone to parent bone
				size_t preRotationPropagateBoneIndex = acclaimSkeleton->_pSkeleton->getParentBoneIndex(boneIndex);
				
				// exception about root that has no parent
				if (boneIndex == 0)
					preRotationPropagateBoneIndex = 0;
				
				Animation::Frame frame = {};
				frame.key = frameIndex;
				XMStoreFloat4(&frame.v, quaternion);
				_boneAnimation[preRotationPropagateBoneIndex].rotation.push_back(frame);
			}
		}
	}

	return true;
}

void pa::Animation::compressAnimation()
{
	for (auto& boneAnimation : _boneAnimation)
	{
		fitBoneAnimationRotation(boneAnimation.rotation);
	}
}

void pa::Animation::fitBoneAnimationRotation(std::vector<Animation::Frame>& rotations, float threshold)
{
	using namespace DirectX;

	if (rotations.empty())
		return;

	std::vector<bool>		picked(rotations.size(), false);
	picked.front()			= true;
	picked.back()			= true;

	while(true)
	{
		std::vector<int>	sectionMiddles;
		std::vector<float>	errorSums;

		int p0 = 0;

		auto iterator = std::find(picked.begin(), picked.end(), true);
		int p1 = static_cast<int>(std::distance(picked.begin(), iterator));

		iterator = std::find(++iterator, picked.end(), true);
		int p2 = static_cast<int>(std::distance(picked.begin(), iterator));

		if (iterator < picked.end())
			iterator = std::find(++iterator, picked.end(), true);

		int p3 = (iterator != picked.end()) ? static_cast<int>(std::distance(picked.begin(), iterator)) : p2;

		while (p1 != p2)
		{
			sectionMiddles.push_back((p1 + p2) / 2);
			errorSums.push_back(0.0f);

			for (int between = p1 + 1; between < p2; between++)
			{
				const float t = static_cast<float>(between - p1) / (p2 - p1);
				XMVECTOR catmullRom = XMVectorCatmullRom(
					XMLoadFloat4(&rotations[p0].v),
					XMLoadFloat4(&rotations[p1].v),
					XMLoadFloat4(&rotations[p2].v),
					XMLoadFloat4(&rotations[p3].v), t);

				catmullRom = XMQuaternionNormalize(catmullRom);

				const XMVECTOR	difference = XMLoadFloat4(&rotations[between].v) - catmullRom;
				const float error = XMVectorGetX(XMVector4LengthSq(difference));
				//errorSums.back() += error;

				if (errorSums.back() < error)
				{
					errorSums.back() = error;
					sectionMiddles.back() = between;
				}
			}

			p0 = p1;
			p1 = p2;
			p2 = p3;

			if (iterator < picked.end())
				iterator = std::find(++iterator, picked.end(), true);
			
			p3 = (iterator != picked.end()) ? static_cast<int>(std::distance(picked.begin(), iterator)) : p2;
		}

		auto errorIterator = std::max_element(errorSums.begin(), errorSums.end());
		if (*errorIterator < threshold)
			break;

		picked[sectionMiddles[std::distance(errorSums.begin(), errorIterator)]] = true;
	}

	
	std::vector<Animation::Frame> newRotations;
	for (int point = 0; point < rotations.size(); point++)
	{
		if (picked[point])
			newRotations.push_back(rotations[point]);
	}
	rotations = newRotations;
}

float pa::Animation::getError(const DirectX::XMVECTOR& origin, const DirectX::XMVECTOR& other) const
{
	using namespace DirectX;

	const XMVECTOR	difference = other - origin;
	const float		error = XMVectorGetX(XMVector4LengthSq(difference));

	return error;
}
