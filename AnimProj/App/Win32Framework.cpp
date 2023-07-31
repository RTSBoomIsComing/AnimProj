// author: Changwan Yu
#include "pch.h"
#include "Win32Framework.h"
#include "Win32Application.h"
#include <DirectXTK/Keyboard.h>
#include <DirectXTK/Mouse.h>

pa::Win32Framework::Win32Framework(Win32Application* pApplication)
{
	const HWND hWnd = pApplication->getHwnd();
	::SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApplication));
	::SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc));
	::ShowWindow(hWnd, SW_SHOWDEFAULT);
}

int pa::Win32Framework::Run() noexcept
{
	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}
	}
	return static_cast<char>(msg.wParam);
}

LRESULT pa::Win32Framework::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	Win32Application* pApplication = reinterpret_cast<Win32Application*>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			// This is where you'd implement the classic ALT+ENTER hotkey for fullscreen toggle
			// ...
		}
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;


	case WM_PAINT:
		if (nullptr != pApplication)
		{
			pApplication->OnUpdate();
			pApplication->OnRender();
		}
		return 0;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProcW(hWnd, message, wParam, lParam);
}

