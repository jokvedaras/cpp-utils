#include "SystemCall.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

SystemCall::SystemCall( const std::string &cmd )
   : command( cmd ),
     result( "" )
{

}

const std::string& SystemCall::execute()
{
   // clear out result
   result = "";

   // write stderr to stdout
   std::string command_to_execute = command + " 2>&1";

   FILE* stream;
   stream = popen( command_to_execute.c_str(), "r" );

   if( stream )
   {
      const int max_buffer = 256;
      char buffer[max_buffer];

      while( !feof( stream ) )
      {
         if( fgets( buffer, max_buffer, stream ) != NULL )
         {
            result.append( buffer );
         }
      }

      pclose( stream );
   }

   std::cout << "SystemCall Command:" << command_to_execute << std::endl;
   std::cout << "SystemCall Result:" << result << std::endl;

   return result;
}

const std::string& SystemCall::execute( const std::string &cmd )
{
   command = cmd;
   return execute();
}

const std::string& SystemCall::getLastResult()
{
   return result;
}
