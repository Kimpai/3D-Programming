#include "positionclass.h"

PositionClass::PositionClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_frameTime = 0.0f;

	m_forwardSpeed = 0.0f;
	m_backwardSpeed = 0.0f;
	m_rightSpeed = 0.0f;
	m_leftSpeed = 0.0f;
	m_upwardSpeed = 0.0f;
	m_downwardSpeed = 0.0f;
	m_leftTurnSpeed = 0.0f;
	m_rightTurnSpeed = 0.0f;
	m_lookUpSpeed = 0.0f;
	m_lookDownSpeed = 0.0f;
}

PositionClass::PositionClass(const PositionClass& other)
{

}

PositionClass::~PositionClass()
{

}

void PositionClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}

void PositionClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

void PositionClass::GetPosition(float& x, float& y, float& z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
}

void PositionClass::GetRotation(float& x, float& y, float& z)
{
	x = m_rotationX;
	y = m_rotationY;
	z = m_rotationZ;
}

void PositionClass::SetFrameTime(float frameTime)
{
	m_frameTime = frameTime;
}

void PositionClass::MoveForward(bool keydown)
{
	float radians;

	//Update the forward speed movement based on the frame time and whether the user is holding the key down or not
	if (keydown)
	{
		m_forwardSpeed += (m_frameTime * 0.15f);
		if (m_forwardSpeed > (m_frameTime * 0.15f))
		{
			m_forwardSpeed = (m_frameTime * 0.15f);
		}
	}
	else
	{
		m_forwardSpeed -= m_frameTime * 0.5f;

		if (m_forwardSpeed < 0.0f)
		{
			m_forwardSpeed = 0.0f;
		}
	}

	//Convert degrees to radians
	radians = m_rotationY * 0.0174532925f;

	//Update the position
	m_positionZ += cosf(radians) * m_forwardSpeed;
	m_positionX += sinf(radians) * m_forwardSpeed;
}

void PositionClass::MoveBackward(bool keydown)
{
	float radians;

	if (keydown)
	{
		m_backwardSpeed += (m_frameTime * 0.15f);

		if (m_backwardSpeed > (m_frameTime * 0.15f))
		{
			m_backwardSpeed = (m_frameTime * 0.15f);
		}
	}
	else
	{
		m_backwardSpeed -= m_frameTime * 0.5f;

		if (m_backwardSpeed < 0.0f)
		{
			m_backwardSpeed = 0.0f;
		}
	}

	radians = m_rotationY * 0.0174532925f;

	m_positionZ -= cosf(radians) * m_backwardSpeed;
	m_positionX -= sinf(radians) * m_backwardSpeed;
}

void PositionClass::MoveUpward(bool keydown)
{
	if (keydown)
	{
		m_upwardSpeed += (m_frameTime * 0.15f);

		if (m_upwardSpeed > (m_frameTime * 0.15f))
		{
			m_upwardSpeed = m_frameTime + 0.15f;
		}
	}
	else
	{
		m_upwardSpeed -= m_frameTime * 0.5f;

		if (m_upwardSpeed < 0.0f)
		{
			m_upwardSpeed = 0.0f;
		}
	}

	m_positionY += m_upwardSpeed;
}

void PositionClass::MoveDownward(bool keydown)
{
	if (keydown)
	{
		m_downwardSpeed += (m_frameTime * 0.15f);

		if (m_downwardSpeed > (m_frameTime * 0.15f))
		{
			m_upwardSpeed = m_frameTime + 0.15f;
		}
	}
	else
	{
		m_upwardSpeed -= m_frameTime * 0.5f;

		if (m_downwardSpeed < 0.0f)
		{
			m_downwardSpeed = 0.0f;
		}
	}

	m_positionY -= m_downwardSpeed;
}

void PositionClass::StrafeRight(bool keydown)
{
	float radians;

	//Update the forward speed movement based on the frame time and whether the user is holding the key down or not
	if (keydown)
	{
		m_rightSpeed += (m_frameTime * 0.15f);
		if (m_rightSpeed > (m_frameTime * 0.15f))
		{
			m_rightSpeed = (m_frameTime * 0.15f);
		}
	}
	else
	{
		m_rightSpeed -= m_frameTime * 0.5f;

		if (m_rightSpeed < 0.0f)
		{
			m_rightSpeed = 0.0f;
		}
	}

	//Convert degrees to radians
	radians = m_rotationY * 0.0174532925f;

	//Update the position
	m_positionX += sinf(radians) * m_rightSpeed;
}

void PositionClass::StrafeLeft(bool keydown)
{
	float radians;

	if (keydown)
	{
		m_leftSpeed += (m_frameTime * 0.15f);

		if (m_leftSpeed > (m_frameTime * 0.15f))
		{
			m_leftSpeed = (m_frameTime * 0.15f);
		}
	}
	else
	{
		m_leftSpeed -= m_frameTime * 0.5f;

		if (m_leftSpeed < 0.0f)
		{
			m_leftSpeed = 0.0f;
		}
	}

	radians = m_rotationY * 0.0174532925f;

	m_positionX -= sinf(radians) * m_leftSpeed;
}

void PositionClass::TurnLeft(bool keydown)
{
	if (keydown)
	{
		m_leftTurnSpeed += m_frameTime * 0.1f;

		if (m_leftTurnSpeed > (m_frameTime * 0.1f))
		{
			m_leftTurnSpeed = m_frameTime * 0.1f;
		}
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime * 3.5f;

		if (m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
	}

	m_rotationY -= m_leftTurnSpeed;

	if (m_rotationY < 0.0f)
	{
		m_rotationY += 360.0f;
	}
}

void PositionClass::TurnRight(bool keydown)
{
	if (keydown)
	{
		m_rightTurnSpeed += m_frameTime * 0.1f;

		if (m_rightTurnSpeed > (m_frameTime * 0.1f))
		{
			m_rightTurnSpeed = m_frameTime * 0.1f;
		}
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime * 3.5f;

		if (m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
	}

	m_rotationY += m_rightTurnSpeed;

	if (m_rotationY > 360.0f)
	{
		m_rotationY -= 360.0f;
	}
}

void PositionClass::TurnUpward(bool keydown)
{
	if (keydown)
	{
		m_lookUpSpeed += m_frameTime * 0.1f;

		if (m_lookUpSpeed > (m_frameTime * 0.1f))
		{
			m_lookUpSpeed = m_frameTime * 0.1f;
		}
	}
	else
	{
		m_lookUpSpeed -= m_frameTime * 2.0f;

		if (m_lookUpSpeed < 0.0f)
		{
			m_lookUpSpeed = 0.0f;
		}
	}

	m_rotationX -= m_lookUpSpeed;

	if (m_rotationX > 90.0f)
	{
		m_rotationX = 90.0f;
	}
}

void PositionClass::TurnDownward(bool keydown)
{
	if (keydown)
	{
		m_lookDownSpeed += m_frameTime * 0.1f;

		if (m_lookDownSpeed > (m_frameTime * 0.1f))
		{
			m_lookDownSpeed = m_frameTime * 0.1f;
		}
	}
	else
	{
		m_lookDownSpeed -= m_frameTime * 2.0f;

		if (m_lookDownSpeed < 0.0f)
		{
			m_lookDownSpeed = 0.0f;
		}
	}

	m_rotationX += m_lookDownSpeed;

	if (m_rotationX < -90.0f)
	{
		m_rotationX = -90.0f;
	}
}
