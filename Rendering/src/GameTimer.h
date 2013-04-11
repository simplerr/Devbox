#pragma once

//! Graphics Library namespace.
namespace GLib
{
	//!
	//	Calculates delta time between frames
	//	and the total run time of the application.
	//!
	class GameTimer
	{
	public:
		GameTimer();

		float TotalTime() const;  
		float DeltaTime() const; 

		void Reset(); 
		void Start(); 
		void Stop();  
		void Tick();  

	private:
		double mSecondsPerCount;
		double mDeltaTime;

		__int64 mBaseTime;
		__int64 mPausedTime;
		__int64 mStopTime;
		__int64 mPrevTime;
		__int64 mCurrTime;

		bool mStopped;

	};	// Class
}