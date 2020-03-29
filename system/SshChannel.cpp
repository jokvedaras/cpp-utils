#include "SshChannel.h"

#include <stdexcept>
#include <iostream>

#include <libssh/libssh.h>

namespace
{
   ssh_channel channel;
   int rc;
   bool shell_open = false;
}

SshChannel::SshChannel( ssh_session_struct *session )
{
   channel = ssh_channel_new( session );

   if( channel == NULL )
   {
      throw std::runtime_error( "Error while creating ssh channel" );
   }

   rc = ssh_channel_open_session( channel );

   if( rc != SSH_OK )
   {
      ssh_channel_free( channel );
      throw std::runtime_error( "Error while opening ssh channel" );
   }
}

SshChannel::~SshChannel()
{
   close();
}

bool SshChannel::isOpen() const
{
   if( ssh_channel_is_open( channel ) && !ssh_channel_is_eof( channel ) )
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool SshChannel::requestShell()
{
   rc = ssh_channel_request_shell( channel );

   if( rc != SSH_OK )
   {
      return false;
   }

   shell_open = true;

   return true;
}

std::string SshChannel::execute( const std::string& cmd )
{
   if( !isOpen() )
   {
      std::cerr << "Could not execute command:" << cmd << std::endl;
      std::cerr << "SSH Channel is not open" << std::endl;
      return "";
   }

   if( !shell_open )
   {
      std::cerr << "Could not execute command:" << cmd << std::endl;
      std::cerr << "SSH Shell is not open" << std::endl;
      return "";
   }

   // read off existing data from channel
   read( false, false );

   // update command to include return character
   std::string channel_cmd = cmd + "\n";

   std::cout << "SshChannell::execute command:" << channel_cmd << std::endl;

   // write command over ssh channel
   int nwritten = ssh_channel_write( channel, channel_cmd.c_str(), channel_cmd.length() );

   if( nwritten != channel_cmd.length() )
   {
      throw std::runtime_error( "Did not write entire command over ssh channel" );
   }

   return read( true, false );

}

void SshChannel::close()
{
   ssh_channel_close( channel );
   ssh_channel_send_eof( channel );
   ssh_channel_free( channel );

   shell_open = false;
}

std::string SshChannel::read( bool wait_for_data, bool stdcerr )
{
   char buffer[256];
   int nbytes;
   std::string data;

   if( wait_for_data )
   {
      // wait until some data is available on the channel
      nbytes = ssh_channel_read( channel, buffer, 1, 0 );
      data += std::string( buffer, nbytes );
   }

   // read all the data that is available
   do
   {
      nbytes = ssh_channel_read_nonblocking( channel, buffer, sizeof( buffer ), static_cast<int>( stdcerr ) );
      data += std::string( buffer, nbytes );
   }
   while( nbytes > 0 );

   std::cout << "SshChannel::read data:" << data << std::endl;

   return data;
}

