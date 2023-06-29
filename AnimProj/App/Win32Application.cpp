// author: Changwan Yu
#include "pch.h"
#include "Win32Application.h"

static const wchar_t* WindowClassName = L"AnimProjWindow";

pa::Win32Application::Win32Application()
{
	_width = 1280;
	_height = 720;

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
}

pa::Win32Application::~Win32Application()
{
    ::DestroyWindow(_hWnd);
    ::UnregisterClassW(WindowClassName, GetModuleHandle(NULL));
}

HWND pa::Win32Application::GetHwnd() const
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
