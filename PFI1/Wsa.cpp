#include "Wsa.h"
#include <stdexcept>
Wsa::Wsa()
{
	//winsock initialisation
	if (WSAStartup(MAKEWORD(2, 2), &m_wsadata) != 0)
	{
		throw std::runtime_error("Failed to initialize Winsock");
	}
}
Wsa::~Wsa()
{
	WSACleanup();
}