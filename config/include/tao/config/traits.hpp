// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_TRAITS_HPP
#define TAO_CONFIG_TRAITS_HPP

#include "external/json.hpp"

#include "annotation.hpp"

namespace tao
{
   namespace config
   {
      template< typename T >
      struct traits
         : public json::traits< T >
      {
      };

      template<>
      struct traits< void >
         : public json::traits< void >
      {
         template< typename Value >
         using public_base = annotation;
      };

      template<>
      struct traits< pointer >
      {
         template< template< typename... > class Traits, typename Consumer >
         static void produce( Consumer& c, const pointer& p )
         {
            c.string( json::to_string( p ) );
         }
      };

      template<>
      struct traits< annotation >
         : public json::binding::object< TAO_JSON_BIND_REQUIRED( "source", &annotation::source ),
                                         TAO_JSON_BIND_REQUIRED( "pointer", &annotation::pointer ),
                                         TAO_JSON_BIND_REQUIRED( "line", &annotation::line ),
                                         TAO_JSON_BIND_REQUIRED( "byte_in_line", &annotation::byte_in_line ) >
      {
         TAO_JSON_DEFAULT_KEY( "meta" );
      };

   }  // namespace config

}  // namespace tao

#endif
