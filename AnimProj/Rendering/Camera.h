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

		bool checkFrustumWithSphere(DirectX::XMVECTOR center, float radius) const;

	private:
		void calculateMatrices();
		void constructFrustum();

	private:
		ComPtr<ID3D11Buffer> _cameraConstantBuffer;

	private:
		// near, far, left, right, top, bottom
		std::array<DirectX::XMFLOAT4, 6> _frustum;

	private:
		Matrices		  _matrices;
		DirectX::XMFLOAT3 _eyePosition	  = {0.0f, 10.0f, 0.0f};
		DirectX::XMFLOAT3 _eyeDirection	  = {0.0f, 0.0f, 1.0f};
		DirectX::XMFLOAT3 _upDirection	  = {0.0f, 1.0f, 0.0f};
		DirectX::XMFLOAT3 _rightDirection = {1.0f, 0.0f, 0.0f};

		float _eyePitch = DirectX::XM_PIDIV4;
		float _eyeYaw	= 0.0f; //DirectX::XM_PIDIV4;
		float _aspectRatio;

		float _nearZ = 0.01f;
		float _farZ	 = 100.0f;
	};
}
