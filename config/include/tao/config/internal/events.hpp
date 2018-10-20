// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_EVENTS_HPP
#define TAO_CONFIG_INTERNAL_EVENTS_HPP

#include "../external/json.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Consumer, template< typename... > class Traits >
         void events_from_value( Consumer& c, const json::basic_value< Traits >& v )
         {
            c.begin_object( 2 );
            c.key( "meta" );
            json::events::produce< Traits >( c, v.public_base() );
            c.member();
            c.key( "data" );
            json::events::from_value< events_from_value< Consumer, Traits > >( c, v );
            c.member();
            c.end_object( 2 );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
