#include "ChangeUsername.h"
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Text_Display.H>
#include <iostream>
#include "pugixml.hpp"
ChangeUsername::ChangeUsername() : Fl_Window(400, 200, "Change Username") 
{
	//formatting n stuff
	Fl_Flex* mainCol = new Fl_Flex(0,0,w(), h());
	Fl_Box* box = new Fl_Box(0, 0, w(), h());
	buff = new Fl_Text_Buffer();
	box->box(FL_DOWN_BOX);
	box->color(FL_GRAY);

	Fl_Input* input = new Fl_Input(w()/6, h()/3 * 1.5, w() / 6 * 4, h()/3);
	//callback for the input box
	input->when(FL_WHEN_ENTER_KEY_ALWAYS);
	input->callback(getUsername, this);
}
ChangeUsername::~ChangeUsername() 
{

}

void ChangeUsername::getUsername(Fl_Widget* w, void* data) 
{
	ChangeUsername* usnWin = (ChangeUsername*)data;
	Fl_Input* input = (Fl_Input*)w;
	//take the input and assign it to a var
	const char* inputText = input->value();
	//when enter key is pressed
	if (Fl::event_key() == FL_KP_Enter || Fl::event_key() == FL_Enter) 
	{
		if (input->index(0) != 10)
		{
			usnWin->buff->append(inputText);
			std::cout << inputText;
			input->value("");
			usnWin->take_focus();
			//save the username to an xml doc
			pugi::xml_document doc;
			pugi::xml_node root = doc.append_child("Users");
			pugi::xml_node uN = root.append_child("Username");
			uN.append_attribute("Name").set_value(inputText);
			doc.save_file("Usernames.xml");
			//hide the window
			usnWin->hide();
		}
	}
}