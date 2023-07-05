// author: Changwan Yu
#pragma once

namespace pa
{
	class Animation
	{
		friend class AMC;
	public:
		Animation() = default;
		~Animation() = default;

	public:
		void						initialize(size_t frameCount, size_t boneCount);
		inline std::size_t			getFrameCount() const { return _frameCount; };
		inline std::size_t			getBoneCount() const { return _boneCount; };
		
		DirectX::XMFLOAT4 const&	getRotation(std::size_t frameIndex, std::size_t boneIndex) const;
		DirectX::XMFLOAT4 const&	getPosition(std::size_t frameIndex, std::size_t boneIndex) const;
		DirectX::XMFLOAT4&			getRotation(std::size_t frameIndex, std::size_t boneIndex);
		DirectX::XMFLOAT4&			getPosition(std::size_t frameIndex, std::size_t boneIndex);

	private:
		size_t _boneCount	= 0;
		size_t _frameCount	= 0;

		// represent two dimensional array from
		// 1. frame index
		// 2. bone index
		std::vector<DirectX::XMFLOAT4> _rotations; // quarternion
		std::vector<DirectX::XMFLOAT4> _positions;
	};
}


