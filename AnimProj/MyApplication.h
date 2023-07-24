// author: Changwan Yu
#pragma once
#include "App/Win32Application.h"
#include "Timer.h"
namespace pa
{
	class Camera;
	class Mesh;
	class Character;

	class MyApplication : public Win32Application
	{
	public:
		MyApplication();
		~MyApplication();
		virtual void OnUpdate(void) override;
		virtual void OnRender(void) override;
		virtual void OnKeyDown(UINT8 key) override;
		virtual void OnKeyUp(UINT8 key) override;

	private:
		void initializeD3dDevices(HWND hWnd);
		void initializeGraphicsPipeline();
		void processInput(float deltaTime);

	private:
		ComPtr<ID3D11Device>				_device;
		ComPtr<ID3D11DeviceContext>			_deviceContext;
		ComPtr<IDXGISwapChain>				_swapChain;
		ComPtr<ID3D11RenderTargetView>		_renderTargetView;
		ComPtr<ID3D11DepthStencilView>		_depthStencilView;
		ComPtr<ID3D11DepthStencilState>		_depthStencilState;
		D3D11_VIEWPORT						_viewport;

		ComPtr<ID3D11InputLayout>			_inputLayout;
		ComPtr<ID3D11VertexShader>			_vertexShader;
		ComPtr<ID3D11PixelShader>			_pixelShader;
		ComPtr<ID3D11RasterizerState>		_rasterizerState;

	private:
		Timer	_timer;

		float	_clearColor[4]			= { 0.2f, 0.4f, 0.6f, 1.0f };

		Camera*							_camera		= nullptr;
		Character*						_character	= nullptr;

	private:
		int		_animationIndex			= 0;
		float	_animationBlendFactor	= 0.0f;
	};
}

