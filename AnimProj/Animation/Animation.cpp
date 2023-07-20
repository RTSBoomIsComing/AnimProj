#include "pch.h"
#include "Animation.h"
#include "../AcclaimMotionCapture/ASF.h"
#include "../AcclaimMotionCapture/AMC.h"
#include "Skeleton.h"

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
				XMMATRIX rotation = ASF::eulerRotation(&dataBuffer[0], acclaimSkeleton->_axisOrders[boneIndex]);
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
		fitBoneAnimationCatmullRom(boneAnimation.position);
		fitBoneAnimationCatmullRom(boneAnimation.scale);
	}

	testCreateTrack();
	bool isOk = this->validateAnimationCompression();
	for (const auto& keyframe : _rotationTrack)
	{
		auto compact = CompactKeyframe::createFromQuaternion(XMLoadFloat4(&keyframe._v));
		compact.keytime = keyframe._keytime;
		using namespace DirectX;
		XMVECTOR decomp = compact.decompressAsQuaternion();
		XMVECTOR origin = XMLoadFloat4(&keyframe._v);
		XMVECTOR comp = XMVectorNearEqual(origin, decomp, XMVectorReplicate(0.000022f));
		assert(XMVectorGetX(comp));
		assert(XMVectorGetY(comp));
		assert(XMVectorGetZ(comp));
		assert(XMVectorGetW(comp));
		
		_compactTrack.push_back(compact);
	}
}

void pa::Animation::testCreateTrack()
{

	std::vector<uint16_t> cursors(_boneAnimation.size(), 0ui16);
	for (uint16_t i = 0; i < _boneAnimation.size(); i++)
	{
		const auto& currentTrack = _boneAnimation[i].rotation;
		if (currentTrack.empty())
			continue;

		_trackDescriptors.push_back(i);

		Keyframe keyframe{ currentTrack.front().key, i, currentTrack.front().v };
		_rotationTrack.push_back(keyframe);
		_rotationTrack.push_back(keyframe);

		keyframe._keytime	= currentTrack[1].key;
		keyframe._v			= currentTrack[1].v;
		_rotationTrack.push_back(keyframe);

		if (2 < currentTrack.size())
		{
			keyframe._keytime	= currentTrack[2].key;
			keyframe._v			= currentTrack[2].v;
		}

		_rotationTrack.push_back(keyframe);
		cursors[i] = 2;

	}

	
	for (uint16_t currentKeytime = 0; currentKeytime < _duration; currentKeytime++)
	{
		for (uint16_t i = 0; i < _boneAnimation.size(); i++)
		{
			const auto& currentTrack = _boneAnimation[i].rotation;
			uint16_t& cursor = cursors[i];
			if (currentTrack.size() == cursor)
				continue;

			if (currentTrack[cursor - 1].key == currentKeytime)
			{
				cursor += 1;
				//assert(cursor < currentTrack.size());
				if (cursor == currentTrack.size())
				{

					Keyframe keyframe{ currentTrack.back().key , i, currentTrack.back().v };
					_rotationTrack.push_back(keyframe);
				}
				else
				{
					Keyframe keyframe{ currentTrack[cursor].key, i, currentTrack[cursor].v };
					_rotationTrack.push_back(keyframe);
				}
			}
		}
	}
}

