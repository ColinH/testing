// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_POINTER_HPP
#define TAO_CONFIG_INTERNAL_POINTER_HPP

#include <vector>

#include "token.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         using pointer = std::vector< token >;

         inline pointer array_to_pointer( const std::vector< value >& v )
         {
            pointer result;

            for( const auto& i : v ) {
               //  assert( !i.kind );

               if( i.is_string_type() ) {
                  result.push_back( token( i.as< std::string >() ) );
               }
               else if( i.is_number() ) {
                  result.push_back( token( i.as< std::size_t >() ) );
               }
               else {
                  throw "TODO";
               }
            }
            return result;
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
