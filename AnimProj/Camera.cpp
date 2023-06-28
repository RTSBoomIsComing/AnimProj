// author: Changwan Yu
#include "pch.h"
#include "Camera.h"

pa::Camera::Camera()
{
	initializeCamera();
}

void pa::Camera::updateMatrices(float viewWidth, float viewHeight)
{
	using namespace DirectX;

	// TODO: Update eyePosition and focusPosition

	XMStoreFloat4x4(&_matrices.View, 
		XMMatrixLookAtLH(XMLoadFloat4(&_eyePosition), XMLoadFloat4(&_focusPosition), XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f }));

	XMStoreFloat4x4(&_matrices.ViewProjection, 
		XMMatrixPerspectiveLH(viewWidth, viewHeight, 1.0f, 1000.0f));
}

const pa::Camera::Matrices& pa::Camera::getMatrices() const
{
	return _matrices;
}

void pa::Camera::initializeCamera()
{
	using namespace DirectX;
	XMStoreFloat4(&_eyePosition, XMVECTOR{ 0.0f, 0.0f, -1.0f, 1.0f });
	XMStoreFloat4(&_eyePosition, XMVECTOR{ 0.0f, 0.0f,  0.0f, 1.0f });

	XMStoreFloat4x4(&_matrices.View, XMMatrixIdentity());
	XMStoreFloat4x4(&_matrices.ViewProjection, XMMatrixIdentity());
}
