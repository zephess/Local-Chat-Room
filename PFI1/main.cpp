#include <FL/Fl.H>
#include "MainWindow.h"
#include "ChatWindow.h"

int main(int argc, char* argv[])
{
	MainWindow win;
	Fl::scheme("fltk+");
	win.color(FL_WHITE);
	win.show();
	return Fl::run();
}

