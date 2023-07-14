// author: Changwan Yu
#pragma once

namespace pa
{
	struct Keyboard
	{
		bool keyState[256] = {};
	};

}

extern pa::Keyboard* GKeyboard;


