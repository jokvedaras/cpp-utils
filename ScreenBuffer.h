#ifndef SCREENBUFFER_H_
#define SCREENBUFFER_H_

#include <string>
#include <fstream>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>

// Screen Buffer forces std::cout and std::cerr to be printed to
// a file as well as the screen
class ScreenBuffer
{
   public:
      ScreenBuffer( const std::string& filename );

      virtual ~ScreenBuffer();

   private:
      ScreenBuffer( const ScreenBuffer& );
      ScreenBuffer operator=( const ScreenBuffer& );

      // old cout and cerr buffers
      std::streambuf *coutbuf;
      std::streambuf *cerrbuf;

      // file to output to
      std::ofstream outfile;

      // attempt to use boost tee to print to both screen and file
      typedef boost::iostreams::tee_device<std::ostream, std::ofstream> Tee;
      typedef boost::iostreams::stream<Tee> TeeStream;
      std::ostream couthelper;
      Tee tee;
      TeeStream both;
};

#endif /* SCREENBUFFER_H_ */
