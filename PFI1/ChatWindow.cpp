#include "ChatWindow.h"
#include "ChangeUsername.h"
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Button.H>
#include <string>
#include <iostream>
#include "pugixml.hpp"
#include <FL/Fl_ask.H>
#include <FL/Fl_Color_Chooser.H>

ChatWindow::ChatWindow(ClientSocket* clientSock, std::string userName, Fl_Window* mainWin) 
	: Fl_Window(640, 480, "Chat Window"), userName(userName), isClient(true), clientSocket(nullptr), mw(mainWin), mainCol(0,0,w(),h()), subCol(0, 0, w() / 3, h()) 
{
	
	double r, g, b;
	r = 211;
	g = 211;
	b = 211;
	pugi::xml_document doc;
	doc.load_file("Theme.xml");
	pugi::xml_parse_result res = doc.load_file("Theme.xml");
	if (res.status)
	{
		//error handling 
		printf("could not open doc");
		
	}
	else {
		printf("opened doc");
		pugi::xml_node root = doc.child("Themes");
		pugi::xml_node colors = root.child("Colors");
		r = std::stof(colors.attribute("r").value())*255;
		g = std::stof(colors.attribute("g").value())*255;
		b = std::stof(colors.attribute("b").value())*255;
	}
	
	
	
	//initial constructor
	//this one actually looks okay imo
	mainCol.type(Fl_Flex::ROW);
	mainCol.window()->color(fl_rgb_color(r, g, b));
	clientSocket = clientSock;

	buff = new Fl_Text_Buffer();
	chatBox = new Fl_Text_Display(0, 0, w() * 2 / 3, h());
	chatBox->box(FL_UP_BOX);
	chatBox->color(fl_rgb_color(r,g,b));
	chatBox->buffer(buff);

	Fl_Box* spacer = new Fl_Box(0, 0, 10, h());
	Fl_Box* spacer2 = new Fl_Box(0, 0, w(), h());
	Fl_Box* spacer3 = new Fl_Box(0, 0, w(), h());
	Fl_Box* spacer4 = new Fl_Box(0, 0, w(), h());
	spacer->box(FL_NO_BOX);
	spacer2->box(FL_NO_BOX);
	spacer3->box(FL_NO_BOX);
	spacer4->box(FL_NO_BOX);
	
	subCol.type(Fl_Flex::COLUMN);

	Fl_Button* usnButton = new Fl_Button(0, 0, w() / 3, h() / 3);
	usnButton->label("Change Username");
	Fl_Button* quitButton = new Fl_Button(0, 0, w() / 3, h() / 3);
	quitButton->label("Menu");
	Fl_Button* themeButton = new Fl_Button(0, 0, w() / 3, h() / 3);
	themeButton->label("Change Theme");

	Fl_Multiline_Input* xd = new Fl_Multiline_Input(0, 0, w() / 3, h() * 2 / 8);

	xd->box(FL_UP_BOX);
	xd->color(fl_rgb_color(r, g, b));
	subCol.add_resizable(*themeButton);
	subCol.add_resizable(*spacer4);
	subCol.add_resizable(*usnButton);
	subCol.add_resizable(*spacer3);
	subCol.add_resizable(*quitButton);
	subCol.add_resizable(*chatBox);
	subCol.add_resizable(*spacer2);
	subCol.add(*xd);
	mainCol.add_resizable(*chatBox);
	mainCol.add_resizable(*spacer);
	mainCol.add_resizable(subCol);
	mainCol.fixed(subCol, w() * 1 / 3);
	subCol.fixed(spacer4, 10);
	subCol.fixed(spacer3, 10);
	subCol.fixed(themeButton, 50);
	subCol.fixed(quitButton, 50);
	subCol.fixed(usnButton, 50);
	subCol.fixed(xd, h() * 2 / 8);
	mainCol.fixed(spacer, 10);
	mainCol.margin(10);
	chatBox->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
	themeButton->color(fl_rgb_color(r, g, b));
	quitButton->color(fl_rgb_color(r, g, b));
	usnButton->color(fl_rgb_color(r, g, b));
	xd->wrap(1);
	xd->when(FL_WHEN_CHANGED);
	xd->callback(displayToChatBox, this);
	usnButton->callback(changeUsername, this);
	quitButton->callback(closeWindow, this);
	themeButton->callback(changeTheme, this);
	//send join message
	sendMessage(("<" + userName + "> has joined the chatroom\n"));
	this->resizable(mainCol);
	redraw();
	end();

}

