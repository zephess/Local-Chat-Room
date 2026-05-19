#pragma once
#include <FL/Fl_Window.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Text_Display.H>
#include <string>
class ChangeUsername : public Fl_Window {
public:
	ChangeUsername();
	~ChangeUsername();
	static void getUsername(Fl_Widget* w, void* data);
private:
	Fl_Text_Buffer* buff;

};