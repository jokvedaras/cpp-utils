#ifndef SIGNALHANDLER_H_
#define SIGNALHANDLER_H_

#include <csignal>
#include <functional>
#include <iostream>
#include <vector>

// forward declaration of function
void signalCallback( int signal );

class SignalHandler
{
   public:
      SignalHandler()
      {
         // Listen for all signals
         std::signal( SIGABRT, signalCallback );
         std::signal( SIGFPE,  signalCallback );
         std::signal( SIGILL,  signalCallback );
         std::signal( SIGINT,  signalCallback );
         std::signal( SIGTERM, signalCallback );
         // Don't catch seg fault so stack trace can be created
         // std::signal( SIGSEGV, signalCallback );
      }

      virtual ~SignalHandler() = default;

      void setSignalCallback( const std::function<void()> &cb )
      {
         callbacks.push_back( cb );
      }

      const std::vector<std::function<void()>>& getSignalCallbacks() const
      {
         return callbacks;
      }

   private:
      std::vector<std::function<void()>> callbacks;
};

// Define global instance
static SignalHandler signal_handler;

void signalCallback( int signal )
{
   std::cerr << "Received signal:" << signal << std::endl;

   // callback appropriate functions
   for( const auto& cb : signal_handler.getSignalCallbacks() )
   {
      cb();
   }

   // Just in case we get stuck in the program for some reason,
   // exit the application after the 5th SIGINT signal
   if( signal == SIGINT )
   {
      static unsigned int sigint_counter = 0;
      ++sigint_counter;

      if( sigint_counter >= 5 )
      {
         std::cerr << "5 SIGINT Signals received.  Exiting program" << std::endl;
         exit( 1 );
      }
   }
}

#endif /* SIGNALHANDLER_H_ */
