#ifndef TEMPLATEOBSERVABLE_H_
#define TEMPLATEOBSERVABLE_H_

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "TemplateObserver.h"

template<typename T>
class Observable
{
   public:
      Observable()
         : observer_map()
      {

      }

      virtual ~Observable() {};

      /// Call by an observer to register for updates
      /// \param [in] observer Reference to the observer requesting to subscribe
      /// \param [in] specialization for a certain topic
      ///
      void subscribe( Observer<T> &observer, const std::string& topic = "default" )
      {
         auto it = observer_map.find( topic );

         if( it != observer_map.end() )
         {
            it->second.push_back( &observer );
         }
         else
         {
            observer_map[topic] = std::vector<Observer<T>*>();
            observer_map[topic].push_back( &observer );
         }
      }

      ///
      /// Called by an Observer to stop receiving updates.
      ///
      /// \param [in] observer Reference to the Observer requesting to be un-subscribed
      /// \param [in] specialization for a certain topic
      ///
      void unsubscribe( const Observer<T> &observer, const std::string& topic = "default" )
      {
         auto it = observer_map.find( topic );

         if( it != observer_map.end() )
         {
            auto &listeners = it->second;

            for( auto vit = listeners.begin();
                  vit != listeners.end();
                  /* do nothing */ )
            {
               if( *vit == &observer )
               {
                  listeners.erase( vit );
               }
               else
               {
                  ++vit;
               }
            }
         }
         else
         {
            // do nothing
         }
      }

      ///
      /// Notifies all subscribed Observers when a change has taken place.
      ///
      /// \param [in] T& - object to notify listeners with
      /// \param [in] specialization for a certain topic
      ///
      void notify( const T& obj, const std::string& topic = "default" )
      {
         auto it = observer_map.find( topic );

         if( it != observer_map.end() )
         {
            auto &listeners = it->second;

            for( auto& listener : listeners )
            {
               listener->notify( obj );
            }
         }
      }

   private:
      std::map<std::string, std::vector<Observer<T>*>> observer_map;
};

#endif /* TEMPLATEOBSERVABLE_H_ */
