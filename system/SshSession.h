#ifndef SSHSESSION_H_
#define SSHSESSION_H_

#include <memory>
#include <string>

#include "ScpSession.h"

class SshChannel;
class ScpSession;

/**
 *  Abstraction around libssh session
 */
class SshSession
{
   public:
      SshSession();

      virtual ~SshSession();

      /**
       *  Connect to a remote host
       *
       *  \param[in] host to connect to
       *  \param[in] username to use
       *  \param[in] port to connect to
       */
      bool connect( const std::string& username,
                    const std::string& host,
                    int port = 22 );

      /**
       *  Return true if the session is connected to a host
       */
      bool isConnected() const;

      /**
       * Authenticate session by password
       *
       * \param[in] password to use while authenticating
       */
      bool authenticate( const std::string& password );

      /**
       * Authenticate session by ssh keys
       *
       * \param[in] private key file
       * \param[in] private key password
       */
      bool authenticate( const std::string& public_key,
                         const std::string& private_key,
                         const std::string& private_key_pass );

      /**
       * Return true if the session is authenticated
       */
      bool isAuthenticated() const;

      /**
       * Open a new ssh channel
       */
      std::unique_ptr<SshChannel> openChannel();

      /**
       * Open a new scp session
       */
      std::unique_ptr<ScpSession> openScpSession( ScpSession::Type );

      /**
       * Close the session
       */
      void close();

   private:
      /**
       * Verify that we know the host we are connecting to
       */
      bool verifyHost();
};

#endif /* SSHSESSION_H_ */
