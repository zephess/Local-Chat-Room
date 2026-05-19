#pragma once

#include "Timer.h"
#include "ClientSocket.h"
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Text_Display.H>
#include <string>

class ChatWindow : public Fl_Window 
{
public:
	ChatWindow(ClientSocket* clienSock, std::string userName, Fl_Window* mainWin);
	~ChatWindow();
	static void displayToChatBox(Fl_Widget* w, void* data);
	void receiveMessage(std::string message);
	void sendMessage(std::string message);
	ClientSocket* clientSocket;
	static void changeUsername(Fl_Widget* w, void* data);
	static void closeWindow(Fl_Widget* w, void* data);
	static void changeTheme(Fl_Widget* w, void* data);
	static void confirmTheme(Fl_Widget* w, void* data);
	static void closeTheme(Fl_Widget* w, void* data);
	void hostDisconnect();
	
private:
	Fl_Text_Buffer* buff;
	Fl_Text_Display* chatBox;
	std::string userName;
	Fl_Window* mw;
	Fl_Flex mainCol, subCol;
	bool isClient;
	
};