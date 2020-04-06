#pragma once
#include <utils.h>

class KeyState
{
public:
	/**
		Sets a key down in mCurrent(sets to 1)
		@param code Represents the code from the standard USB keyboard codes. This should always use Input::Keys
		*/
	void KeyDown(const int code);
	/**
		Sets a key up in mCurrent (sets to 0)
		@param code Represents the code from the standard USB keyboard codes. This should always use Input::Keys
		*/
	void KeyUp(const int code);
	/**
		Checks if key is currently down
		@param code Represents the code from the standard USB keyboard codes. This should always use Input::Keys
		@return Returns true if key is down and false otherwise
		*/
	bool IsKeyPressed(const int code) const;

private:
	static const u32 INPUT_SIZE = 512;
	bool m_keyboardState[INPUT_SIZE];
};