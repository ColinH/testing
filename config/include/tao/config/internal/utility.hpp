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
            assert( st.stack.back()->t == annotation::ADDITION );
            assert( st.stack.back()->is_array() );

            st.stack.emplace_back( &st.stack.back()->get_array().emplace_back( T{ 0 } ) );
         }

         inline token token_from_value( const value& v )
         {
            assert( !v.t );

            switch( v.type() ) {
               case json::type::STRING:
               case json::type::STRING_VIEW:
                  return token( v.as< std::string >() );
                  break;
               case json::type::SIGNED:
               case json::type::UNSIGNED:
                  return token( v.as< std::size_t >() );
                  break;
               case json::type::NULL_:
                  return token();
               default:
                  throw std::runtime_error( "invalid json type for reference" );
            }
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
