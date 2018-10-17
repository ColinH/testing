// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_TOKEN_HPP
#define TAO_CONFIG_INTERNAL_TOKEN_HPP

#include <cstddef>
#include <string>

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct token
         {
            enum type
            {
               KEY,
               INDEX,
               APPEND
            };

            token()
               : t( type::APPEND )
            {
            }

            token( const std::size_t index )
               : t( type::INDEX ),
                 i( index )
            {
            }

            token( const std::string& key )
               : t( type::KEY ),
                 k( key )
            {
            }

            type t;
            // TODO: Encapsulation w/union.
            std::size_t i;
            std::string k;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
