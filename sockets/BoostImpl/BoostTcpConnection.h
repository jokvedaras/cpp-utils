#ifndef BOOST_TCP_CONNECTION_H_
#define BOOST_TCP_CONNECTION_H_

#include <array>
#include <iostream>
#include <map>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "BoostAsioUtils.h"
#include "SocketConnection.h"

class BoostTcpConnection : public SocketConnection
{
   public:
      // Create a TCP client connection
      BoostTcpConnection( boost::asio::io_service& io_service,
                          boost::asio::ip::tcp::resolver::iterator iter )
         : socket( io_service ),
           acceptor( io_service ),
           endpoint_iterator( iter ),
           connected( false )
      {
         connectToTcpServer();
      }
      // Create a TCP server connection
      BoostTcpConnection( boost::asio::io_service& io_service,
                          short unsigned int port )
         : socket( io_service ),
           acceptor( io_service, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), port ) ),
           endpoint_iterator(),
           connected( false )
      {
      }
      // Used by TcpServer.h to create additional server connection
      BoostTcpConnection( boost::asio::io_service& io_service,
                          boost::asio::ip::tcp::socket& newSocket )
         : socket( std::move( newSocket ) ),
           acceptor( io_service ),
           endpoint_iterator(),
           connected( true )
      {
      }

      virtual ~BoostTcpConnection();

      //is connected from boost socket
      bool isConnected()
      {
         return connected;
      }

      virtual std::string read( size_t len )
      {
         bool is_disconnected = false;
         std::string byte_string = std::move( BoostAsioUtils::read( socket, len, is_disconnected ) );

         if( is_disconnected )
         {
            // TODO Handle disconnect logic
            std::cout << __PRETTY_FUNCTION__ << " needs to handle disconnect logic" << std::endl;
         }

         return byte_string;
      }

      virtual std::string read()
      {
         std::cerr << __PRETTY_FUNCTION__ << " not implemented for TCP sockets" << std::endl;
         return "";
      }

      virtual void write( const std::string &data )
      {
         try
         {
            BoostAsioUtils::write( socket, data );
         }
         catch( const boost::exception& e )
         {
            std::cout << "CONNECTION DROPPED" << std::endl;
            connected = false;
         }
      }

      virtual bool isDataAvailable()
      {
         if( socket.available() > 0 )
         {
            return true;
         }
         else
         {
            return false;
         }
      }

      virtual size_t dataSize()
      {
         return socket.available();
      }

   private:

      void connectToTcpServer()
      {
         // Note: could also use endpoint
         //   boost::asio::ip::tcp::endpoint server_addr( boost::asio::ip::address::make_address(ip), port );
         //   socket.async_connect( server_addr, boost::bind(&BoostTcpConnection::handleServerResponse, conn,
         //          boost::asio::placeholders::error));

         // set up async event to start connection with server
         boost::asio::async_connect( socket,
                                     endpoint_iterator,
                                     boost::bind( &BoostTcpConnection::handleServerResponse, this,
                                           boost::asio::placeholders::error ) );
      }


      void handleServerResponse( const boost::system::error_code& error )
      {
         if( !error )
         {
            std::cout << __PRETTY_FUNCTION__ << " connected to the server" << std::endl;
            connected = true;
         }
         else
         {
            std::cerr << __PRETTY_FUNCTION__ << " " << error << std::endl;
            std::cerr << error.message() << std::endl;

            boost::this_thread::sleep_for( boost::chrono::seconds( 1 ) );
            std::cerr << "Attempting to connect to server again" << std::endl;

            connectToTcpServer();
         }
      }

      // Sample code for async receive callback func
      //   void handleReceive( const boost::system::error_code& error,
      //                       size_t received_bytes )
      //   {
      //      if( error )
      //      {
      //         std::cerr << __PRETTY_FUNCTION__ << " " << error << std::endl;
      //         std::cerr << error.message() << std::endl;
      //      }
      //
      //      std::cout << "Received: ";
      //      std::cout.write( recv_buf.data(), static_cast<int>(received_bytes) );
      //   }

      boost::asio::ip::tcp::socket socket;
      boost::asio::ip::tcp::acceptor acceptor;
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator;

      bool connected;

};


#endif /* BOOST_TCP_CONNECTION_H_ */

