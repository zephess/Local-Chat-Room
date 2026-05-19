#include "Timer.h"
#include "MainWindow.h"

Timer::Timer(double _duration)
{
	m_duration = _duration;
	m_userData = (void*)0;
	Fl::add_timeout(1.0, tick, this);
}

Timer::~Timer() 
{
	Fl::remove_timeout(tick, this);
}

void Timer::on_tick(void* userData) 
{
	printf("Tick!\n");
	Fl::repeat_timeout(m_duration, tick, userData);
}

void Timer::tick(void* _userData) 
{
	Timer* timer = (Timer*)_userData;
	timer->on_tick(_userData);
}



