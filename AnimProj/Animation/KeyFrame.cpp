#include "pch.h"
#include "KeyFrame.h"

DirectX::XMVECTOR pa::KeyFrame::playCatmullRomCyclic(FrameIndex frameIndex, size_t& P1)
{
	using namespace DirectX;

	if (frameIndices.empty())
		return { 0.0f, 0.0f, 0.0f, 1.0f };

	//if (frameIndices.back() < frameIndex)
	//	;

	auto iteratorP1 = std::lower_bound(frameIndices.begin() + P1, frameIndices.end(), frameIndex);
	P1 = std::distance(frameIndices.begin(), iteratorP1);

	size_t P0 = (P1 > 0) ? P1 - 1 : frameIndices.size() - 1;
	size_t P2 = (P1 + 1) % frameIndices.size();
	size_t P3 = (P2 + 1) % frameIndices.size();

	FrameIndex	frameIndex0 = frameIndices[P0];
	FrameIndex	frameIndex1 = frameIndices[P1];
	FrameIndex	frameIndex2 = frameIndices[P2];
	FrameIndex	frameIndex3 = frameIndices[P3];

	assert(frameIndex1 != frameIndex2);


	float	t = static_cast<float>(frameIndex - frameIndex1) / (frameIndex2 - frameIndex1);
	if (frameIndex2 < frameIndex1)
		t = 0.5f;
	
	XMVECTOR quaternion = XMVectorCatmullRom(
		XMLoadFloat4(&values[P0]),
		XMLoadFloat4(&values[P1]),
		XMLoadFloat4(&values[P2]),
		XMLoadFloat4(&values[P3]), t);

	return quaternion;
}
