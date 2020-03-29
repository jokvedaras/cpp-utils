#include "SshSession.h"

#include <iostream>
#include <stdexcept>

#include <string.h>
#include <errno.h>

#include <libssh/libssh.h>

#include <boost/make_unique.hpp>
#include <boost/filesystem.hpp>

#include "SshChannel.h"
#include "ScpSession.h"

namespace
{
   ssh_session my_ssh_session;
   int verbosity = SSH_LOG_PROTOCOL;
   int rc;

   bool isConnectedVar     = false;
   bool isAuthenticatedVar = false;
}

SshSession::SshSession()
{

}

SshSession::~SshSession()
{
   close();
}

bool SshSession::connect( const std::string& username,
                          const std::string& host,
                          int port )
{
   my_ssh_session = ssh_new();
   if( my_ssh_session == nullptr )
   {
      throw std::runtime_error( "Error creating new ssh session" );
   }

   ssh_options_set( my_ssh_session, SSH_OPTIONS_HOST, host.c_str() );
   ssh_options_set( my_ssh_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity );
   ssh_options_set( my_ssh_session, SSH_OPTIONS_PORT, &port );
   ssh_options_set( my_ssh_session, SSH_OPTIONS_USER, username.c_str() );

   rc = ssh_connect( my_ssh_session );

   if( rc != SSH_OK )
   {
      std::cerr << "Error connecting to:" << host << ". Error:" << ssh_get_error( my_ssh_session );
      throw std::runtime_error( "Error during connection" );
   }

   if( verifyHost() == false )
   {
      throw std::runtime_error( "Problem verifying the host" );
   }

   isConnectedVar = true;

   return isConnectedVar;
}

bool SshSession::isConnected() const
{
   return isConnectedVar;
}

bool SshSession::authenticate( const std::string& password )
{
   if( !isConnected() )
   {
      return false;
   }

   rc = ssh_userauth_password( my_ssh_session, nullptr, password.c_str() );

   if( rc != SSH_AUTH_SUCCESS )
   {
      std::cerr << "Error authenticating with password:" <<
                ssh_get_error( my_ssh_session ) << std::endl;

      close();

      throw std::runtime_error( "Error during authentication" );
   }

   isAuthenticatedVar = true;

   return isAuthenticatedVar;
}

bool SshSession::authenticate( const std::string& public_key_loc,
                               const std::string& private_key_loc,
                               const std::string& private_key_pass )
{
   if( !isConnected() )
   {
      return false;
   }

   // verify the input filepaths
   if( !boost::filesystem::exists( public_key_loc ) && !boost::filesystem::exists( private_key_loc ) )
   {
      std::cerr << "ERROR: Can not authenticate ssh session. Public or private key is not valid" << std::endl;
      std::cerr << "Public Key:" << public_key_loc << std::endl;
      std::cerr << "Private Key:" << private_key_loc << std::endl;
      return false;
   }

   // Retrieve the public key
   ssh_key public_key;
   rc = ssh_pki_import_pubkey_file( public_key_loc.c_str(), &public_key );

   if( rc != SSH_OK )
   {
      throw std::runtime_error( "Error when attempting to read public key" );
   }

   // Offer the public key to the SSH server
   // if the return value is SSH_AUTH_SUCCESS, the SSH server
   // accepts to authenticate using the public key
   rc = ssh_userauth_try_publickey( my_ssh_session, NULL, public_key );

   if( rc != SSH_AUTH_SUCCESS )
   {
      std::cerr << ssh_get_error( my_ssh_session ) << std::endl;
      std::cerr << "Public Key:" << public_key_loc << std::endl;
      throw std::runtime_error( "Error when asking server to accept public key" );
   }

   // Retrieve the private key
   ssh_key private_key;
   rc = ssh_pki_import_privkey_file( private_key_loc.c_str(), private_key_pass.c_str(), NULL, NULL, &private_key );

   if( rc != SSH_OK )
   {
      throw std::runtime_error( "Error when attempting to read private key" );
   }

   // Authenticate w/ the private key
   rc = ssh_userauth_publickey( my_ssh_session, NULL, private_key );

   if( rc != SSH_AUTH_SUCCESS )
   {
      throw std::runtime_error( "Error when asking server to accept private key" );
   }

   // clean up memory
   ssh_key_free( public_key );
   ssh_key_free( private_key );

   isAuthenticatedVar = true;

   return isAuthenticatedVar;
}

bool SshSession::isAuthenticated() const
{
   return isAuthenticatedVar;
}

std::unique_ptr<SshChannel> SshSession::openChannel()
{
   if( isConnectedVar && isAuthenticatedVar )
   {
      return boost::make_unique<SshChannel>( my_ssh_session );
   }

   return nullptr;
}

std::unique_ptr<ScpSession> SshSession::openScpSession( ScpSession::Type type )
{
   if( isConnectedVar && isAuthenticatedVar )
   {
      return boost::make_unique<ScpSession>( my_ssh_session, type );
   }

   return nullptr;
}

void SshSession::close()
{
   if( isConnectedVar )
   {
      ssh_disconnect( my_ssh_session );
      ssh_free( my_ssh_session );
   }

   isConnectedVar     = false;
   isAuthenticatedVar = false;
}

bool SshSession::verifyHost()
{
   int state;
   int hlen;
   unsigned int key_len;
   unsigned char *hash = nullptr;
   char *hexa;
   char buf[10];

   state = ssh_is_server_known( my_ssh_session );
   hlen  = ssh_get_pubkey_hash( my_ssh_session, &hash );

   if( hlen < 0 )
   {
      return false;
   }
   else
   {
      key_len = static_cast<unsigned int>( hlen );
   }

   switch( state )
   {
      case SSH_SERVER_KNOWN_OK:
         break; /* ok */
      case SSH_SERVER_KNOWN_CHANGED:
         std::cerr << "Host key for server changed: it is now:\n";
         ssh_print_hexa( "Public key hash", hash, key_len );
         std::cerr << "For security reasons, connection will be stopped\n";
         free( hash );
         return false;
      case SSH_SERVER_FOUND_OTHER:
         std::cerr << "The host key for this server was not found but an other"
                   "type of key exists.\n";
         std::cerr << "An attacker might change the default server key to"
                   "confuse your client into thinking the key does not exist\n";
         free( hash );
         return -1;
      case SSH_SERVER_FILE_NOT_FOUND:
         std::cerr << "Could not find known host file.\n";
         std::cerr << "If you accept the host key here, the file will be"
                   "automatically created.\n";
      /* fallback to SSH_SERVER_NOT_KNOWN behavior */
      case SSH_SERVER_NOT_KNOWN:
         hexa = ssh_get_hexa( hash, key_len );
         std::cerr << "The server is unknown. Do you trust the host key?\n";
         std::cerr << "Public key hash:" << hexa << std::endl;
         free( hexa );
         if( fgets( buf, sizeof( buf ), stdin ) == nullptr )
         {
            free( hash );
            return false;
         }
         if( strncasecmp( buf, "yes", 3 ) != 0 )
         {
            free( hash );
            return false;
         }
         if( ssh_write_knownhost( my_ssh_session ) < 0 )
         {
            std::cerr << "Error:" << strerror( errno ) << std::endl;
            free( hash );
            return false;
         }
         break;
      case SSH_SERVER_ERROR:
         std::cerr << "Error:" << ssh_get_error( my_ssh_session ) << std::endl;;
         free( hash );
         return false;
      default:
         throw std::runtime_error( "Error. Reached end of switch for verifying host" );
   }
   free( hash );

   return true;
}
