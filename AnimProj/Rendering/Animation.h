// author: Changwan Yu
#pragma once

namespace pa
{
	class Animation
	{
	public:
		Animation() = default;
		~Animation() = default;

	public:
		inline std::size_t getFrameCount() const { return _rotations.size(); };
		inline std::size_t getBoneCount() const { return _boneCount; };

		// TODO: implement this function
		DirectX::XMFLOAT4 getRotation(std::size_t frameIndex, std::size_t boneIndex);

	private:
		// quaternion 
		// two dimensions, firstly frame index then secondly bone index
		std::vector<DirectX::XMFLOAT4> _rotations;

		// one dimension, per frame
		std::vector<DirectX::XMFLOAT4> _rootPositions;

		// count of bones that of skeleton binded
		// if need to get bone count, dont use this directly
		// rather use getBoneCount()
		std::size_t _boneCount;

	};
}


