#ifndef TEMPLATEOBSERVER_H_
#define TEMPLATEOBSERVER_H_

template<typename T>
class Observer
{
   public:
      virtual ~Observer() {};

      ///
      /// Pure virtual method used to alert the observer that the subject has changed.
      ///
      /// \param [out] Pointer to the subject that has changed
      ///
      virtual void notify( const T &subject ) = 0;
};

#endif /* TEMPLATEOBSERVER_H_ */
