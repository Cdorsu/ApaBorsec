#pragma once

#include <Windows.h>

class CHRTimer
{
	LARGE_INTEGER m_StartTime;
	LARGE_INTEGER m_LastFrame;
	float m_Frequency;
	float m_FrameTime;
	int m_CurrentFrames;
	int m_FramesPerSecond;
public:

	CHRTimer()
	{
		ZeroMemory( this, sizeof( CHRTimer ) );
	}

	inline float GetTime()
	{
		LARGE_INTEGER CurrentTime;
		QueryPerformanceCounter( &CurrentTime );
		return (CurrentTime.QuadPart - m_StartTime.QuadPart) / m_Frequency;
	}

	inline void StartTimer()
	{
		m_FramesPerSecond = m_CurrentFrames;
		m_CurrentFrames = 0;
		LARGE_INTEGER FrequencyCount;
		QueryPerformanceFrequency( &FrequencyCount );
		m_Frequency = (float)(FrequencyCount.QuadPart);
		QueryPerformanceCounter( &FrequencyCount );
		m_StartTime.QuadPart = FrequencyCount.QuadPart;
	}

	inline void Frame()
	{
		LARGE_INTEGER Count;
		QueryPerformanceCounter( &Count );
		m_FrameTime = float( Count.QuadPart - m_LastFrame.QuadPart ) / m_Frequency;
		m_LastFrame.QuadPart = Count.QuadPart;
		m_CurrentFrames++;
	}

	inline int GetFPS()
	{
		return m_FramesPerSecond;
	}

	inline float GetFrameTime()
	{
		if (m_FrameTime > 0.30f)
			m_FrameTime = 0.30f;
		return m_FrameTime;
	}

	~CHRTimer()
	{
	}
};

