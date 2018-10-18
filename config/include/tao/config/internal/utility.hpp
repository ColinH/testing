// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_UTILITY_HPP

#include <cassert>
#include <vector>

#include "pointer.hpp"
#include "resolve.hpp"
#include "state.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename T >
         void begin_container( state& st )
         {
            assert( !st.stack.empty() );
            assert( st.stack.back()->kind() == kind::ADDITION );
            assert( st.stack.back()->is_array() );

            st.stack.emplace_back( &st.stack.back()->get_array().emplace_back( T{ 0 } ) );
         }

         inline pointer array_to_pointer( const std::vector< value >& v )
         {
            pointer result;

            for( const auto& i : v ) {
               //  assert( !i.kind );

               switch( i.type() ) {
                  case json::type::STRING:
                  case json::type::STRING_VIEW:
                     result.push_back( token( i.as< std::string >() ) );
                     break;
                  case json::type::SIGNED:
                  case json::type::UNSIGNED:
                     result.push_back( token( i.as< std::size_t >() ) );
                     break;
                  case json::type::NULL_:
                     result.push_back( token() );
                  default:
                     throw "TODO";
               }
            }
            return result;
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
