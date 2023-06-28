#pragma once

namespace pa
{
	class Win32Application
	{
	public:
				Win32Application();
		virtual	~Win32Application();

	public:
		virtual void OnUpdate() abstract;
		virtual void OnRender() abstract;

		HWND	GetHwnd()	const;
		int		getWidth()	const;
		int		getHeight()	const;

	private:
		HWND	_hWnd;
		int		_width;
		int		_height;		
	};
}
