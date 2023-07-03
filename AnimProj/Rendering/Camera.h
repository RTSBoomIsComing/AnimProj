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
		Camera();
		~Camera() = default;


		void initialize(void);
		void updateAspectRatio(float aspectRatio);
		void updateEyePosition(const DirectX::XMVECTOR& eyePosition);
		void updateFocusPosition(const DirectX::XMVECTOR& focusPosition);

		const Matrices& getMatrices(void);

	private:
		void calculateMatrices();

	private:
		Matrices			_matrices;
		DirectX::XMFLOAT4	_eyePosition;
		DirectX::XMFLOAT4	_focusPosition;
		float				_aspectRatio;
		bool				_dirtyBit;
	};
}

