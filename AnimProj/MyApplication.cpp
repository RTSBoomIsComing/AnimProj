// author: Changwan Yu
#include "pch.h"
#include "MyApplication.h"
#include "Rendering/Camera.h"
#include "Rendering/Mesh.h"
#include "Rendering/StickMesh.h"
#include "Rendering/CubeMesh.h"
#include "Rendering/Skeleton.h"
#include "Rendering/Character.h"
#include "Animation/Animation.h"
#include "ASFAMC/ASF.h"
#include "ASFAMC/AMC.h"
#include "Animation/Quantization.h"

namespace pa
{
	extern void SandboxModifySkeleton(Skeleton* pSkeleton);
}

pa::MyApplication::MyApplication()
{
	using namespace DirectX;

	initializeD3dDevices(getHwnd());

	_pCamera = new Camera();
	_pStickMesh = new StickMesh(_device.Get());
	_pCubeMesh = new CubeMesh(_device.Get(), 0.25f);

	std::wstring asfFilePath = _SOLUTIONDIR;
	asfFilePath += LR"(Assets\ASFAMC\subject02\02.asf)";
	//asfFilePath += LR"(Assets\ASFAMC\07-walk\07-walk.asf)";
	//asfFilePath += LR"(Assets\ASFAMC\09-run\09-run.asf)";
	//asfFilePath += LR"(Assets\ASFAMC\131-dance\131-dance.asf)";
	//asfFilePath += LR"(Assets\ASFAMC\135-martialArts\135-martialArts.asf)";

	std::wstring amcFilePath = _SOLUTIONDIR;
	//amcFilePath += LR"(Assets\ASFAMC\subject02\walk.amc)";
	//amcFilePath += LR"(Assets\ASFAMC\subject02\runjog.amc)";
	//amcFilePath += LR"(Assets\ASFAMC\subject02\jumpbalance.amc)";
	amcFilePath += LR"(Assets\ASFAMC\subject02\punchstrike.amc)";
	//amcFilePath += LR"(Assets\ASFAMC\07-walk\07_05-walk.amc)";
	//amcFilePath += LR"(Assets\ASFAMC\09-run\09_06-run.amc)";
	//amcFilePath += LR"(Assets\ASFAMC\131-dance\131_04-dance.amc)";
	//amcFilePath += LR"(Assets\ASFAMC\135-martialArts\135_06-martialArts.amc)";


	_pSkeleton = new Skeleton();
	ASF asf(_pSkeleton, asfFilePath.c_str());

	SandboxModifySkeleton(_pSkeleton);


	AMC amc(amcFilePath.c_str());

	_animation = new Animation(&asf, &amc);
	//_animation->compressAnimation();
	for (const auto& boneAnimation : _animation->_boneAnimation)
	{
		std::cout << boneAnimation.rotation.size() << std::endl;
	}

	initializeGraphicsPipeline();

	_worldTransforms.resize(_pSkeleton->getBoneCount());
	_boneStickTransforms.resize(_pSkeleton->getBoneCount());

}

pa::MyApplication::~MyApplication()
{
	if (nullptr != _pCamera)
		delete _pCamera;

	if (nullptr != _pCubeMesh)
		delete _pCubeMesh;

	if (nullptr != _pStickMesh)
		delete _pStickMesh;

	if (nullptr != _pSkeleton)
		delete _pSkeleton;

	if (nullptr != _animation)
		delete _animation;

	if (nullptr != _character)
		delete _character;
}

