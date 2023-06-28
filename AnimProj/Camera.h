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
			DirectX::XMFLOAT4X4 ViewProjection;
		};

	public:
		Camera();
		void updateMatrices(float viewWidth, float viewHeight);
		const Matrices&	getMatrices() const;

		void initializeCamera();

	private:
		Matrices _matrices;
		DirectX::XMFLOAT4 _eyePosition;
		DirectX::XMFLOAT4 _focusPosition;
	};
}

