#pragma once

namespace Fusion {

	class Timestep 
	{
	public:
		Timestep(float p_time = 0.0f) : _time(p_time)
		{

		}
		operator float() const { return _time; }

		float GetSeconds() const { return _time; }
		float GetMilliseconds() const { return _time * 1000.0f; }

	private:
		float _time;
	};
}