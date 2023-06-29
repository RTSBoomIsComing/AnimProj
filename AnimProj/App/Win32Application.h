#pragma once

namespace pa
{
	class Win32Application
	{
	public:
				Win32Application();
		virtual	~Win32Application();

	public:
		virtual void OnUpdate(void) abstract;
		virtual void OnRender(void) abstract;
		virtual void OnKeyDown(UINT8 key) {};
		virtual void OnKeyUp(UINT8 key) {};

		HWND	GetHwnd()	const;
		int		getWidth()	const;
		int		getHeight()	const;

	private:
		HWND	_hWnd;
		int		_width;
		int		_height;		
	};
}
