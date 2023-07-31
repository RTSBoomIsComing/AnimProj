// author: Changwan Yu
#include "pch.h"
#include "Win32Application.h"
#include <DirectXTK/Keyboard.h>
#include <DirectXTK/Mouse.h>

const wchar_t* pa::Win32Application::WindowClassName = L"AnimProjWindow";

pa::Win32Application::Win32Application()
{
    const wchar_t* windowName = L"MainWindow";

	WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_CLASSDC;
    windowClass.lpfnWndProc = DefWindowProcW;
    windowClass.hInstance = GetModuleHandleW(NULL);
    windowClass.lpszClassName = WindowClassName;
    ::RegisterClassExW(&windowClass);

    RECT rect = {};
    rect.left = 0;
    rect.top = 0;
    rect.right = static_cast<LONG>(_width);
    rect.bottom = static_cast<LONG>(_height);
    ::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    int adjustedWidth = rect.right - rect.left;
    int adjustedHeight = rect.bottom - rect.top;

    _hWnd = ::CreateWindowW(WindowClassName, windowName, WS_OVERLAPPEDWINDOW, 100, 100,
        adjustedWidth, adjustedHeight, NULL, NULL, GetModuleHandle(NULL), NULL);

    _mouse = std::make_unique<DirectX::Mouse>();
    _mouse->SetWindow(_hWnd);

    _keyboard = std::make_unique<DirectX::Keyboard>();
}

pa::Win32Application::~Win32Application()
{
    ::DestroyWindow(_hWnd);
    ::UnregisterClassW(WindowClassName, GetModuleHandle(NULL));
}

void pa::Win32Application::onResize(UINT width, UINT height)
{
    _resizeWidth = width;
    _resizeHeight = height;
}

HWND pa::Win32Application::getHwnd() const
{
    return _hWnd;
}

int pa::Win32Application::getWidth() const
{
    return _width;
}

int pa::Win32Application::getHeight() const
{
    return _height;
}
