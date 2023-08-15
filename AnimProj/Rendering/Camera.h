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
		void update(ID3D11DeviceContext* deviceContext);
		void processInput(float deltaTime);

		const Matrices& getMatrices(void);

	private:
		void calculateMatrices();

	private:
		ComPtr<ID3D11Buffer> _cameraConstantBuffer;


		Matrices		  _matrices;
		DirectX::XMFLOAT3 _eyePosition	  = {0.0f, 10.0f, 0.0f};
		DirectX::XMFLOAT3 _eyeDirection	  = {0.0f, 0.0f, 1.0f};
		DirectX::XMFLOAT3 _upDirection	  = {0.0f, 1.0f, 0.0f};
		DirectX::XMFLOAT3 _rightDirection = {1.0f, 0.0f, 0.0f};

		float _eyePitch	   = DirectX::XM_PIDIV4;
		float _eyeYaw	   = DirectX::XM_PIDIV4;
		float _eyeDistance = 30.0f;
		float _aspectRatio;
	};
}
