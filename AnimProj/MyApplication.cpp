// author: Changwan Yu
#include "pch.h"
#include "MyApplication.h"
#include "Rendering/Camera.h"
#include "Rendering/Mesh.h"
#include "Rendering/StickMesh.h"
#include "Rendering/CubeMesh.h"
#include "Animation/AnimationManager.h"
#include "Rendering/MeshManager.h"
#include "App/ImGuiManager.h"

pa::MyApplication::MyApplication()
{
	using namespace DirectX;

	initializeD3dDevices(getHwnd());
	initializeGraphicsPipeline();

	_imguiManager = std::make_unique<ImGuiManager>(getHwnd(), _device.Get(), _deviceContext.Get());

	AnimationManager::get().initialize();
	MeshManager::get().initialize(_device.Get());

	_camera		= new Camera(_device.Get());
	_characters.emplace_back(_device.Get());
}

pa::MyApplication::~MyApplication()
{
	if (nullptr != _camera)
	{
		delete _camera;
		_camera = nullptr;
	}

	//if (nullptr != _character)
	//{
	//	delete _character;
	//	_character = nullptr;	
	//}
}

void pa::MyApplication::onUpdate()
{
	using namespace DirectX;
	
	_timer.update();
	processInput(_timer.getDeltaTime());

	_camera->update(_deviceContext.Get());
	for (Character& character : _characters)
	{
		character.update(_timer.getDeltaTime(), _deviceContext.Get());
	}
}

void pa::MyApplication::onRender()
{
	// rendering screen 
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	_deviceContext->RSSetViewports(1, &_viewport);
	_deviceContext->OMSetDepthStencilState(_depthStencilState.Get(), 0);

	// renderer
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->IASetInputLayout(_inputLayout.Get());
	_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
	_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
	_deviceContext->RSSetState(_rasterizerState.Get());


	this->renderScene();
	this->renderImGui();

	// renderer
	_swapChain->Present(0, 0);
}

void pa::MyApplication::renderScene(void)
{
	for (Character& character : _characters)
	{
		character.render(_deviceContext.Get());
	}
}

void pa::MyApplication::renderImGui(void)
{
	_imguiManager->newFrame();

	static bool show_demo_window = true;
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	_imguiManager->endFrame();
}

void pa::MyApplication::initializeGraphicsPipeline()
{
	{
		std::wstring csoFilePath = _OUTDIR;
		csoFilePath.append(L"BasicVertexShader.cso");

		ComPtr<ID3DBlob> vertexShaderBlob;
		checkResult(D3DReadFileToBlob(csoFilePath.c_str(), &vertexShaderBlob));
		checkResult(_device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &_vertexShader));


		D3D11_INPUT_ELEMENT_DESC inputElementDescs[2] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		checkResult(_device->CreateInputLayout(
			inputElementDescs, ARRAYSIZE(inputElementDescs),
			vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &_inputLayout));
	}

	{
		std::wstring csoFilePath = _OUTDIR;
		csoFilePath.append(L"BasicPixelShader.cso");

		ComPtr<ID3DBlob> pixelShaderBlob;
		checkResult(D3DReadFileToBlob(csoFilePath.c_str(), &pixelShaderBlob));
		checkResult(_device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &_pixelShader));
	}

	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME D3D11_FILL_SOLID
		rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE D3D11_CULL_BACK
		rasterizerDesc.FrontCounterClockwise = FALSE;
		checkResult(_device->CreateRasterizerState(&rasterizerDesc, &_rasterizerState));
	}
}

void pa::MyApplication::processInput(float deltaTime)
{
	using namespace DirectX;
	float cameraDistance = 10.f;


	for (Character& character : _characters)
	{
		character.processInput(deltaTime);
	}

	_camera->processInput(deltaTime);
}

void pa::MyApplication::initializeD3dDevices(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	const D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
	};
	D3D_FEATURE_LEVEL featureLevel = {};
	checkResult(D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
		&swapChainDesc, _swapChain.GetAddressOf(), _device.GetAddressOf(), &featureLevel, _deviceContext.GetAddressOf()));

	ComPtr<ID3D11Texture2D> backBuffer;
	checkResult(_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
	checkResult(_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &_renderTargetView));


	_viewport = CD3D11_VIEWPORT(backBuffer.Get(), _renderTargetView.Get());

	{
		// Create the depth stencil buffer
		D3D11_TEXTURE2D_DESC backBufferDesc;
		backBuffer->GetDesc(&backBufferDesc);
		CD3D11_TEXTURE2D_DESC depthStencilBufferDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT, backBufferDesc.Width, backBufferDesc.Height,
			1, 0, D3D11_BIND_DEPTH_STENCIL);

		ComPtr<ID3D11Texture2D> depthStencilBuffer;
		checkResult(_device->CreateTexture2D(&depthStencilBufferDesc, nullptr, &depthStencilBuffer));

		// Create the main depth stencil view
		CD3D11_DEPTH_STENCIL_VIEW_DESC dsviewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
		checkResult(_device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsviewDesc, &_depthStencilView));
	}

	{
		// Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
		checkResult(_device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilState));
	}
}
