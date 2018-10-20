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
         auto array_apply_one( A* a, std::size_t n, const F& f )
         {
            assert( a->t == annotation::ADDITION );

            for( auto& v : a->get_array() ) {
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

         template< typename B, typename F >
         auto array_apply_last( B* b, const F& f )
         {
            assert( b->t == annotation::ADDITION );

            auto& a = b->get_array();

            for( std::size_t j = 0; j < a.size(); ++j ) {
               const std::size_t i = a.size() - j - 1;
               auto& v = a[ i ];

               if( v.t ) {
                  assert( v.t == annotation::REFERENCE );
                  throw std::runtime_error( "phase one across phase two reference" );
               }
               if( !v.is_array() ) {
                  throw std::runtime_error( "phase one size of non-array" );
               }
               auto& b = v.unsafe_get_array();

               if( !b.empty() ) {
                  return f( b, b.size() - 1 );
               }
            }
            throw std::runtime_error( "phase one array has no last element" );
         }

         template< typename A, typename F >
         void object_apply_all( A* a, const F& f )
         {
            assert( a->t == annotation::ADDITION );

            for( auto& v : a->get_array() ) {
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

         template< typename B, typename F >
         auto object_apply_last( B* b, const std::string& k, const F& f, const std::nullptr_t )
         {
            assert( b->t == annotation::ADDITION );

            auto& a = b->get_array();

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

         template< typename A, typename F >
         void container_apply_all( A* a, const F& f )
         {
            assert( a->t == annotation::ADDITION );

            for( auto& v : a->get_array() ) {
               if( v.t ) {
                  assert( v.t == annotation::REFERENCE );
                  throw std::runtime_error( "phase one across phase two reference" );
               }
               if( v.is_array() ) {
                  f( v );
                  continue;
               }
               if( v.is_object() ) {
                  f( v );
                  continue;
               }
               throw std::runtime_error( "wrong type" );
            }
         }

         template< typename A, typename F >
         void container_apply_all_all( A* a, const F& f )
         {
            assert( a->t == annotation::ADDITION );

            for( auto& v : a->get_array() ) {
               if( v.t ) {
                  assert( v.t == annotation::REFERENCE );
                  throw std::runtime_error( "phase one across phase two reference" );
               }
               if( v.is_array() ) {
                  for( auto& w : v.unsafe_get_array() ) {
                     f( w );
                  }
                  continue;
               }
               if( v.is_object() ) {
                  for( auto& p : v.unsafe_get_object() ) {
                     f( p.second );
                  }
                  continue;
               }
               throw std::runtime_error( "wrong type" );
            }
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