void pa::MyApplication::OnUpdate()
{
	using namespace DirectX;

	static auto lastTime = std::chrono::high_resolution_clock::now();
	const auto currentTime = std::chrono::high_resolution_clock::now();
	const auto deltaTime = std::chrono::duration<float>(currentTime - lastTime);
	lastTime = currentTime;

	{
		float cameraDistance = 10.f;
		if (keyState[0])
			_cameraRotationFactor += deltaTime.count();
		if (keyState[2])
			_cameraRotationFactor -= deltaTime.count();
		if (keyState[1])
			_cameraHeight += deltaTime.count();
		if (keyState[3])
			_cameraHeight -= deltaTime.count();



		XMVECTOR newEyePosition = XMVECTOR{
			cameraDistance * std::cosf(-XM_PIDIV2 + _cameraRotationFactor * 3),
			_cameraHeight * 15,
			cameraDistance * std::sinf(-XM_PIDIV2 + _cameraRotationFactor * 3),
			1.0f };

		_pCamera->updateEyePosition(newEyePosition);
	}

	_deviceContext->UpdateSubresource(_cameraConstantBuffer.Get(), 0, nullptr, &_pCamera->getMatrices(), 0, 0);
	_deviceContext->VSSetConstantBuffers(0, 1, _cameraConstantBuffer.GetAddressOf());

	struct KeyFrameData 
	{
		int		cursor0 = 0;
		int		cursor1 = 0;
		int		cursor2 = 0;

		// in loop, cursor3 would be adjusted
		int		cursor3 = 1;
		float	t0		= 0.0f;
		float	t1		= 0.0f;
	};

	//static std::vector<KeyFramePair> positions(_pSkeleton->getBoneCount());
	static std::vector<KeyFrameData> keyFrameRotations(_pSkeleton->getBoneCount());

	constexpr int	animationFrameRate = 120;
	constexpr float interval = 1.0f / animationFrameRate;
	static float	playTime = 0.0f; 

	playTime += deltaTime.count();
	if (playTime * animationFrameRate >= _animation->_duration -1)
	{
		playTime = 0.0f;
		for (auto& keyFrameRotation : keyFrameRotations)
		{
			keyFrameRotation = {};
		}
	}

	for (const size_t boneIndex : _pSkeleton->getDFSPath())
	{
		const size_t parentBoneIndex = _pSkeleton->getParentBoneIndex(boneIndex);
		const XMMATRIX& parentWorldTransform = 
			(parentBoneIndex < _pSkeleton->getBoneCount()) ? _worldTransforms[parentBoneIndex] : XMMatrixIdentity();

		const Skeleton::Bone& bone = _pSkeleton->getBone(boneIndex);
		const XMVECTOR bonePosition = XMLoadFloat4(&bone.direction);
		const XMMATRIX boneRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&bone.rotation));
		XMMATRIX boneMatrix			= boneRotation * XMMatrixTranslationFromVector(bonePosition);



		XMMATRIX animationRotation = XMMatrixIdentity();
		if (nullptr != _animation 
			&& false == _animation->_boneAnimation[boneIndex].rotation.empty())
		{
			const auto& animationRotations = _animation->_boneAnimation[boneIndex].rotation;
			
			int cursor = keyFrameRotations[boneIndex].cursor2;
			while (animationRotations[cursor].key < playTime * animationFrameRate)
			{
				cursor = std::min(cursor + 1, static_cast<int>(animationRotations.size()) - 1);
				
				keyFrameRotations[boneIndex].t0			= keyFrameRotations[boneIndex].t1;
				keyFrameRotations[boneIndex].t1			= animationRotations[cursor].key * interval;

				keyFrameRotations[boneIndex].cursor0	= keyFrameRotations[boneIndex].cursor1;
				keyFrameRotations[boneIndex].cursor1	= keyFrameRotations[boneIndex].cursor2;
				keyFrameRotations[boneIndex].cursor2	= cursor;
				keyFrameRotations[boneIndex].cursor3	= std::min(cursor + 1, static_cast<int>(animationRotations.size()) - 1);
			}


			const float	t0	= keyFrameRotations[boneIndex].t0;
			const float	t1	= keyFrameRotations[boneIndex].t1;
			assert(t0 <= playTime && playTime <= t1);

			float		t	= (playTime - t0) / (t1 - t0);

			const int p0 = keyFrameRotations[boneIndex].cursor0;
			const int p1 = keyFrameRotations[boneIndex].cursor1;
			const int p2 = keyFrameRotations[boneIndex].cursor2;
			const int p3 = keyFrameRotations[boneIndex].cursor3;

			XMVECTOR finalQuaternion;
			if (p1 == p2)
			{
				finalQuaternion = XMLoadFloat4(&animationRotations[p1].v);
			}
			else
			{
				finalQuaternion = XMVectorCatmullRom(
					XMLoadFloat4(&animationRotations[p0].v),
					XMLoadFloat4(&animationRotations[p1].v),
					XMLoadFloat4(&animationRotations[p2].v),
					XMLoadFloat4(&animationRotations[p3].v), t);
			}
			finalQuaternion = XMQuaternionNormalize(finalQuaternion);


			//finalQuaternion = XMQuaternionNormalize(XMQuaternionSlerp({ 0.0f, 0.0f, 0.0f, 1.0f }, finalQuaternion, 0.5f));
			animationRotation = XMMatrixRotationQuaternion(finalQuaternion);
		}


		boneMatrix = animationRotation * boneMatrix;
		_worldTransforms[boneIndex] = boneMatrix * parentWorldTransform;

		const float boneStickScale = XMVectorGetX(XMVector3Length(bonePosition));
		if (boneStickScale <= 0)
		{
			_boneStickTransforms[boneIndex] = XMMATRIX{};
			continue;
		}

		const XMVECTOR vec0 = XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f };
		const XMVECTOR vec1 = XMVector3Normalize(bonePosition);
		
		const float		dotProduct		= XMVectorGetX(XMVector3Dot(vec0, vec1));
		const float		angle			= std::acosf(dotProduct);
		const XMVECTOR	rotationAxis	= XMVector3Cross(vec0, vec1);


		_boneStickTransforms[boneIndex] =
			XMMatrixScaling(0.15f, boneStickScale, 0.15f) * XMMatrixRotationAxis(rotationAxis, angle)
			* XMMatrixTranslation(0.f, 0.f, 0.f) * parentWorldTransform;
	}
}

