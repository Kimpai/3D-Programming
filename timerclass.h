#pragma once

#include <windows.h>

class TimerClass
{
public:
	TimerClass();
	TimerClass(const TimerClass&);
	~TimerClass();

	bool Initialize();
	void Frame();

	double GetTime();

private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	double m_frameTime;
};