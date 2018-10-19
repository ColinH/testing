// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_TO_STREAM_HPP
#define TAO_CONFIG_INTERNAL_TO_STREAM_HPP

#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         namespace events
         {
            template< typename Consumer >
            void from_value( Consumer& c, const value& v )
            {
               c.begin_object( 3 );
               c.key( "type" );
               json::events::produce< traits >( c, v.t );
               c.member();
               c.key( "position" );
               json::events::produce< traits >( c, v.position );
               c.member();
               c.key( "data" );
               json::events::basic_from_value< from_value< Consumer >, Consumer, traits >( c, v );
               c.member();
               c.end_object( 3 );
            }

         }  // namespace events

         void to_stream( std::ostream& os, const value& v )
         {
            json::jaxn::events::to_stream consumer( os );
            events::from_value( consumer, v );
         }

         void to_stream( std::ostream& os, const value& v, const std::size_t indent )
         {
            json::jaxn::events::to_pretty_stream consumer( os, indent );
            events::from_value( consumer, v );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
