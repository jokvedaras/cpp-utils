#ifndef SSHCHANNEL_H_
#define SSHCHANNEL_H_

#include <string>

class ssh_session_struct;

/**
 *  Abstraction around libssh channel
 */
class SshChannel
{
   public:
      /**
       * Construct a channel with an ssh session
       */
      SshChannel( ssh_session_struct *session );

      virtual ~SshChannel();

      /**
       * \return true if the channel is open and does not contain EOF
       */
      bool isOpen() const;

      /**
       * Request a non-interactive shell
       * \return true if successful
       */
      bool requestShell();

      /**
       * Execute a given command over the shell
       * \return the result as a string
       */
      std::string execute( const std::string& cmd );

      /**
       * Close the channel
       */
      void close();

   private:

      /**
       *   Read data from the ssh channel
       *
       *  \param[in] wait for data
       *  \param[in] read from stderr instead of stdout
       */
      std::string read( bool wait_for_data, bool stdcerr );
};

#endif /* SSHCHANNEL_H_ */
