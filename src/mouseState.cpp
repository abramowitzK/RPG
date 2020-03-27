#include <mouseState.h>
void MouseState::SetPosition(const int x, const int y)
{
	mX = x;
	mY = y;
}

int MouseState::GetMouseX() const
{
	return mX;
}

int MouseState::GetMouseY() const
{
	return mY;
}

int MouseState::GetMouseRelativeX() const
{
	return mXRel;
}

int MouseState::GetMouseRelativeY() const
{
	return mYRel;
}

bool MouseState::GetLeftButtonDown() const
{
	return mMouseButtonState[0];
}

void MouseState::SetLeftButton(bool down)
{
	mMouseButtonState[0] = down;
}

bool MouseState::GetRightButtonDown() const
{
	return mMouseButtonState[1];
}

void MouseState::SetRightButtonDown(bool down)
{
	mMouseButtonState[1] = down;
}

bool MouseState::HasScrolled() const
{
	return 0 != mScrollAmount;
}

int MouseState::GetScrollAmount() const
{
	return mScrollAmount;
}

void MouseState::SetScrollAmount(int amt)
{
	mScrollAmount = amt;
}

void MouseState::IncrementRelativePosition(int x, int y)
{
	mXRel += x;
	mYRel += y;
}
