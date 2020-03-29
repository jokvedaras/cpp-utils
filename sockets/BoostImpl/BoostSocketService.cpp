#include "BoostSocketService.h"

#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_unique.hpp>

#include "BoostTcpServer.h"
#include "BoostTcpConnection.h"
#include "BoostUdpConnection.h"

BoostSocketService::BoostSocketService()
   : io_service( new boost::asio::io_service ),
     socket_service_thread( NULL )
{
   socket_service_thread = new boost::thread( &BoostSocketService::run, this );
}

BoostSocketService::~BoostSocketService()
{
   io_service->stop();

   socket_service_thread->join();
   delete socket_service_thread;
   delete io_service;
}

std::unique_ptr<TcpServer> BoostSocketService::startTcpServer( const short unsigned int port )
{
   std::unique_ptr<TcpServer> conn = boost::make_unique<BoostTcpServer>( *io_service, port );

   return conn;
}

std::unique_ptr<SocketConnection> BoostSocketService::startTcpClient( const std::string& ip,
      const short unsigned int port )
{
   // Attempt to resolve IP address or hostname
   boost::asio::ip::tcp::resolver::query query( ip, boost::lexical_cast<std::string>( port ) );
   boost::asio::ip::tcp::resolver resolver( *io_service );
   boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve( query );

   std::unique_ptr<SocketConnection> conn = boost::make_unique<BoostTcpConnection>( *io_service, iter );

   return conn;
}

std::unique_ptr<SocketConnection> BoostSocketService::startUdpConnection( const short unsigned int r_port )
{
   std::unique_ptr<SocketConnection> conn = boost::make_unique<BoostUdpConnection>( *io_service, r_port );

   return conn;
}

std::unique_ptr<SocketConnection> BoostSocketService::startUdpConnection( const std::string& s_ip,
      const short unsigned int s_port )
{
   std::unique_ptr<SocketConnection> conn = boost::make_unique<BoostUdpConnection>( *io_service, s_ip, s_port );

   return conn;
}

std::unique_ptr<SocketConnection> BoostSocketService::startUdpConnection( const std::string& s_ip,
      const short unsigned int s_port,
      const short unsigned int r_port )
{
   std::unique_ptr<SocketConnection> conn = boost::make_unique<BoostUdpConnection>( *io_service, s_ip, s_port,
         r_port );

   return conn;
}

std::unique_ptr<SocketConnection> BoostSocketService::startUdpMulticastConnection(
   const std::string& multicast_address,
   const short unsigned int port )
{
   std::unique_ptr<SocketConnection> connection = boost::make_unique<BoostUdpConnection>( *io_service,
         multicast_address, port, true );

   return connection;
}

void BoostSocketService::run()
{
   std::cerr << __PRETTY_FUNCTION__ << " starting io service" << std::endl;

   // the purpose of work is to not allow io_service.run() call to return
   // until we call stop
   boost::asio::io_service::work work( *io_service );
   io_service->run();

   std::cerr << __PRETTY_FUNCTION__ << " stopping io service" << std::endl;
}
