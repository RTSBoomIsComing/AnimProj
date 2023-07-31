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

		HWND	getHwnd()	const;
		int		getWidth()	const;
		int		getHeight()	const;

	private:
		HWND	_hWnd;
		int		_width;
		int		_height;		
	};
}
