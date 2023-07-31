// author: Changwan Yu
#pragma once
namespace pa
{
	class Skeleton;
	class SkeletonRenderer
	{
	public:
		SkeletonRenderer(const Skeleton* skeleton);
		~SkeletonRenderer() = default;

		void render(ID3D11DeviceContext* deviceContext, std::vector<DirectX::XMFLOAT4X4> const& jointGTs,
			std::vector<DirectX::XMFLOAT4X4>& jointToJointGTs);

	private:
		const Skeleton* const _skeleton;
		std::vector<DirectX::XMFLOAT4X4> _jointToJointLTs;
	};
}

