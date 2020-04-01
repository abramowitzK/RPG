#pragma once
class MouseState
{
public:
	void SetPosition(const int x, const int y);
	int GetMouseX() const;
	int GetMouseY() const;
	int GetMouseRelativeX() const;
	int GetMouseRelativeY() const;
	bool GetLeftButtonDown() const;
	void SetLeftButton(bool down);
	bool GetRightButtonDown() const;
	void SetRightButtonDown(bool down);
	void IncrementRelativePosition(int x, int y);
	bool HasScrolled() const;
	int GetScrollAmount() const;
	void SetScrollAmount(int amt);

private:
	const static int NUM_MOUSE_BUTTONS = 4;
	char mMouseButtonState[NUM_MOUSE_BUTTONS];
	int mX;
	int mY;
	int mScrollAmount;
	int mXRel;
	int mYRel;
};