#include "ScreenBuffer.h"

#include <iostream>

ScreenBuffer::ScreenBuffer( const std::string &filename )
   : coutbuf( std::cout.rdbuf() ),
     cerrbuf( std::cerr.rdbuf() ),
     outfile( filename ),
     couthelper( std::cout.rdbuf() ),
     tee( couthelper, outfile ),
     both( tee )
{
   // write all stdcout and stdcerr to file
   // replace cout and cerr with file
   std::cout.rdbuf( both.rdbuf() );
   std::cerr.rdbuf( both.rdbuf() );
}

ScreenBuffer::~ScreenBuffer()
{
   // put cout and cerr back to original state
   std::cout.rdbuf( coutbuf );
   std::cerr.rdbuf( cerrbuf );
}

