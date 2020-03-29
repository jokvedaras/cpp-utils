#ifndef BOOST_SOCKET_SERVICE_H_
#define BOOST_SOCKET_SERVICE_H_

#include <memory>

namespace boost
{
   class thread;
   namespace asio
   {
      class io_service;
   }
}

#include "SocketService.h"
#include "TcpServer.h"

// create a thread to control boost async socket programming
// NOTE: When using any async command, be sure the objects that are passed in
//       never get deallocated!
class BoostSocketService : public SocketService
{
   public:
      BoostSocketService();

      virtual ~BoostSocketService();

      virtual std::unique_ptr<TcpServer> startTcpServer( const short unsigned int port );

      virtual std::unique_ptr<SocketConnection> startTcpClient( const std::string& ip, const short unsigned int port );

      virtual std::unique_ptr<SocketConnection> startUdpConnection( const short unsigned int r_port );

      virtual std::unique_ptr<SocketConnection> startUdpConnection( const std::string& s_ip,
            const short unsigned int s_port );

      virtual std::unique_ptr<SocketConnection> startUdpConnection( const std::string& s_ip,
            const short unsigned int s_port,
            const short unsigned int r_port );

      virtual std::unique_ptr<SocketConnection> startUdpMulticastConnection( const std::string& multicast_address,
            const short unsigned int port );

   private:
      BoostSocketService( const BoostSocketService& );
      BoostSocketService& operator=( const BoostSocketService& );

      void run();

      boost::asio::io_service *io_service;
      boost::thread           *socket_service_thread;
};

#endif /* BOOST_SOCKET_SERVICE_H_ */
