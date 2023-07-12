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


	// If frameCount is 121 then total duration is 120
	// Because the frameIndex 0 is just base pose and not included in duration
	_duration = acclaimMotion->getFrameCount() - 1;
	_boneAnimation.resize(acclaimSkeleton->getBoneCount());


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
				size_t preRotationPropagateBoneIndex = acclaimSkeleton->getParentBoneIndex(boneIndex);
				
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
		fitBoneAnimationCatmullRom(boneAnimation.rotation);
		//fitBoneAnimationCatmullRom(boneAnimation.position);
		//fitBoneAnimationCatmullRom(boneAnimation.scale);
	}
}

DirectX::XMVECTOR pa::Animation::getBoneRotation(size_t boneIndex, uint32_t key, uint32_t offset)
{
	using namespace DirectX;
	//if (_boneAnimation[boneIndex].rotation.empty())
	//	return XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };

	return XMQuaternionNormalize(playBoneAnimationCatmullRomCyclic(_boneAnimation[boneIndex].rotation, key));
}

DirectX::XMVECTOR pa::Animation::playBoneAnimationCatmullRom(std::vector<Animation::Frame> const& frames, uint32_t key, uint32_t offset)
{
	using namespace DirectX;

	int frameIndex = key;

	if (frames.empty())
		return { 0.0f, 0.0f, 0.0f, 1.0f };

	assert(offset < frames.size());
	assert(frames.size() >= 2 && "frames need to have two control points, start point and end point");

	constexpr int	frameRate = 120;
	constexpr float interval = 1.0f / frameRate;

	
	XMMATRIX animationRotation = XMMatrixIdentity();

	Animation::Frame findFrame{ key };
	auto findIt = std::upper_bound(frames.begin() + offset, frames.end(), findFrame, 
		[](Animation::Frame const& f1, Animation::Frame const& f2) {
			return f1.key < f2.key; 
		});

	size_t	index2	= (findIt != frames.end()) ? std::distance(frames.begin(), findIt) : frames.size() - 1;
	size_t	index1	= index2 - 1;
	size_t	index0	= (0 < index1) ? index1 - 1 : index1;
	size_t	index3	= (index2 < frames.size() - 1) ? index2 + 1 : index2;

	uint32_t	P0	= frames[index0].key;
	uint32_t	P1	= frames[index1].key;
	uint32_t	P2	= frames[index2].key;
	uint32_t	P3	= frames[index3].key;

	assert(P1 < P2);
	assert(P1 <= key && key <= P2);
	float	t	= static_cast<float>(key - P1) / (P2 - P1);


	XMVECTOR quaternion = XMVectorCatmullRom(XMLoadFloat4(&frames[index0].v), XMLoadFloat4(&frames[index1].v),
			XMLoadFloat4(&frames[index2].v), XMLoadFloat4(&frames[index3].v), t);
	
	// Is need normalize here ? 
	// quaternion = XMQuaternionNormalize(quaternion);
	
	return quaternion;
}

DirectX::XMVECTOR pa::Animation::playBoneAnimationCatmullRomCyclic(std::vector<Animation::Frame> const& frames, uint32_t key, uint32_t offset)
{
	using namespace DirectX;

	if (frames.empty())
		return { 0.0f, 0.0f, 0.0f, 1.0f };

	assert(offset < frames.size());
	assert(frames.size() >= 2 && "frames need to have two control points, start point and end point");
	uint32_t keyModular = _duration + 1;
	key = key % keyModular;

	Animation::Frame findFrame{ key };
	auto findIt = std::upper_bound(frames.begin() + offset, frames.end(), findFrame,
		[](Animation::Frame const& f1, Animation::Frame const& f2) {
			return f1.key < f2.key;
		});

	size_t	index2 = findIt != frames.end() ? std::distance(frames.begin(), findIt) : 0;
	size_t	index3 = (index2 + 1) % frames.size();
	size_t	index1 = (0 < index2) ? index2 - 1 : frames.size() - 1;
	size_t	index0 = (0 < index1) ? index1 - 1 : frames.size() - 1;

	int 	P0 = frames[index0].key;
	int 	P1 = frames[index1].key;
	int 	P2 = frames[index2].key;
	int 	P3 = frames[index3].key;

	assert(P1 != P2);
	//assert(P1 <= key && key <= P2);
	//assert(P1 <= key);
	float	t = static_cast<float>(key - P1) / std::abs(P2 - P1);


	XMVECTOR quaternion = XMVectorCatmullRom(XMLoadFloat4(&frames[index0].v), XMLoadFloat4(&frames[index1].v),
		XMLoadFloat4(&frames[index2].v), XMLoadFloat4(&frames[index3].v), t);

	// Is need normalize here ? 
	// quaternion = XMQuaternionNormalize(quaternion);

	return quaternion;
}

void pa::Animation::fitBoneAnimationCatmullRom(std::vector<Animation::Frame>& frames, float threshold)
{
	using namespace DirectX;

	if (frames.empty())
		return;

	std::vector<bool>		picked(frames.size(), false);
	picked.front()			= true;
	picked.back()			= true;

	while(true)
	{
		std::vector<size_t>	sectionMiddles;
		std::vector<float>	errorSums;

		size_t P0 = 0;

		auto iterator = std::find(picked.begin(), picked.end(), true);
		size_t P1 = std::distance(picked.begin(), iterator);

		iterator = std::find(++iterator, picked.end(), true);
		size_t P2 = std::distance(picked.begin(), iterator);

		if (iterator != picked.end())
			iterator = std::find(++iterator, picked.end(), true);

		size_t P3 = (iterator != picked.end()) ? std::distance(picked.begin(), iterator) : P2;

		while (P1 != P2)
		{
			sectionMiddles.push_back((P1 + P2) / 2);
			errorSums.push_back(0.0f);

			for (size_t between = P1 + 1; between < P2; between++)
			{
				const float t = static_cast<float>(between - P1) / (P2 - P1);
				XMVECTOR catmullRom = XMVectorCatmullRom(
					XMLoadFloat4(&frames[P0].v),
					XMLoadFloat4(&frames[P1].v),
					XMLoadFloat4(&frames[P2].v),
					XMLoadFloat4(&frames[P3].v), t);

				catmullRom = XMQuaternionNormalize(catmullRom);

				const XMVECTOR	difference = XMLoadFloat4(&frames[between].v) - catmullRom;
				const float error = XMVectorGetX(XMVector4LengthSq(difference));
				//errorSums.back() += error;

				if (errorSums.back() < error)
				{
					errorSums.back() = error;
					sectionMiddles.back() = between;
				}
			}

			P0 = P1;
			P1 = P2;
			P2 = P3;

			if (iterator < picked.end())
				iterator = std::find(++iterator, picked.end(), true);
			
			P3 = (iterator != picked.end()) ? static_cast<int>(std::distance(picked.begin(), iterator)) : P2;
		}

		auto errorIterator = std::max_element(errorSums.begin(), errorSums.end());
		if (*errorIterator < threshold)
			break;

		picked[sectionMiddles[std::distance(errorSums.begin(), errorIterator)]] = true;
	}

	
	std::vector<Animation::Frame> newframes;
	for (size_t point = 0; point < frames.size(); point++)
	{
		if (picked[point])
			newframes.push_back(frames[point]);
	}
	frames = newframes;
}

