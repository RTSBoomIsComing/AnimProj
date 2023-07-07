#include "pch.h"
#include "AnimationCompress.h"
#include "Animation.h"
#include "Quantization.h"

pa::AnimationCompress::AnimationCompress(pa::Animation* pAnimation)
{
	using namespace DirectX;
	
	const size_t	boneCount	= pAnimation->getBoneCount();

	for (size_t i = 0; i < boneCount; i++)
	{
		fitCurve(pAnimation, i);
	}
	auto compareFrame = [](const CompressedFrame& a, const CompressedFrame& b) {
		return a.getKeyTime() < b.getKeyTime();
	};

	// need more fast sorting algorithm
	// or population algorithm
	std::sort(_compressedFrames.begin(), _compressedFrames.end(), compareFrame);
	
	for (auto& frame : _compressedFrames)
	{

		auto v = Quantization::deQuantize(frame.getQuantized());
		std::cout << v.x << " " << v.y << " " << v.z << " " << v.w << std::endl;
	}

}

float pa::AnimationCompress::getError(const DirectX::XMVECTOR& origin, const DirectX::XMVECTOR& other) const
{
	using namespace DirectX;

	const XMVECTOR	difference = other - origin;
	const float		error = XMVectorGetX(XMVector4LengthSq(difference));

	return error;
}

void pa::AnimationCompress::fitCurve(const Animation* pAnimation, size_t boneIndex)
{
	using namespace DirectX;

	const size_t			frameCount	= pAnimation->getFrameCount();
	const size_t			boneCount	= pAnimation->getBoneCount();
	std::vector<size_t>		controlPoints = { 0, frameCount - 1 };

	for (size_t nIteration = 0; nIteration < frameCount; nIteration++)
	{

		size_t	cursor			= 0;
		size_t	p0				= controlPoints[0];
		size_t	p1				= controlPoints[cursor++];
		size_t	p2				= controlPoints[cursor++];
		size_t	p3				= (cursor < controlPoints.size()) ? controlPoints[cursor++] : controlPoints.back();
		size_t	maxErrorIndex	= std::numeric_limits<size_t>::max();

		float			maxError = 0.0f;
		bool			needFitting = false;
		constexpr float	threshold = 0.01f;

		for (size_t i = 0; i < frameCount; i++)
		{
			if (std::find(controlPoints.begin(), controlPoints.end(), i) != controlPoints.end())
				continue;	// skip already collected index

			if (p2 < i)
			{
				p0 = p1;
				p1 = p2;
				p2 = p3;
				if (cursor < controlPoints.size() - 1)
					p3 = controlPoints[++cursor];
				else
					p3 = controlPoints.back();
			}

			const float t = static_cast<float>(i - p1) / (p2 - p1);

			XMVECTOR catmullRom = XMVectorCatmullRom(
				XMLoadFloat4(&pAnimation->getRotation(p0, boneIndex)),
				XMLoadFloat4(&pAnimation->getRotation(p1, boneIndex)),
				XMLoadFloat4(&pAnimation->getRotation(p2, boneIndex)),
				XMLoadFloat4(&pAnimation->getRotation(p3, boneIndex)), t);

			const float error = getError(XMLoadFloat4(&pAnimation->getRotation(i, boneIndex)), catmullRom);

			if (maxError < error)
			{
				maxError = error;
				maxErrorIndex = i;
			}
		}

		if (maxError < threshold)
			break;

		for (auto it = controlPoints.rbegin(); it < controlPoints.rend(); it++)
		{
			if (*it < maxErrorIndex)
			{
				controlPoints.insert(it.base(), maxErrorIndex);
				break;
			}
		}
	}

	// For Test, need to remove
	std::cout << controlPoints.size() << std::endl;
	
	for (size_t p : controlPoints)
	{
		CompressedFrame frame = {};
		frame.setChannelType(ChannelType::Rotation);
		frame.setJointIndex(static_cast<uint16_t>(boneIndex));
		frame.setKeyTime(static_cast<uint16_t>(p));
		frame.setQuantized(Quantization::quantize(pAnimation->getRotation(p, boneIndex)));

		_compressedFrames.push_back(frame);
	}
}
