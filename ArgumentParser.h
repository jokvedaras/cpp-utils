#ifndef ARGUMENTPARSER_H_
#define ARGUMENTPARSER_H_

#include <vector>
#include <string>

#include "BoostUtils.h"

/**
 *  Provide a generic way to parse and interpret arguments for the executable
 */
class ArgumentParser
{
   public:
      ArgumentParser() = default;

      virtual ~ArgumentParser() = default;

      void readArgs( int argc, char *argv[] )
      {
         for( int i = 0; i < argc; ++i )
         {
            args.push_back( argv[i] );
         }
      }

      template<typename T>
      T getArg( int position, const std::string& arg_description )
      {
         if( position >= args.size() || position < 0 )
         {
            std::stringstream str;

            str << "No argument at position " << position << "\n"
                << "Arg Description:" << arg_description;

            throw std::runtime_error( str.str() );
         }
         else
         {
            return BoostUtils::safe_lexical_cast<T>( args.at( static_cast<unsigned int>( position ) ) );
         }
      }

      /// Attempt to find specific argument
      /// \return position of that argument if found. -1 otherwise
      int getArgPosition( const std::string& arg_str )
      {
         for( unsigned int i = 0; i < args.size(); ++i )
         {
            const auto& item = args[i];

            if( item == arg_str )
            {
               return static_cast<int>( i );
            }
         }

         // arg string not found
         return -1;
      }


   private:
      std::vector<std::string> args;
};

#endif /* ARGUMENTPARSER_H_ */
