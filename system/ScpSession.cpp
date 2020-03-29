#include "ScpSession.h"

#include <iostream>
#include <stdexcept>
#include <fstream>

#include <libssh/libssh.h>
#include <sys/stat.h>
#include <boost/filesystem.hpp>

namespace
{
   ssh_session session;
   ssh_scp scp_session;
   int rc;
   bool is_open = false;
}

ScpSession::ScpSession( ssh_session_struct *session_in, Type st )
   : session_type( st )
{
   session = session_in;

   int mode = SSH_SCP_RECURSIVE;

   if( session_type == WRITE )
   {
      mode |=  SSH_SCP_WRITE;
   }
   else
   {
      mode |= SSH_SCP_READ;
   }

   scp_session = ssh_scp_new( session, mode, "." );

   if( scp_session == nullptr )
   {
      std::cerr << "Error allocating scp session:" << ssh_get_error( session )
                << std::endl;
      throw std::runtime_error( "Error creating scp session" );
   }

   rc = ssh_scp_init( scp_session );

   if( rc != SSH_OK )
   {
      std::cerr << "Error initializing scp session:" << ssh_get_error( session )
                << std::endl;
      throw std::runtime_error( "Error initializing scp session" );
   }

   is_open = true;
}

ScpSession::~ScpSession()
{
   close();
}

void ScpSession::copyFileToHost( const std::string& file, const std::string& dest_folder )
{
   // make sure file exists
   if( !boost::filesystem::exists( file ) )
   {
      std::cerr << "ScpSession::copyFileToHost File:" << file << " does not exist!" << std::endl;
      return;
   }

   // you create directories with ssh_scp_push_directory.  In recursive mode, you are
   // placed in this directory once it is created.  If the directory already exists
   // and you are in recursive mode, you simply enter that directory
   rc = ssh_scp_push_directory( scp_session, dest_folder.c_str(), S_IRWXU );

   if( rc != SSH_OK )
   {
      std::cerr << "Cannot create remote directory:" << ssh_get_error( session )
                << std::endl;
      throw std::runtime_error( "Error creating remote directory" );
   }

   // Creating files is done in two steps. First, you prepare the writing with
   // ssh_scp_push_file(). Then, you write the data with ssh_scp_write().
   // The length of the data to write must be identical between both function calls.
   // There's no need to "open" nor "close" the file, this is done automatically
   // on the remote end. If the file already exists, it is overwritten and truncated.

   // extract just the filename from the input file
   boost::filesystem::path p( file );
   std::string filename = p.filename().string();

   // calculate filesize and capture data into a buffer
   int filesize = 0;
   char *buf    = nullptr;

   std::ifstream fin( file, std::ios::binary );
   if( fin )
   {
      fin.seekg( 0, std::ios::end );

      // get file size in bytes
      std::ios::pos_type bufsize = fin.tellg();

      // rewind to beginning of file
      fin.seekg( 0 );

      // read contents into buffer
      filesize = static_cast<int>( bufsize );
      buf = new char[filesize];

      fin.read( buf, filesize );
   }

   rc = ssh_scp_push_file( scp_session, filename.c_str(), filesize, S_IRUSR | S_IWUSR );

   if( rc != SSH_OK )
   {
      std::cerr << "Cannot open remote file:" << ssh_get_error( session )
                << std::endl;
      delete[] buf;
      throw std::runtime_error( "Error opening remote file" );
   }

   // attempt to write the data
   rc = ssh_scp_write( scp_session, buf, filesize );

   if( rc != SSH_OK )
   {
      std::cerr << "Can not write to remote file:" << ssh_get_error( session )
                << std::endl;
      throw std::runtime_error( "Error writing to remote file" );
   }

   delete[] buf;
}

void ScpSession::close()
{
   if( is_open )
   {
      ssh_scp_close( scp_session );
      ssh_scp_free( scp_session );
      is_open = false;
   }
}

