#ifndef BOOSTUDPCONNECTION_H_
#define BOOSTUDPCONNECTION_H_

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include "SocketConnection.h"
#include "BoostAsioUtils.h"

class BoostUdpConnection : public SocketConnection
{
   public:
      // Server configuration
      BoostUdpConnection( boost::asio::io_service& io_service, short unsigned int r_port )
         : remote_endpoint(),
           socket( io_service, boost::asio::ip::udp::endpoint( boost::asio::ip::udp::v4(), r_port ) )
      {
      }

      // Client configuration
      BoostUdpConnection( boost::asio::io_service& io_service, const std::string& s_ip, const short unsigned int s_port )
         : remote_endpoint( ),
           socket( io_service )
      {
         // Attempt to resolve IP address or hostname
         boost::asio::ip::udp::resolver::query query( boost::asio::ip::udp::v4(),
               s_ip,
               boost::lexical_cast<std::string>( s_port ) );
         boost::asio::ip::udp::resolver resolver( io_service );
         boost::asio::ip::udp::resolver::iterator iter = resolver.resolve( query );
         remote_endpoint = *iter;

         // if you don't initialize socket with an endpoint, you must open socket manually
         socket.open( boost::asio::ip::udp::v4() );
      }

      // Client - Server configuration
      BoostUdpConnection( boost::asio::io_service& io_service,
                          const std::string& s_ip,
                          const short unsigned int s_port,
                          const short unsigned int r_port )
         : remote_endpoint( ),
           socket( io_service, boost::asio::ip::udp::endpoint( boost::asio::ip::udp::v4(), r_port ) )
      {
         // Attempt to resolve IP address or hostname
         boost::asio::ip::udp::resolver::query query( boost::asio::ip::udp::v4(),
               s_ip,
               boost::lexical_cast<std::string>( s_port ) );
         boost::asio::ip::udp::resolver resolver( io_service );
         boost::asio::ip::udp::resolver::iterator iter = resolver.resolve( query );
         remote_endpoint = *iter;
      }

      BoostUdpConnection( boost::asio::io_service& io_service,
                          const std::string& multicast_address,
                          const short unsigned int port,
                          bool )
         : remote_endpoint( boost::asio::ip::udp::endpoint( boost::asio::ip::address::from_string( multicast_address ), port ) ),
           socket( boost::asio::ip::udp::socket( io_service, remote_endpoint.protocol() ) )
      {
         socket.set_option( boost::asio::ip::udp::socket::reuse_address( true ) );
         socket.bind( remote_endpoint );

         // Join multicast group
         socket.set_option( boost::asio::ip::multicast::join_group( boost::asio::ip::address::from_string(
                               multicast_address ) ) );

         // Set Multicast TTL (time to live)
         // TODO - Make configurable
         socket.set_option( boost::asio::ip::multicast::hops( 10 ) );
      }
      virtual ~BoostUdpConnection();

      virtual std::string read( size_t )
      {
         std::cerr << __PRETTY_FUNCTION__ << " not implemented for UDP sockets" << std::endl;
         return "";
      }

      virtual size_t dataSize()
      {
         return socket.available();
      }

      virtual std::string read( )
      {
         // TODO there needs to be a dynamic buffer we could use instead...
         std::array<char, 512> recv_buf;
         boost::system::error_code error;

         if( isDataAvailable() == false )
         {
            // if no data is available, return
            return "";
         }

         // NOTE: receive blocks until something is received.
         size_t n = socket.receive( boost::asio::buffer( recv_buf ), 0, error );

         if( error )
         {
            BoostAsioUtils::displayError( error );
         }
         else
         {
            if( n > 512 )
            {
               std::cerr << __PRETTY_FUNCTION__ << " overflowed the buffer!" << std::endl;
            }

            std::string str =  std::string( recv_buf.begin(), recv_buf.end() );
            return str.erase( n, std::string::npos );
         }
         return "";
      }

      virtual void write( const std::string &byte_string )
      {
         sendTo( byte_string );
      }

      virtual bool isConnected()
      {
         return socket.is_open();
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

   private:
      void sendTo( const std::string &byte_string )
      {
         size_t n = socket.send_to( boost::asio::buffer( byte_string ), remote_endpoint );

         if( n != byte_string.size() )
         {
            // if we did not send the entire message, print an error message
            std::cerr << __PRETTY_FUNCTION__ << " - Did not send the entire message!" << std::endl;
         }
      }

      boost::asio::ip::udp::endpoint remote_endpoint;
      boost::asio::ip::udp::socket socket;
};

#endif /* BOOSTUDPCONNECTION_H_ */
