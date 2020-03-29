#ifndef SYSTEMCALL_H_
#define SYSTEMCALL_H_

#include <string>

/**
 * Abstraction around a system call
 * NOTE: Only works on linux!
 */
class SystemCall
{
   public:
      SystemCall() = default;

      /**
       * Construct a system call with a given command
       */
      SystemCall( const std::string &cmd );

      virtual ~SystemCall() = default;

      /**
       * Execute the current stored command
       * \return result of the command
       */
      const std::string& execute();

      /**
       * Execute the command passed in
       * \return result of the command
       */
      const std::string& execute( const std::string &cmd );

      /**
       * Get the last result
       * \return result of last run command
       */
      const std::string& getLastResult();

   private:
      std::string command;
      std::string result;
};

#endif /* SYSTEMCALL_H_ */
