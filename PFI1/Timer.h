#pragma once

struct Timer {
	Timer(double _duration);
	virtual ~Timer();
	virtual void on_tick(void* userData);
	static void tick(void* _userdata);
private:
	void* m_userData;
	double m_duration;
	
};