// author: Changwan Yu
#pragma once
#include "Win32Application.h"

namespace pa
{
	class Win32D3D11Application : public Win32Application
	{
	public:
		Win32D3D11Application();

	protected:
		ComPtr<ID3D11Device>			_device;
		ComPtr<ID3D11DeviceContext>		_deviceContext;
		ComPtr<IDXGISwapChain>			_swapChain;
		ComPtr<ID3D11RenderTargetView>	_renderTargetView;
		ComPtr<ID3D11DepthStencilView>	_depthStencilView;
		ComPtr<ID3D11DepthStencilState>	_depthStencilState;
		D3D11_VIEWPORT _viewport;

	private:
		void initializeD3D11Application();

	};
}