void pa::Animation::fitBoneAnimationCatmullRom(std::vector<Animation::Frame>& frames, float threshold)
{
	using namespace DirectX;

	if (frames.empty())
		return;

	std::vector<bool>		samples(frames.size(), false);
	samples.front()			= true;
	samples.back()			= true;

	while(true)
	{
		std::vector<size_t>	midPoints;
		std::vector<float>	errors;

		size_t P0 = 0;

		auto iterator = std::find(samples.begin(), samples.end(), true);
		size_t P1 = std::distance(samples.begin(), iterator);

		iterator = std::find(++iterator, samples.end(), true);
		size_t P2 = std::distance(samples.begin(), iterator);

		iterator = std::find(++iterator, samples.end(), true);
		size_t P3 = (iterator != samples.end()) ? std::distance(samples.begin(), iterator) : P2;

		while (P1 < P2)
		{
			midPoints.push_back((P1 + P2) / 2);
			errors.push_back(0.0f);

			for (size_t between = P1 + 1; between < P2; between++)
			{
				const float t = static_cast<float>(between - P1) / (P2 - P1);
				XMVECTOR catmullRom = XMVectorCatmullRom(
					XMLoadFloat4(&frames[P0].v),
					XMLoadFloat4(&frames[P1].v),
					XMLoadFloat4(&frames[P2].v),
					XMLoadFloat4(&frames[P3].v), t);

				const XMVECTOR	difference = XMLoadFloat4(&frames[between].v) - catmullRom;
				const float error = XMVectorGetX(XMVector4LengthSq(difference));
				//errors.back() += error;

				if (errors.back() < error)
				{
					errors.back() = error;
					midPoints.back() = between;
				}
			}

			P0 = P1;
			P1 = P2;
			P2 = P3;

			if (iterator != samples.end())
				iterator = std::find(++iterator, samples.end(), true);
			
			if (iterator != samples.end())
				P3 = std::distance(samples.begin(), iterator);
		}

		if (errors.empty())
			break;

		auto findMaxError = std::max_element(errors.begin(), errors.end());
		if (*findMaxError < threshold)
			break;

		samples[midPoints[std::distance(errors.begin(), findMaxError)]] = true;
	}

	std::vector<Animation::Frame> newframes;
	for (size_t point = 0; point < frames.size(); point++)
	{
		if (samples[point])
			newframes.push_back(frames[point]);
	}
	frames = newframes;
}

void pa::Animation::fitBoneAnimationCatmullRomCyclic(std::vector<Animation::Frame>& frames, float threshold)
{
	using namespace DirectX;

	if (frames.size() < 4)
		return;

	std::vector<bool>		picked(frames.size(), false);
	picked.front() = true;
	picked[frames.size() / 4] = true;
	picked[frames.size() / 2] = true;
	picked.back() = true;

	while (true)
	{
		std::vector<size_t>	sectionMiddles;
		std::vector<float>	errorSums;

		size_t P0 = frames.size() - 1;

		auto iterator = std::find(picked.begin(), picked.end(), true);
		size_t P1 = std::distance(picked.begin(), iterator);

		iterator = std::find(++iterator, picked.end(), true);
		size_t P2 = std::distance(picked.begin(), iterator);

		iterator = std::find(++iterator, picked.end(), true);
		size_t P3 = std::distance(picked.begin(), iterator);

		while (P1 < P2)
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
				errorSums.back() += error;

				//if (errorSums.back() < error)
				//{
				//	errorSums.back() = error;
				//	sectionMiddles.back() = between;
				//}
			}

			P0 = P1;
			P1 = P2;
			P2 = P3;

			if (iterator < picked.end())
				iterator = std::find(++iterator, picked.end(), true);

			P3 = (iterator != picked.end()) ? static_cast<int>(std::distance(picked.begin(), iterator)) : 0;
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

bool pa::Animation::validateAnimationCompression()
{
	std::vector<std::array<Keyframe, 4>> active(_trackDescriptors.size());

	//for (uint16_t i = 0; i < _trackCount * 4; i++)
	//{
	//	auto const& keyframe = _rotationTrack[i];
	//	active[i / 4][i % 4] = keyframe;
	//}
	std::memcpy(active.data(), _rotationTrack.data(), sizeof(Keyframe) * 4 * _trackDescriptors.size());
	
	uint32_t cursor = _trackDescriptors.size() * 4;
	for (uint16_t t = 0; t < _duration; t++)
	{
		for (auto& cp : active)
		{
			if (cp[2]._keytime < t)
			{
				cp[0] = cp[1];
				cp[1] = cp[2];
				cp[2] = cp[3];
				cp[3] = _rotationTrack[cursor++];
			}
			assert(cp[0]._bone == cp[1]._bone 
				&& cp[1]._bone == cp[2]._bone
				&& cp[2]._bone == cp[3]._bone);

			assert(cp[0]._keytime <= cp[1]._keytime
				&& cp[1]._keytime <= cp[2]._keytime
				&& cp[2]._keytime <= cp[3]._keytime);
		}
	}

	return true;
}

