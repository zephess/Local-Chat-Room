#pragma once
#include "Timer.h"
#include "ChatWindow.h"
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Generic_Image.H>
#include <FL/fl_draw.H>
#include <vector>
#include <memory>
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "Wsa.h"

class MainWindow : public Fl_Window, Timer
{
public:
	MainWindow();
	~MainWindow();
	static void closeWindow(Fl_Widget* w, void* data);
	static void openChat(Fl_Widget* w, void* data);
	static void changeUsername(Fl_Widget* w, void* data);
	static void startHost(Fl_Widget* w, void* data);
	static void handleServer(void* data);
	void isServerActive(const std::string& ip, int port);
	void show() override;
private:
	void on_tick(void* userData);
	Fl_Flex mainCol;
	Fl_Box testBox, spacer, spacer2, spacer3;
	Fl_Button joinButton, otherButton, changeUsnButton, hostButton;
	Fl_Flex subCol;
	bool isClient;
	ServerSocket* server;
	ClientSocket* client;
	std::vector<std::shared_ptr<ClientSocket>> clients;
	bool serverInitialised;
	Wsa wsa;
	ChatWindow* chw;
	ClientSocket* clientSocket;
	bool receivedMessage;
};




