// author: Changwan Yu
#pragma once
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

namespace pa
{
	class ImGuiManager
	{
	public:
		ImGuiManager(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		~ImGuiManager();
	};
}
