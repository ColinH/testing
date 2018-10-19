// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_TRAITS_HPP

#include "../external/json.hpp"

#include "annotation.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
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
         struct traits< annotation::type >
         {
            template< template< typename... > class Traits, typename Consumer >
            static void produce( Consumer& c, const annotation::type t )
            {
               if( t == annotation::ADDITION ) {
                  c.string( "ADDITION" );
               }
               else {
                  c.string( "REFERENCE" );
               }
            }
         };

         template<>
         struct traits< position >
            : json::binding::object< TAO_JSON_BIND_REQUIRED( "source", &position::source ),
                                     TAO_JSON_BIND_REQUIRED( "line", &position::line ),
                                     TAO_JSON_BIND_REQUIRED( "byte_in_line", &position::byte_in_line ) >
         {
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
