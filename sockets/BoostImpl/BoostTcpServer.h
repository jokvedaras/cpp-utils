#ifndef BOOST_TCP_SERVER_H_
#define BOOST_TCP_SERVER_H_

#include <iostream>
#include <map>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "BoostAsioUtils.h"
#include "SocketConnection.h"
#include "TcpServer.h"
#include "BoostTcpConnection.h"

class BoostTcpServer : public TcpServer
{
      // Create a TCP server connection
   public:
      BoostTcpServer( boost::asio::io_service& io_service,
                      short unsigned int port )
         : socket( io_service ),
           acceptor( io_service, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), port ) ),
           endpoint_iterator(),
           new_client_available( false )
      {
         acceptClientConnections();
      }

      virtual ~BoostTcpServer();

      virtual bool isConnectionAvailable()
      {
         return socket.is_open() && new_client_available;
      }

      virtual std::unique_ptr<SocketConnection> getConnection()
      {
         std::unique_ptr<SocketConnection> return_val( new BoostTcpConnection( socket.get_io_service(), socket ) );
         new_client_available = false;
         acceptClientConnections();

         return std::move( return_val );
      }

   private:

      void acceptClientConnections()
      {
         // set up async event to accept connections
         //
         acceptor.async_accept( socket,
                                boost::bind( &BoostTcpServer::handleClientConnection, this,
                                             boost::asio::placeholders::error ) );

      }

      void handleClientConnection( const boost::system::error_code& error )
      {
         if( !error && new_client_available == false )
         {
            std::cout << __PRETTY_FUNCTION__ << " connected to a client" << std::endl;
            new_client_available = true;
         }
         else if( error )
         {
            std::cerr << __PRETTY_FUNCTION__ << " " << error << std::endl;
            std::cerr << error.message() << std::endl;

            // Filtering out error #995 "The I/O operation has been aborted because
            // of either a thread exit or an application request".  This error is returned during
            // deconstruction of this class since the async_accept is stopped
            // Filtering out error #125 "Operation Aborted". No need to throw an exception
            if( error.value() != 995 && error.value() != 125 )
            {
               throw std::runtime_error( "Could not accept client connection!" );
            }
         }
      }

      boost::asio::ip::tcp::socket socket;
      boost::asio::ip::tcp::acceptor acceptor;
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator;

      bool new_client_available;
};

#endif
