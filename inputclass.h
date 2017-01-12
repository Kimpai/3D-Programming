#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#include <dinput.h>
#include "positionclass.h"

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();
	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame(float, float&, float&, float&, float&, float&, float&);
	bool IsEscapePressed();
	bool IsLeftPressed();
	bool IsUpPressed();
	bool IsZPressed();
	bool IsXPressed();
	bool TurnUp();
	bool TurnDown();
	bool TurnRight();
	bool TurnLeft();
	bool IsDownPressed();
	bool IsRightPressed();
	void HandelInput(float, float&, float&, float&, float&, float&, float&);
	void UpdatePosition(float, float, float);
	void UpdateRotation(float, float, float);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

	unsigned char m_keyboardState[256];
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;
	DIMOUSESTATE m_mouseState, m_lastMouseState;
	IDirectInput8* m_directInput; 
	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
	PositionClass* m_position;
};
