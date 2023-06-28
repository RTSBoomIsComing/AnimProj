#pragma once

namespace pa
{
	class Win32Application;

	class Win32Framework
	{
	public:
		Win32Framework(Win32Application* application);
		int Run() noexcept;

	private:
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	};
}


