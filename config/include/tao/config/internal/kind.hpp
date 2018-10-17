// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_KIND_HPP
#define TAO_CONFIG_INTERNAL_KIND_HPP

#include <string>
#include <ostream>

namespace tao
{
   namespace config
   {
      namespace internal
      {
         enum class kind : bool
         {
            ADDITION,
            REFERENCE
         };

         inline std::string to_string( const kind k )
         {
            switch( k ) {
               case kind::ADDITION:
                  return "addition";
               case kind::REFERENCE:
                  return "reference";
            }
         }

         inline std::ostream& operator<<( std::ostream& o, const kind k )
         {
            o << to_string( k );
            return o;
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
