#include <keyState.h>

void KeyState::KeyDown(int code)
{
	if (code < INPUT_SIZE)
	{
		m_keyboardState[code] = true;
	}
}

void KeyState::KeyUp(int code)
{
	if (code < INPUT_SIZE)
	{
		m_keyboardState[code] = false;
	}
}

bool KeyState::IsKeyPressed(int code) const
{
	if (code < INPUT_SIZE)
	{
		return m_keyboardState[code];
	}
	return false;
}
