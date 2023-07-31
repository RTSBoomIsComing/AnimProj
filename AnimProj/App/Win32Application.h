// author: Changwan Yu
#pragma once
namespace DirectX
{
	class Mouse;
	class Keyboard;
}

namespace pa
{
	class Win32Application
	{
	public:
				Win32Application();
		virtual	~Win32Application();

	public:
		virtual void onUpdate(void) abstract;
		virtual void onRender(void) abstract;

		virtual void onPostResize(void) abstract;
		void onResize(UINT width, UINT height);

		HWND	getHwnd()	const;
		int		getWidth()	const;
		int		getHeight()	const;

	public:
		static const wchar_t* WindowClassName;

	protected:
		HWND	_hWnd;
		UINT	_width			= 1280;
		UINT	_height			= 720;

		UINT	_resizeWidth	= 0;
		UINT	_resizeHeight	= 0;

	private:
		std::unique_ptr<DirectX::Keyboard>	_keyboard;
		std::unique_ptr<DirectX::Mouse>		_mouse;
	};
}
