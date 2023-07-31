// author: Changwan Yu
#pragma once
#include "imgui.h"


namespace pa
{
	class ImGuiManager
	{
	public:
		ImGuiManager(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		~ImGuiManager();

		void newFrame();
		void endFrame();
	};
}
