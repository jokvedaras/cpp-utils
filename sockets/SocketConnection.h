#ifndef SOCKETCONNECTION_H_
#define SOCKETCONNECTION_H_

#include <string>
#include <memory>
#include <vector>

class SocketConnection
{
   public:
      virtual ~SocketConnection() {}

      // read only a specified number of bytes (TCP only)
      virtual std::string read( size_t bytes ) = 0;

      //return size of data available on socket (TCP only)
      virtual size_t dataSize() = 0;

      // read the entire buffer/datagram
      virtual std::string read( ) = 0;

      // write a string to the socker
      virtual void write( const std::string &byte_string ) = 0;

      // check if the socket is connected
      virtual bool isConnected() = 0;

      // check if data is available
      virtual bool isDataAvailable() = 0;
};

#endif /* SOCKETCONNECTION_H_ */
