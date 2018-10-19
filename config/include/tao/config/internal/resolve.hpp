// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_RESOLVE_HPP
#define TAO_CONFIG_INTERNAL_RESOLVE_HPP

#include <cassert>
#include <stdexcept>

#include "pointer.hpp"
#include "state.hpp"
#include "token.hpp"
#include "utility.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         // All resolve functions return a pointer to the array representing the value addition.

         inline const value* resolve_for_get( const value* const v, const pointer& p, const std::size_t i )
         {
            assert( v );
            assert( v->t == annotation::ADDITION );
            assert( i <= p.size() );

            if( i == p.size() ) {
               return v;
            }
            auto& a = v->get_array();

            switch( p[ i ].t ) {
               case token::NAME:
                  return object_apply_last( a, p[ i ].k, [ i, &p ]( const value* v ){ return resolve_for_get( v, p, i + 1 ); } );
               case token::INDEX:
                  return array_apply( a, p[ i ].i, [ i, &p ]( auto& a, const std::size_t n ){ return resolve_for_get( a.data() + n, p, i + 1 ); } );
               case token::APPEND:
                  throw std::runtime_error( "resolve for get has append in key" );
            }
            return nullptr;
         }

         inline value* resolve_for_set( value* const v, const pointer& p, const std::size_t i );

         inline value* resolve_for_set_append( std::vector< value >& a, const pointer& p, const std::size_t i )
         {
            if( a.empty() ) {
               a.emplace_back( json::empty_array );  // TODO: Can this happen?
            }
            auto& b = a.back().get_array();
            auto& c = b.emplace_back( json::empty_array );
            c.t = annotation::ADDITION;
            return resolve_for_set( &c, p, i + 1 );
         }

         inline value* resolve_for_set_insert( std::vector< value >& a, const pointer& p, const std::size_t i )
         {
            if( a.empty() ) {
               a.emplace_back( json::empty_object );  // Can this happen?
            }
            auto& b = a.back().get_object();
            auto d = b.emplace( p[ i ].k, json::empty_array );
            d.first->second.t = annotation::ADDITION;
            return resolve_for_set( &d.first->second, p, i + 1 );
         }

         inline value* resolve_for_set( value* const v, const pointer& p, const std::size_t i )
         {
            assert( v );
            assert( v->t == annotation::ADDITION );
            assert( i <= p.size() );

            if( i == p.size() ) {
               return v;
            }
            auto& a = v->get_array();

            switch( p[ i ].t ) {
               case token::NAME:
                  if( auto* x = object_apply_last( a, p[ i ].k, [ i, &p ]( value* v ){ return resolve_for_set( v, p, i + 1 ); }, nullptr ) ) {
                     return x;
                  }
                  return resolve_for_set_insert( a, p, i );
               case token::INDEX:
                  return array_apply( a, p[ i ].i, [ i, &p ]( auto& a, const std::size_t n ){ return resolve_for_set( a.data() + n, p, i + 1 ); } );
               case token::APPEND:
                  return resolve_for_set_append( a, p, i );
            }
            assert( false );
         }

         inline const value& resolve_for_get( const value& v, const pointer& p )
         {
            assert( !p.empty() );

            if( const auto* w = resolve_for_get( &v, p, 0 ) ) {
               return *w;
            }
            throw std::runtime_error( "resolve for get failure" );
         }

         inline const value& resolve_and_pop_for_get( state& st )
         {
            assert( !st.stack.empty() );
            assert( !st.keys.empty() );
            assert( !st.keys.back().empty() );

            if( const auto* w = resolve_for_get( st.stack.front(), st.keys.back(), 0 ) ) {
               st.keys.pop_back();
               return *w;
            }
            throw std::runtime_error( "resolve and pop for get failure" );
         }

         inline value& resolve_for_set( value& v, const pointer& p )
         {
            assert( !p.empty() );

            if( auto* w = resolve_for_set( &v, p, 0 ) ) {
               return *w;
            }
            throw std::runtime_error( "resolve for set failure" );
         }

         inline value& resolve_and_pop_for_set( state& st )
         {
            assert( st.stack.size() > 1 );
            assert( ( st.stack.size() & 1 ) == 0 );
            assert( !st.keys.empty() );
            assert( !st.keys.back().empty() );

            if( auto* w = resolve_for_set( *( st.stack.end() - 2 ), st.keys.back(), 0 ) ) {
               st.keys.pop_back();
               return *w;
            }
            throw std::runtime_error( "these messages need a lot of improvement" );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