void pa::MyApplication::OnRender()
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

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};

		_deviceContext->Map(_meshConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		//  Update the vertex buffer here.
		std::memcpy(mappedResource.pData, _boneStickTransforms.data(), sizeof(DirectX::XMMATRIX) * _boneStickTransforms.size());

		//  Reenable GPU access to the vertex buffer data.
		_deviceContext->Unmap(_meshConstantBuffer.Get(), 0);
		_deviceContext->VSSetConstantBuffers(1, 1, _meshConstantBuffer.GetAddressOf());
	}
	_pStickMesh->drawInstanced(_deviceContext.Get(), static_cast<UINT>(_pSkeleton->getBoneCount()));

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};

		_deviceContext->Map(_meshConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		//  Update the vertex buffer here.
		std::memcpy(mappedResource.pData, _worldTransforms.data(), sizeof(DirectX::XMMATRIX) * _worldTransforms.size());

		//  Reenable GPU access to the vertex buffer data.
		_deviceContext->Unmap(_meshConstantBuffer.Get(), 0);
		_deviceContext->VSSetConstantBuffers(1, 1, _meshConstantBuffer.GetAddressOf());
	}
	_pCubeMesh->drawInstanced(_deviceContext.Get(), static_cast<UINT>(_pSkeleton->getBoneCount()));

	// renderer
	_swapChain->Present(1, 0);
}

void pa::MyApplication::OnKeyDown(UINT8 key)
{
	// left  : 37
	// up    : 38
	// right : 39
	// down  : 40
	if (37 <= key)
		keyState[key - 37] = true;
}

void pa::MyApplication::OnKeyUp(UINT8 key)
{
	// left  : 37
	// up    : 38
	// right : 39
	// down  : 40
	if (37 <= key)
		keyState[key - 37] = false;
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

	{
		// Create camera constant buffer
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(pa::Camera::Matrices);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		checkResult(_device->CreateBuffer(&bufferDesc, nullptr, &_cameraConstantBuffer));
	}
	{
		// Create mesh constant buffer
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * 100;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		checkResult(_device->CreateBuffer(&bufferDesc, nullptr, &_meshConstantBuffer));
	}
}

void pa::MyApplication::initializeD3dDevices(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
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
