#include <winsock2.h>
#include <memory>
#include "ClientSocket.h"
#include <vector>
class ServerSocket
{
public:
	ServerSocket(int _port);
	~ServerSocket();
	std::shared_ptr<ClientSocket> accept();
	void handleConnections();
private:
	SOCKET m_socket;
	ServerSocket(const ServerSocket& _copy);
	ServerSocket& operator=(const ServerSocket& _assign);
	std::vector<std::shared_ptr<ClientSocket>> clients;
};