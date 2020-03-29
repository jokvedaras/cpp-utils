#ifndef SCPSESSION_H_
#define SCPSESSION_H_

#include <string>

class ssh_session_struct;

/**
 *  Abstraction around libssh scp session
 */
class ScpSession
{
   public:

      enum Type
      {
         READ,
         WRITE
      };

      /**
       *  Construct a new SCP sesion with an ssh session
       *  \param[in] existing ssh session
       *  \param[in] type of scp session
       */
      ScpSession( ssh_session_struct *session, Type session_type );

      virtual ~ScpSession();

      /**
       *  Copy a file located on the filesystem to the destination folder on the remote
       *  \param[in] file to copy
       *  \param[in] folder to copy to
       */
      void copyFileToHost( const std::string& file, const std::string& dest_folder );

      /**
       *  Close the scp session
       */
      void close();

   private:
      Type session_type;

};

#endif /* SCPSESSION_H_ */
