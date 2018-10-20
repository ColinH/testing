// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_TO_STREAM_HPP
#define TAO_CONFIG_INTERNAL_TO_STREAM_HPP

#include "events.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline void to_stream( std::ostream& os, const value& v )
         {
            json::jaxn::events::to_stream consumer( os );
            events_from_value( consumer, v );
         }

         inline void to_stream( std::ostream& os, const value& v, const std::size_t indent )
         {
            json::jaxn::events::to_pretty_stream consumer( os, indent );
            events_from_value( consumer, v );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
