#ifndef SSH_GTEST_H_
#define SSH_GTEST_H_

#include "SshSession.h"
#include "SshChannel.h"
#include "ScpSession.h"

#include <boost/filesystem.hpp>

std::string ssh_host = "localhost";
std::string ssh_user = "linux-dev";
std::string ssh_pass = "linux-dev";
std::string user_home = "/home/" + ssh_user;
std::string ssh_public_key  = user_home + "/.ssh/id_rsa.pub";
std::string ssh_private_key = user_home + "/.ssh/id_rsa";
std::string ssh_private_key_pass = "";

TEST( ssh, UserAuth_pass )
{
   SshSession session;

   session.connect( ssh_user, ssh_host );
   EXPECT_EQ( session.isConnected(), true );

   session.authenticate( ssh_pass );
   EXPECT_EQ( session.isAuthenticated(), true );
}

TEST( ssh, UserAuth_fail )
{
   SshSession session;

   session.connect( "not_my_username", ssh_host );
   EXPECT_EQ( session.isConnected(), true );

   EXPECT_ANY_THROW( session.authenticate( "not_my_password" ) );
   EXPECT_EQ( session.isAuthenticated(), false );
}

TEST( ssh, UserAuthKey_fail )
{
   SshSession session;

   session.connect( ssh_user, ssh_host );
   EXPECT_EQ( session.isConnected(), true );

   session.authenticate( "not_my_ssh_key", "not_my_ssh_key", "not_my_ssh_pass" );
   EXPECT_EQ( session.isAuthenticated(), false );
}

TEST( ssh, UserAuthKey_pass )
{
   SshSession session;

   session.connect( ssh_user, ssh_host );
   EXPECT_EQ( session.isConnected(), true );

// Private and Public key must have correct permissions.  I think this is
// why we are failing this step on the compilation server but it does work
// locally on the development vm
//   session.authenticate( ssh_public_key, ssh_private_key, ssh_private_key_pass );
//   EXPECT_EQ( session.isAuthenticated(), true );
}

TEST( ssh, SshChannel )
{
   SshSession session;

   session.connect( ssh_user, ssh_host );
   EXPECT_EQ( session.isConnected(), true );

   session.authenticate( ssh_pass );
   EXPECT_EQ( session.isAuthenticated(), true );

   auto channel = session.openChannel();
   EXPECT_NE( channel, nullptr );

   bool x = channel->requestShell();
   EXPECT_EQ( x, true );

   auto result = channel->execute( "ls -la /" );
   EXPECT_GT( result.length(), 0 );
}

TEST( scp, ScpSession )
{
   SshSession session;

   session.connect( ssh_user, ssh_host );
   EXPECT_EQ( session.isConnected(), true );

   session.authenticate( ssh_pass );
   EXPECT_EQ( session.isAuthenticated(), true );

   // delete the file if it already exists
   // For some reason, the file is being created in the users home area
   std::string test_file = user_home + "/tmp/cans_config.cfg";
   boost::filesystem::remove( test_file );

   auto scp = session.openScpSession( ScpSession::WRITE );
   scp->copyFileToHost( "./data/cans_config.cfg", "/tmp/" );

   // verify the file exists
   EXPECT_EQ( true, boost::filesystem::exists( test_file ) );
}

#endif /* SSH_GTEST_H_ */
