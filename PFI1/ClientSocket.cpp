#include "ClientSocket.h"
#include "ServerSocket.h"
#include "ChatWindow.h"
#include <memory>
#include <stdexcept>
#include <iostream>
#include <string>
#include <WS2tcpip.h>
ClientSocket::ClientSocket()
	: m_socket(INVALID_SOCKET), m_closed(false), chatWindow(nullptr)
{ }

//client socket initialisation technical mumbo jumbo
ClientSocket::ClientSocket(const char* serverAddr, int port)
	: isConnected(false), chatWindow(nullptr), m_socket(INVALID_SOCKET), m_closed(false)
{
	WSADATA wsaData;
	//initialise winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		throw std::runtime_error("WSAStartup failed");
	}

	//create a socket
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		WSACleanup();
		throw std::runtime_error("Failed to create socket");
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);

	//convert IP address using inet_pton
	if (inet_pton(AF_INET, serverAddr, &serverAddress.sin_addr) <= 0)
	{
		closesocket(m_socket);
		WSACleanup();
		throw std::runtime_error("Invalid address / Address not supported");
	}

	//connect socket to ip address
	if (::connect(m_socket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
	{
		closesocket(m_socket);
		WSACleanup();
		throw std::runtime_error("Failed to connect to server");
	}

	u_long mode = 1;
	//ignore blocking errors
	if (ioctlsocket(m_socket, FIONBIO, &mode) == SOCKET_ERROR)
	{
		throw std::runtime_error("Failed to set non-blocking");
	}

	//yippee!
	printf("Connected to server at %s:%d\n", serverAddr, port);
}
ClientSocket::~ClientSocket()
{
	//close the socket when destroyed
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
	}
}

//gonna be honest, i dont know why this is here. but when i delete it the program wont compile. idk
void ClientSocket::connect() {

}

//receiving logic
bool ClientSocket::receive(std::string& _message)
{
	//create a buffer to store a message
	char buffer[128] = { 0 };
	int bytes = ::recv(m_socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			//catch any errors other than blocking errors
			throw std::runtime_error("Read failed");
		}

		return false;
	}
	else if (bytes == 0)
	{
		m_closed = true;
		return false;
	}

	_message = buffer;
	return true;
}

//sending logic
void ClientSocket::send(const std::string& _message)
{
	int bytes = ::send(m_socket, _message.c_str(), _message.length(), 0);
	if (bytes <= 0)
	{
		//catch errors
		throw std::runtime_error("Failed to send data");
	}

}

bool ClientSocket::closed()
{
	return m_closed;
}

