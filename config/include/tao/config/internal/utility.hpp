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

         template< typename A, typename F >
         auto array_apply( A& a, std::size_t n, const F& f )
         {
            for( auto& v : a ) {
               if( v.t ) {
                  assert( v.t == annotation::REFERENCE );
                  throw std::runtime_error( "phase one across phase two reference" );
               }
               if( !v.is_array() ) {
                  throw std::runtime_error( "phase one size of non-array" );
               }
               auto& b = v.unsafe_get_array();
               const auto s = b.size();

               if( n < s ) {
                  return f( b, n );
               }
               n -= s;
            }
            throw std::runtime_error( "phase one array out of bounds" );
         }

         template< typename A, typename F >
         void object_apply_all( A& a, const F& f )
         {
            for( auto& v : a ) {
               if( v.t ) {
                  assert( v.t == annotation::REFERENCE );
                  throw std::runtime_error( "phase one across phase two reference" );
               }
               if( !v.is_object() ) {
                  throw std::runtime_error( "delete with name in non-object" );
               }
               f( v );
            }
         }

         template< typename A, typename F >
         auto object_apply_last( A& a, const std::string& k, const F& f, const std::nullptr_t )
         {
            for( std::size_t j = 0; j < a.size(); ++j ) {
               auto& w = a[ a.size() - j - 1 ];
               if( w.t ) {
                  assert( w.t == annotation::REFERENCE );
                  throw std::runtime_error( "resolve across phase two reference" );
               }
               if( auto* x = w.find( k ) ) {
                  return f( x );
               }
            }
            return static_cast< decltype( a.data() ) >( nullptr );
         }

         template< typename A, typename F >
         auto object_apply_last( A& a, const std::string& k, const F& f )
         {
            if( auto* t = object_apply_last( a, k, f, nullptr ) ) {
               return t;
            }
            throw std::runtime_error( "phase one name not found" );
         }

         template< typename T, typename Input >
         void begin_container( const Input& in, state& st )
         {
            assert( !st.stack.empty() );
            assert( st.stack.back()->t == annotation::ADDITION );
            assert( st.stack.back()->is_array() );

            st.stack.emplace_back( &st.stack.back()->get_array().emplace_back( T{ 0 } ) );
            st.stack.back()->position.set_position( in.position() );
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