ChatWindow::~ChatWindow() 
{
	
}

//custom struct to send extra data using callbacks
struct ThemeData 
{
	Fl_Color_Chooser* colorChooser;
	ChatWindow* chatWindow;
};

//theme changing. this is kinda cool
void ChatWindow::changeTheme(Fl_Widget* w, void* data)
{
	//create a new window
	Fl_Window* chsW = new Fl_Window(200, 160);
	chsW->label("Change Theme");
	//all the necessary stuff
	Fl_Flex* flx = new Fl_Flex(0, 0, chsW->w(), chsW->h());
	flx->type(Fl_Flex::COLUMN);
	Fl_Box* spacer = new Fl_Box(0, 0, 0, 0);
	spacer->box(FL_NO_BOX);
	chsW->add(*flx);
	//fltks inbuilt colour chooser
	Fl_Color_Chooser* chs = new Fl_Color_Chooser(0, 0, chsW->w(), chsW->h());
	flx->fixed(flx, chsW->w());
	flx->add_resizable(*chs);
	flx->fixed(chs, 95);
	//add buttons to it
	Fl_Button* b = new Fl_Button(0, 0, 20, 10);
	Fl_Button* q = new Fl_Button(0, 0, 20, 10);
	b->label("Confirm");
	q->label("Close");
	
	flx->add_resizable(*b);
	flx->add_resizable(*spacer);
	flx->fixed(spacer, 5);
	flx->add_resizable(*q);
	flx->fixed(b, 30);
	flx->fixed(q, 30);
	chsW->show();
	
	//include the colour chooser widget, and chatwindow itself as data
	ThemeData* thmd = new ThemeData{ chs,(ChatWindow*)data};
	//confirm buttom callback using custom struct as data parameter
	b->callback(confirmTheme, thmd);
	//close button callback
	q->callback(closeTheme, chsW);
}

//confirm button callback
void ChatWindow::confirmTheme(Fl_Widget* w, void* data)
{
	//unpackage the data
	ThemeData* thmd = (ThemeData*)data;
	//get reference to the chatwindow
	ChatWindow* chw = thmd->chatWindow;
	//get reference to the colour chooser
	Fl_Color_Chooser* chs = thmd->colorChooser;
	//initialise rgb values
	float r, g, b;
	r = chs->r();
	g = chs->g();
	b = chs->b();
	//set the chat window to the rgb values
	chw->color(fl_rgb_color(r * 255, g * 255, b * 255));
	//set the chat box to the rgb values
	chw->chatBox->color(fl_rgb_color(r * 255, g * 255, b * 255));
	//adds theme to xml file to be remembered
	pugi::xml_document doc;
	pugi::xml_node root = doc.append_child("Themes");
	pugi::xml_node colors = root.append_child("Colors");
	colors.append_attribute("r").set_value(r);
	colors.append_attribute("g").set_value(g);
	colors.append_attribute("b").set_value(b);
	doc.save_file("Theme.xml");
	Fl_Widget* child;	
	//iterate through all of the windows children
	for (int i = 0; i < chw->children(); i++) 
	{
		child = chw->subCol.child(i);
		//set them to the rgb values
		child->color(fl_rgb_color(r * 255, g * 255, b * 255));
		child->redraw();
		//iterate through all of the subColumns children
		for (int j = 0; j < chw->subCol.children(); j++) 
		{
			child = chw->subCol.child(j);
			//set them to the rgb values
			child->color(fl_rgb_color(r * 255, g * 255, b * 255));
			child->redraw();
		}	
		//redraw everything
		chw->redraw();		
	}
}
//close the theme window using button callback
void ChatWindow::closeTheme(Fl_Widget* w, void* data) 
{
	Fl_Window* win = (Fl_Window*)data;
	win->hide();
}
//open a change username window using button callback
void ChatWindow::changeUsername(Fl_Widget* w, void* data) 
{
	ChangeUsername* cUsn = new ChangeUsername();
	cUsn->show();
}

