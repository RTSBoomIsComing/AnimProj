#pragma once

namespace pa
{
	class Skeleton;
	class Animation;
	class SkeletonPose
	{
	public:
		SkeletonPose(Skeleton* skeleton, Animation* animation, uint32_t key);
		~SkeletonPose() = default;

		inline Skeleton* getSkeleton() const { return _skeleton; }
		void	createMatrices();
		void	destroyMatrices();

		std::vector<DirectX::XMMATRIX> const& getMatrices() const;
		std::vector<DirectX::XMFLOAT4> const& getRotations() const;
	private:
		Skeleton*						_skeleton;
		std::vector<DirectX::XMFLOAT4>	_rotationsPerBone;
		std::vector<DirectX::XMMATRIX>	_matricesPerBone;
	};
}

