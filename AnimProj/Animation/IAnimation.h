#pragma once

namespace pa
{
	class IAnimation
	{
	public:
		IAnimation()			= default;
		virtual ~IAnimation()	= default;

		virtual DirectX::XMVECTOR 	getAnimationScale() abstract;
		virtual DirectX::XMVECTOR 	getAnimationRotation() abstract;
		virtual DirectX::XMVECTOR 	getAnimationPosition() abstract;
	};
}

