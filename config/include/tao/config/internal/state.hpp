// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_STATE_HPP
#define TAO_CONFIG_INTERNAL_STATE_HPP

#include <string>
#include <vector>

#include "json.hpp"
#include "pointer.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct state
         {
            state()
            {
               ostack.emplace_back( &result );
            }

            object_t result;

            // General Structure

            std::vector< list_t* > lstack;  // Current rules::value_list
            std::vector< array_t* > astack;  // Array contexts via '['
            std::vector< object_t* > ostack;  // Object contexts via '{'

            // Phase 1 Extensions

            pointer key;  // TODO: PEGTL switching-style?
            std::string str;  // TODO: PEGTL switching-style?

            // Phase 2 References

            std::vector< json::value* > rstack;  // TODO: PEGTL switching-style?

            // Switching-Style Support

            template< typename Input >
            void binary( const Input& in, std::vector< std::byte >&& v )
            {
               assert( !lstack.empty() );

               lstack.back()->emplace_back( entry::atom( in, std::move( v ) ) );
            }

            template< typename Input >
            void string( const Input& in, std::string&& v )
            {
               assert( !lstack.empty() );

               lstack.back()->emplace_back( entry::atom( in, std::move( v ) ) );
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
