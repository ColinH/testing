// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_TO_STREAM_HPP
#define TAO_CONFIG_TO_STREAM_HPP

#include "value.hpp"

#include "internal/events.hpp"

namespace tao
{
   namespace config
   {
      inline void to_stream( std::ostream& os, const value& v )
      {
         json::jaxn::events::to_stream consumer( os );
         internal::events_from_value( consumer, v );
      }

      inline void to_stream( std::ostream& os, const value& v, const std::size_t indent )
      {
         json::jaxn::events::to_pretty_stream consumer( os, indent );
         internal::events_from_value( consumer, v );
      }

   }  // namespace config

}  // namespace tao

#endif
