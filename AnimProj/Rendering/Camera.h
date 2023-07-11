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
		Camera(ID3D11Device* device);
		~Camera() = default;


		void initialize();
		void setAspectRatio(float aspectRatio);
		void setEyePosition(const DirectX::XMVECTOR& eyePosition);
		void setFocusPosition(const DirectX::XMVECTOR& focusPosition);
		void update(ID3D11DeviceContext* deviceContext);

		const Matrices& getMatrices(void);

	private:
		void calculateMatrices();

	private:
		ComPtr<ID3D11Buffer>			_cameraConstantBuffer;


		Matrices						_matrices;
		DirectX::XMFLOAT4				_eyePosition;
		DirectX::XMFLOAT4				_focusPosition;
		float							_aspectRatio;
		bool							_dirtyBit;
	};
}

