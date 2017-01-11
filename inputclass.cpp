#include "inputclass.h"

InputClass::InputClass()
{
	m_keyboard = nullptr;
	m_mouse = nullptr;
	m_directInput = nullptr;
	m_position = nullptr;

}

InputClass::InputClass(const InputClass& other)
{

}

InputClass::~InputClass()
{

}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	//Store the screen size which will be used for positioning the mouse cursor
	m_screenHeight = screenHeight;
	m_screenWidth = screenWidth;

	//Initialize the loacation of the mouse on the screen
	m_mouseX = 0;
	m_mouseY = 0;

	//Initialize the position object
	m_position = new PositionClass;
	if (!m_position)
	{
		return false;
	}

	m_position->SetPosition(0.0f, 1.0f, -5.0f);
	m_position->SetRotation(0.0f, 0.0f, 0.0f);

	//Intialize the main direct input interface
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//Initialize the direct input interface
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	result = ReadKeyboard();
	if (FAILED(result))
	{
		return false;
	}

	result = ReadMouse();
	if (FAILED(result))
	{
		return false;
	}

	m_lastMouseState = m_mouseState;

	return true;
	
}

void InputClass::Shutdown()
{
	//Release stuff
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = nullptr;
	}

	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = nullptr;
	}

	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = nullptr;
	}

	if (m_position)
	{
		delete m_position;
		m_position = nullptr;
	}

}

void InputClass::UpdatePosition(float posX, float posY, float posZ)
{
	m_position->SetPosition(posX, posY, posZ);
}

void InputClass::UpdateRotation(float rotX, float rotY, float rotZ)
{
	m_position->SetRotation(rotX, rotY, rotZ);
}

bool InputClass::Frame(float frameTime, float& posX, float& posY, float& posZ, float& rotX, float& rotY, float& rotZ)
{
	bool result;

	//Read the current state of the keyboard
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	//Read the current state of the mouse
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	//Process the changes in the mouse
	ProcessInput();

	//Handel the input and update the position
	HandelInput(frameTime, posX, posY, posZ, rotX, rotY, rotZ);

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	//Read tthe keyboard device
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		//If the keyboard lost focus or was not acquired then try to get control back
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	//Read the mouse device
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		//If the mouse lost focus or was not acquired then try to get control back
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	//Ensure the mouse loaction doesn't exceed the screen width or height
	if (m_mouseX < 0)
	{
		m_mouseX = 0;
	}

	if (m_mouseY < 0)
	{
		m_mouseY = 0;
	}

	if (m_mouseX > m_screenWidth)
	{
		m_mouseX = m_screenWidth;
	}

	if (m_mouseY > m_screenHeight)
	{
		m_mouseY = m_screenHeight;
	}

	return;
}

bool InputClass::IsEscapePressed()
{
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsLeftPressed()
{
	if (m_keyboardState[DIK_A] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsRightPressed()
{
	if (m_keyboardState[DIK_D] & 0x80)
	{
		return true;
	}

	return false;
}

void InputClass::HandelInput(float frameTime, float& posX, float& posY, float& posZ, float& rotX, float& rotY, float& rotZ)
{
	bool keydown;

	m_position->SetFrameTime(frameTime);

	//Handle the input
	keydown = IsLeftPressed();
	m_position->MoveLeft(keydown);

	keydown = IsRightPressed();
	m_position->MoveRight(keydown);

	keydown = IsUpPressed();
	m_position->MoveForward(keydown);

	keydown = IsDownPressed();
	m_position->MoveBackward(keydown);

	keydown = TurnUp();
	m_position->TurnUpward(keydown);

	keydown = TurnDown();
	m_position->TurnDownward(keydown);

	keydown = TurnRight();
	m_position->TurnRight(keydown);

	keydown = TurnLeft();
	m_position->TurnLeft(keydown);

	//Get the view point position/rotation
	m_position->GetPosition(posX, posY, posZ);
	m_position->GetRotation(rotX, rotY, rotZ);

	//Update the mouse position
	m_lastMouseState = m_mouseState;
}

bool InputClass::IsDownPressed()
{
	if (m_keyboardState[DIK_S] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsUpPressed()
{
	if (m_keyboardState[DIK_W] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::TurnUp()
{
	if (m_keyboardState[DIK_UP] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::TurnDown()
{
	if (m_keyboardState[DIK_DOWN] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::TurnRight()
{
	if (m_keyboardState[DIK_RIGHT] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::TurnLeft()
{
	if (m_keyboardState[DIK_LEFT] & 0x80)
	{
		return true;
	}

	return false;
}