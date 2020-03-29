#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include "SocketConnection.h"

class TcpServer
{
   public:
      virtual ~TcpServer() {}

      ///Checks if a new client/connection is available
      virtual bool isConnectionAvailable() = 0;

      ///Returns a new client connection
      virtual std::unique_ptr<SocketConnection> getConnection() = 0;
};

#endif

