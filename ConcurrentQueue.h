#ifndef CONCURRENTQUEUE_H_
#define CONCURRENTQUEUE_H_

#include <queue>

#include <boost/thread.hpp>

/**
 *  Wrapper class around a queue to allow thread safe access
 */
template<typename T>
class ConcurrentQueue
{
   public:
      ConcurrentQueue() :
         queue(), mutex(), cond()
      {

      }

      virtual ~ConcurrentQueue()
      {

      }

      /// checks whether the queue is empty
      bool empty() const
      {
         return queue.empty();
      }

      /// blocking call that waits for data and returns the first object
      T pop()
      {
         boost::unique_lock<boost::mutex> mlock( mutex );
         while( queue.empty() )
         {
            cond.wait( mlock );
         }
         auto item = queue.front();
         queue.pop();
         return item;
      }

      /// blocking call that waits for data and copies the first object into provided memory
      void pop( T& item )
      {
         boost::unique_lock<boost::mutex> mlock( mutex );
         while( queue.empty() )
         {
            cond.wait( mlock );
         }
         item = queue.front();
         queue.pop();
      }

      /// push the object onto the queue
      void push( const T& item )
      {
         boost::unique_lock<boost::mutex> mlock( mutex );
         queue.push( item );
         mlock.unlock();
         cond.notify_one();
      }

      /// push the object onto the queue (handles r-value references)
      void push( T&& item )
      {
         boost::unique_lock<boost::mutex> mlock( mutex );
         queue.push( std::move( item ) );
         mlock.unlock();
         cond.notify_one();
      }

   private:
      std::queue<T> queue;
      boost::mutex mutex;
      boost::condition_variable cond;
};

#endif /* CONCURRENTQUEUE_H_ */
