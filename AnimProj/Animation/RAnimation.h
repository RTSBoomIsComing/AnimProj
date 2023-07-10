// author: Changwan Yu
#pragma once

namespace pa
{
	class ASF;
	class AMC;
	class RAnimation
	{
	public:
		struct Frame
		{
			uint64_t			joint	= std::numeric_limits<uint64_t>::max();
			uint64_t			key		= std::numeric_limits<uint64_t>::max();

			// scale, position or rotation(quaternion)
			DirectX::XMFLOAT4	v		= {};
		};
	public:
		RAnimation() = default;
		RAnimation(const ASF* acclaimSkeleton, const AMC* acclaimMotion);
		~RAnimation() = default;

		bool initializeAnimation(const ASF* acclaimSkeleton, const AMC* acclaimMotion);



	private:
		std::vector<RAnimation::Frame> _scales;
		std::vector<RAnimation::Frame> _rotations;
		std::vector<RAnimation::Frame> _positions;
	};
}

