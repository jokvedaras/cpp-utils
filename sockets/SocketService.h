#ifndef SOCKETSERVICE_H_
#define SOCKETSERVICE_H_

#include <memory>

#include "SocketConnection.h"
#include "TcpServer.h"

class SocketService
{
   public:
      virtual ~SocketService() {}

      /// Create a new TCP Server on a specified port
      virtual std::unique_ptr<TcpServer> startTcpServer( const short unsigned int port ) = 0;

      /// Create a new TCP Client to a specified ip and port
      virtual std::unique_ptr<SocketConnection> startTcpClient( const std::string& ip,
            const short unsigned int port ) = 0;

      /// Create a new UDP Server on a specified port
      virtual std::unique_ptr<SocketConnection> startUdpConnection( const short unsigned int r_port ) = 0;

      /// Create a new UDP Client on a specified ip and port
      virtual std::unique_ptr<SocketConnection> startUdpConnection( const std::string& s_ip,
            const short unsigned int s_port ) = 0;

      /// Create a new UDP Client/Server for a specified ip and port
      virtual std::unique_ptr<SocketConnection> startUdpConnection( const std::string& s_ip,
            const short unsigned int s_port,
            const short unsigned int r_port ) = 0;

      /// Create a new UDP Client/Server Multicast connection for a specified multicast address and port
      virtual std::unique_ptr<SocketConnection> startUdpMulticastConnection( const std::string& multicast_address,
            const short unsigned int port ) = 0;
};

#endif /* SOCKETSERVICE_H_ */
