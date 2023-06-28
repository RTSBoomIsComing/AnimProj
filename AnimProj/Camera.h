// author: Changwan Yu
#pragma once
namespace pa
{
	class Camera
	{
	public:
		struct Matrices
		{
			DirectX::XMFLOAT4X4 View;
			DirectX::XMFLOAT4X4 Projection;
		};

	public:
		void initializeCamera(float viewWidth, float viewHeight);
		void updateMatrices();
		const Matrices&	getMatrices() const;


	private:
		Matrices _matrices;
		float				_viewWidth;
		float				_viewHeight;
		DirectX::XMFLOAT4	_eyePosition;
		DirectX::XMFLOAT4	_focusPosition;
	};
}

