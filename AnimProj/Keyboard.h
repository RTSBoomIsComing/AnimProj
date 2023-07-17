// author: Changwan Yu
#pragma once

namespace pa
{
	class Keyboard
	{
	public:
		Keyboard() = default;
		~Keyboard() = default;
		inline static Keyboard* get()
		{
			static Keyboard sKeyboard;
			return &sKeyboard;
		}
		
		inline bool& getKeyState(uint8_t key) { return _keyState[key]; }

	private:
		bool _keyState[256] = {};
	};

}


