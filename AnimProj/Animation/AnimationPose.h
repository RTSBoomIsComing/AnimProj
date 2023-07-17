// author: Changwan Yu
#pragma once
namespace pa
{
	class AnimationPose
	{
	public:
		struct Transform
		{
			DirectX::XMFLOAT4 scale;
			DirectX::XMFLOAT4 rotation;
			DirectX::XMFLOAT4 position;
		};

		AnimationPose() = default;
		~AnimationPose() = default;

		const Transform&	getBoneTransform(uint16_t bone)	const	{ return _boneTransforms[bone]; }
		size_t				getBoneCount()					const	{ return _boneTransforms.size(); }
	private:
		std::vector<Transform>			_boneTransforms;
	};
}

