// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_UTILITY_HPP

#include <cassert>
#include <stdexcept>
#include <vector>

#include "pointer.hpp"
#include "state.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline value* array_find( value& v, const std::size_t i )
         {
            auto& a = v.get_array();
            return ( i < a.size() ) ? ( a.data() + i ) : nullptr;
         }

         inline const value* array_find( const value& v, const std::size_t i )
         {
            const auto& a = v.get_array();
            return ( i < a.size() ) ? ( a.data() + i ) : nullptr;
         }

         inline std::size_t array_size( const std::vector< value >& a, const std::size_t n )
         {
            std::size_t r = 0;

            for( std::size_t i = 0; i < n; ++i ) {
               if( a[ i ].t ) {
                  throw std::runtime_error( "resolve requires array size of phase two reference" );
               }
               if( !a[ i ].is_array() ) {
                  throw std::runtime_error( "resolve requires array size of non-array" );
               }
               r += a[ i ].unsafe_get_array().size();
            }
            return r;
         }

         struct array_pair
         {
            std::size_t n;
            std::vector< value >& a;
         };

         inline array_pair array_find( std::vector< value >& a, std::size_t n )
         {
            for( auto& v : a ) {
               if( v.t ) {
                  throw std::runtime_error( "phase one across phase two reference" );
               }
               if( !v.is_array() ) {
                  throw std::runtime_error( "phase one size of non-array" );
               }
               auto& b = v.unsafe_get_array();
               const auto s = b.size();

               if( n < s ) {
                  return { n, b };
               }
               n -= s;
            }
            throw std::runtime_error( "phase one array out of bounds" );
         }

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
