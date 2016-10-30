#pragma once

#include <math.h>

class PositionClass
{
public:
	PositionClass();
	PositionClass(const PositionClass&);
	~PositionClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	void GetPosition(float&, float&, float&);
	void GetRotation(float&, float&, float&);

	void SetFrameTime(float);

	void MoveForward(bool);
	void MoveBackward(bool);
	void MoveRight(bool);
	void MoveLeft(bool);
	void TurnLeft(bool);
	void TurnRight(bool);
	void TurnUpward(bool);
	void TurnDownward(bool);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	float m_frameTime;

	float m_forwardSpeed, m_backwardSpeed;
	float m_rightSpeed, m_leftSpeed;
	float m_leftTurnSpeed, m_rightTurnSpeed;
	float m_lookUpSpeed, m_lookDownSpeed;
};