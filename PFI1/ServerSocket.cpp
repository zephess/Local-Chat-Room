#include "ServerSocket.h"
#include <ws2tcpip.h>
#include <stdexcept>
#include <string>
#include <iostream>
ServerSocket::ServerSocket(int _port) : m_socket(INVALID_SOCKET)
{
	//more initialisation mumbo jumbo
	addrinfo hints = { 0 };
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	addrinfo* result = NULL;
	//reserve ip address and port
	if (getaddrinfo(NULL, std::to_string(_port).c_str(), &hints, &result) != 0)
	{
		throw std::runtime_error("Failed to resolve server address or port");
	}
	//create a socket
	m_socket = socket(result->ai_family, result->ai_socktype,
		result->ai_protocol);
	if (m_socket == INVALID_SOCKET)
	{
		freeaddrinfo(result);
		throw std::runtime_error("Failed to create socket");
	}
	//bind the socket to the port
	if (bind(m_socket, result->ai_addr, result->ai_addrlen) == SOCKET_ERROR)
	{
		freeaddrinfo(result);
		throw std::runtime_error("Failed to bind socket");
	}
	freeaddrinfo(result);
	//listen on socket
	if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		throw std::runtime_error("Failed to listen on socket");
	}
	u_long mode = 1;
	//set non blocking on the socket
	if (ioctlsocket(m_socket, FIONBIO, &mode) == SOCKET_ERROR) 
	{
		throw std::runtime_error("Failed to set non-blocking");
	}
	//yippeee!
	std::cout << "Server listening on port " << _port << "\n";
}

//server socket destructor
ServerSocket::~ServerSocket()
{
	//close the socket
	closesocket(m_socket);
	//iterate through clients
	for (int i = 0; i < clients.size(); ++i)
	{
		if (clients.at(i)->closed())
		{
			clients.erase(clients.begin() + i);
			i--;
		}
		else 
		{
			std::string message;
			bool receivedMessage = clients[i]->receive(message);

			if (receivedMessage)
			{
				printf("Message received: %s\n", message.c_str());
				for (int i = 0; i < clients.size(); i++)
				{
					//send the shutdown message which is handled in chatWindow
					clients[i]->send("SERVER_SHUTDOWN");
				}
			}
		}
	}
}

std::shared_ptr<ClientSocket> ServerSocket::accept()
{
	if (m_socket == INVALID_SOCKET) 
	{
		printf("Error: Attempt to accept on an invalid socket.\n");
		return nullptr;
	}
	SOCKET socket = ::accept(m_socket, NULL, NULL);
	if (socket == INVALID_SOCKET)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK) 
		{
			throw std::runtime_error("Failed to accept socket");
		}
		return std::shared_ptr<ClientSocket>();
	}
	std::shared_ptr<ClientSocket> rtn = std::make_shared<ClientSocket>();
	rtn->m_socket = socket;
	return rtn;
}

//handle active connections
void ServerSocket::handleConnections() {
	std::shared_ptr<ClientSocket> client = accept();
	if (client)
	{
		printf("Client Connected!\n");
		clients.push_back(client);
	}
	//iterate through clients
	for (int i = 0; i < clients.size(); ++i)
	{
		if (clients.at(i)->closed())
		{
			clients.erase(clients.begin() + i);
			i--;
		}
		else {
			std::string message;
			bool receivedMessage = clients[i]->receive(message);

			if (receivedMessage)
			{
				//any messages received are sent on to every client
				printf("Message received: %s\n", message.c_str());
				for (int i = 0; i < clients.size(); i++)
				{
					clients[i]->send(message);
				}
			}
		}
	}	
}