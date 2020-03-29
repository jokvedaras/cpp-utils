#include "gtest/gtest.h"

#include <stdlib.h>
#include <iostream>

#include <boost/thread.hpp>

#include "SocketConnection.h"
#include "BoostSocketService.h"
#include "TcpServer.h"

class TcpSocketsTest : public ::testing::Test
{
      //test fixture used by all Tcp test
      virtual void SetUp()
      {
         std::random_device rd;
         std::mt19937 gen( rd() );
         std::uniform_int_distribution<short unsigned int> distribution( 1500, 65535 );
         port = distribution( gen );

         std::cout << "Staring TCP server on port " << port << std::endl;
         tcpServer = socket_service.startTcpServer( port );

         std::cout << "Starting TCP client1 on port " << port << std::endl;
         client1 = socket_service.startTcpClient( "localhost", port );


         while( client1->isConnected() == false )
         {
            boost::this_thread::sleep_for( boost::chrono::seconds( 1 ) );
         }
         while( tcpServer->isConnectionAvailable() == false )
         {
            boost::this_thread::sleep_for( boost::chrono::seconds( 1 ) );
         }

         server = std::move( tcpServer->getConnection() );


         std::cout << "Starting TCP client2 on port " << port << std::endl;
         client2 = socket_service.startTcpClient( "localhost", port );

         while( client2->isConnected() == false )
         {
            boost::this_thread::sleep_for( boost::chrono::seconds( 1 ) );
         }
         while( tcpServer->isConnectionAvailable() == false )
         {
            boost::this_thread::sleep_for( boost::chrono::seconds( 1 ) );
         }

         server2 = std::move( tcpServer->getConnection() );

         std::cout << "Starting TCP client3 on port " << port << std::endl;
         client3 = socket_service.startTcpClient( "localhost", port );

         while( client3->isConnected() == false )
         {
            boost::this_thread::sleep_for( boost::chrono::seconds( 1 ) );
         }
         while( tcpServer->isConnectionAvailable() == false )
         {
            boost::this_thread::sleep_for( boost::chrono::seconds( 1 ) );
         }

         server3 = std::move( tcpServer->getConnection() );
      }

      virtual void TearDown()
      {
      }

   public:
      BoostSocketService socket_service;
      std::unique_ptr<TcpServer> tcpServer;
      std::unique_ptr<SocketConnection> server;
      std::unique_ptr<SocketConnection> server2;
      std::unique_ptr<SocketConnection> server3;
      std::unique_ptr<SocketConnection> client1;
      std::unique_ptr<SocketConnection> client2;
      std::unique_ptr<SocketConnection> client3;
      std::string received;
      short unsigned int port;

};

TEST_F( TcpSocketsTest, SingleClientService )
{
   //test the read/write between client and server
   client1->write( "Msg #0: Client sending server a message" );
   received = server->read( 20 );
   EXPECT_EQ( "Msg #0: Client sendi", received );

   client1->write( "Msg #1: Hello world" );
   client1->write( "Msg #2: Final Message" );
   received = server->read( 20 );
   EXPECT_EQ( "ng server a messageM", received );

   received = server->read( 20 );
   EXPECT_EQ( "sg #1: Hello worldMs", received );

   received = server->read( 19 );
   EXPECT_EQ( "g #2: Final Message", received );

   received = server->read( 20 );
   EXPECT_EQ( "", received );
}


TEST_F( TcpSocketsTest, TripleClientConnection )
{
   //test that a server is able to connect to three clients

   EXPECT_TRUE( server->isConnected() );
   EXPECT_TRUE( client1->isConnected() );

   EXPECT_TRUE( server2->isConnected() );
   EXPECT_TRUE( client2->isConnected() );

   EXPECT_TRUE( server3->isConnected() );
   EXPECT_TRUE( client3->isConnected() );

}


TEST_F( TcpSocketsTest, serviceThreeClients )
{
   client1->write( "Client 1 is sending a message" );
   client2->write( "Client 2 is also sending a message" );
   client3->write( "Client 3 says hello world" );

   received = server->read( 20 );
   EXPECT_EQ( "Client 1 is sending ", received );
   received = server->read( 9 );
   EXPECT_EQ( "a message", received );

   received = server2->read( 20 );
   EXPECT_EQ( "Client 2 is also sen", received );
   received = server2->read( 14 );
   EXPECT_EQ( "ding a message", received );

   received = server3->read( 20 );
   EXPECT_EQ( "Client 3 says hello ", received );
   received = server3->read( 5 );
   EXPECT_EQ( "world", received );
}


TEST_F( TcpSocketsTest, ServerTest )
{
   server->write( "Main Socket is live" );
   server2->write( "Verifying addition connections" );
   server3->write( "Server can talk to client" );

   received = client1->read( 19 );
   EXPECT_EQ( "Main Socket is live", received );

   received = client2->read( 30 );
   EXPECT_EQ( "Verifying addition connections", received );

   received = client3->read( 25 );
   EXPECT_EQ( "Server can talk to client", received );

}

TEST( SocketTest, udpTest )
{
   BoostSocketService socket_service;
   std::random_device rd;
   std::mt19937 gen( rd() );
   std::uniform_int_distribution<short unsigned int> distribution( 1500, 65535 );
   short unsigned int port = distribution( gen );

   std::unique_ptr<SocketConnection> udp_server = socket_service.startUdpConnection( port ) ;
   std::unique_ptr<SocketConnection> udp_client = socket_service.startUdpConnection( "localhost", port );
   std::string received;

   udp_client->write( "Udp Msg 1" );
   udp_client->write( "Udp Msg 2" );

   received = udp_server->read();
   EXPECT_EQ( "Udp Msg 1", received );

   received = udp_server->read();
   EXPECT_EQ( "Udp Msg 2", received );
}

// Test to see if the client can connect to a server
// if the server comes up after the client
TEST( SocketTest, TcpClientConnectBeforeServer )
{
   // find a random port
   std::random_device rd;
   std::mt19937 gen( rd() );
   std::uniform_int_distribution<short unsigned int> distribution( 1500, 65535 );
   short unsigned int port = distribution( gen );

   BoostSocketService socket_service;

   std::unique_ptr<TcpServer> tcpServer;
   std::unique_ptr<SocketConnection> client1;

   std::cout << "Starting TCP client before server on port " << port << std::endl;
   client1 = socket_service.startTcpClient( "localhost", port );

   boost::this_thread::sleep_for( boost::chrono::seconds( 1 ) );

   std::cout << "Staring TCP server after client on port " << port << std::endl;
   tcpServer = socket_service.startTcpServer( port );

   boost::this_thread::sleep_for( boost::chrono::seconds( 2 ) );

   // should be connected now so lets check it out
   EXPECT_EQ( tcpServer->isConnectionAvailable(), true );
}