//close the chat window using button callback
void ChatWindow::closeWindow(Fl_Widget* w, void* data) 
{
	ChatWindow* chw = (ChatWindow*)data;
	//use main window reference to reopen the main menu
	chw->mw->show();
	chw->~ChatWindow();
}

//handle message inputs from user
void ChatWindow::displayToChatBox(Fl_Widget* w, void* data) 
{
	//get the window, and the multiline input box
	ChatWindow* chatWin = (ChatWindow*)data;
	Fl_Multiline_Input* mli = (Fl_Multiline_Input*)w;
	//set the inputText var to whatever is in the multiline input box
	const char* inputText = mli->value();
	//open the ole docs again
	pugi::xml_document doc;
	doc.load_file("Usernames.xml");
	pugi::xml_parse_result res = doc.load_file("Usernames.xml");
	if (res.status) 
	{
		//error handling 
		printf("could not open doc");
	}
	//navigate the doc
	pugi::xml_node root = doc.child("Users");
	pugi::xml_node uN = root.child("Username");
	//check if the users current name is the same as in the doc
	if (chatWin->userName != uN.attribute("Name").value()) 
	{
		//if it isnt, they must have changed it since last typing. let everybody know!
		std::string msg = ("||| <" + chatWin->userName + "> changed their username to <" + uN.attribute("Name").value() + "> |||\n");
		chatWin->sendMessage(msg.c_str());
	}
	//set the users name to be the name in the doc
	chatWin->userName = uN.attribute("Name").value();

	//if the user hits enter
	if (Fl::event_key() == FL_KP_Enter || Fl::event_key() == FL_Enter) 
	{
		//check the message doesnt start with an enter key to prevent whitespace spam
		if (mli->index(0) != 10) {
			//append the username to whatever is in the mli
			std::string appendUN = chatWin->userName + ": ";
			//clear the mli and refocus on it
			mli->value("");
			mli->take_focus();
			//scroll to the end of the output box
			chatWin->chatBox->insert_position(chatWin->buff->length());
			chatWin->chatBox->show_insert_position();	
			//send the appended username + message to be handled
			chatWin->sendMessage(chatWin->userName + ": " +inputText);
		}
		else
		{
			//if first character is enter-whitespace, just delete it lol
			mli->value("");
		}	
	}
}

//send message to clientsocket handler
void ChatWindow::sendMessage(std::string message) 
{
	//off you go!
	clientSocket->send(message);
}

//received message handler, usually called by serverSocket
void ChatWindow::receiveMessage(std::string message) 
{
	//check buffer exists
	if (buff)
	{
		//append the received message to the buffer
		buff->append((message).c_str());
		//scroll to the end of the output box
		chatBox->insert_position(buff->length());
		chatBox->show_insert_position();
		//check if the message is announcing server shutdown. can only ever be sent by serverSocket as any messages sent by users will automatically have their username + : appended to their message
		//would be funny if users could crash the server sql injection style though
		if (message == "SERVER_SHUTDOWN")
		{
			//little alert popup. thank god fltk has these native and i dont have to make it myself
			fl_alert("Host disconnected, returning to menu");
			//handle disconnect 
			hostDisconnect();
		}
	}
}

//handle disconnect
void ChatWindow::hostDisconnect() 
{
	//show the main window
	this->mw->show();
	//hide the chat window
	this->hide();
}

