#ifndef BOOSTASIOUTILS_H_
#define BOOSTASIOUTILS_H_

#include <iostream>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

// Utility methods for Boost Asio functions
namespace BoostAsioUtils
{
   template<typename Error>
   void displayError( const Error &error )
   {
      std::cerr << __PRETTY_FUNCTION__ << " " << error << std::endl;
      std::cerr << error.message() << std::endl;
   }

   template<typename BoostSocket>
   void write( BoostSocket &socket, const std::string &data )
   {
      size_t n = boost::asio::write( socket, boost::asio::buffer( data ) );

      if( n != data.length() )
      {
         // sanity check for our assumptions
         std::cerr << __PRETTY_FUNCTION__ << " unexpected behavior!" << std::endl;
         throw std::runtime_error( "Did not send entire buffer in boost::asio::write" );
      }

      // if speed becomes a problem, we can async_write messages.
      // a few things to note is the sending of messages is not thread safe
      // we would have to copy the "data" into a queue and have a another thread
      // process that queue and and write the data.
      // boost::asio::async_write(socket, boost::asio::buffer( data ),
      //      boost::bind(&BoostTcpConnection::handleWrite, this, curr_index) );
      // NOTE: the "data" variable can not go out of scope until the writing is complete
      //       (callback method is called)
   }

   template<typename BoostSocket>
   std::string read( BoostSocket &socket, size_t len, bool &disconnected )
   {
      // only perform read operation if data is available
      if( socket.available() >= len )
      {
         boost::asio::streambuf data( len );
         // sanity check. Ensures that the output sequence can accommodate
         // n characters, reallocating character array objects as necessary
         data.prepare( len );
         boost::system::error_code error;

         size_t n = boost::asio::read( socket,
                                       data,
                                       boost::asio::transfer_all(),
                                       error );

         if( error == boost::asio::error::eof ||
               error == boost::asio::error::connection_reset )
         {
            // handle the disconnect.
            disconnected = true;
         }
         else if( error )
         {
            displayError( error );
         }
         else
         {
            // received data is "committed" from output sequences to input sequence
            data.commit( n );

            if( n != len )
            {
               // sanity check for our assumptions
               std::cerr << __PRETTY_FUNCTION__ << " unexpected behavior!" << std::endl;
               throw std::runtime_error( "Unexpected result from boost::asio::read" );
            }

            // copy the buffer into a string for safety
            std::ostringstream iss;
            iss << &data;

            return iss.str();
         }
      }
      else
      {
         std::cerr << "Data is not yet available" << std::endl;
      }
      return "";
   }

   template<typename BoostSocket, typename Buffer, typename Callback>
   void registerForAsyncReceive( BoostSocket &socket, Buffer &recv_buf, Callback &func )
   {
      // TODO need to add a public function async_read_until
      // schedule the next asynchronous read
      socket.async_receive(
         boost::asio::buffer( recv_buf ),
         func );
   }
}

#endif /* BOOSTASIOUTILS_H_ */
