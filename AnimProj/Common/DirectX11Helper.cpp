// author: Changwan Yu
#include "pch.h"
#include "DirectX11Helper.h"
#include <comdef.h>

bool pa::checkResult(HRESULT result)
{
	if (FAILED(result))
	{
		_com_error error(result);
		LPCTSTR errorMessage = error.ErrorMessage();
		::OutputDebugString(errorMessage);
		DebugBreak();
		return false;
	}
	return true;
}

