#pragma once
#include <winsock2.h>
#include<string>
#include <FL/Fl_Widget.H>
//#include "ChatWindow.h"
class ServerSocket;
class ChatWindow;
class ClientSocket
{
public:
	ClientSocket();
	ClientSocket(const char* serverAddress, int port);
	~ClientSocket();
	void connect();
	bool receive(std::string& _message);
	void send(const std::string& _message);
	bool closed();
	void setChatWin(ChatWindow* chatWin);
private:
	friend class ServerSocket;
	SOCKET m_socket;
	bool isConnected;
	bool m_closed;
	ClientSocket(const ClientSocket& _copy);
	ClientSocket& operator=(const ClientSocket& _assign);
	ChatWindow* chatWindow;
};