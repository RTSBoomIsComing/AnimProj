// author: Changwan Yu
#pragma once
namespace pa
{

	struct KeyFrame
	{
		using FrameIndex = uint16_t;
		std::vector<FrameIndex>			frameIndices;
		std::vector<DirectX::XMFLOAT4>	values;

		DirectX::XMVECTOR playCatmullRomCyclic(FrameIndex frameIndex, size_t& P1);
	};
}

