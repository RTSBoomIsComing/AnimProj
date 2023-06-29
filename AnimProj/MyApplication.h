// author: Changwan Yu
#pragma once
#include "App/Win32D3D11Application.h"
namespace pa
{
	class Camera;
	class Mesh;
	class ASF;

	class MyApplication : public Win32D3D11Application
	{
	public:
		MyApplication();
		~MyApplication();
		virtual void OnUpdate(void) override;
		virtual void OnRender(void) override;
	
	
	private:
		void initializeGraphicsPipeline();

	private:
		float	_clearColor[4] = { 0.2f, 0.4f, 0.6f, 1.0f };

		ComPtr<ID3D11Buffer>			_cameraConstantBuffer;
		ComPtr<ID3D11Buffer>			_meshConstantBuffer;
		ComPtr<ID3D11InputLayout>		_inputLayout;
		ComPtr<ID3D11VertexShader>		_vertexShader;
		ComPtr<ID3D11PixelShader>		_pixelShader;
		ComPtr<ID3D11RasterizerState>	_rasterizerState;

		Camera* _pCamera;
		Mesh* _pCubeMesh;
		ASF* _pASF;
	};
}

