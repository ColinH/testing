// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_RESOLVE_HPP
#define TAO_CONFIG_INTERNAL_RESOLVE_HPP

#include <cassert>
#include <stdexcept>

#include "kind.hpp"
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
         template< typename T >
         void begin_container( state& st );

         // All resolve functions return a pointer to the array representing the value addition.

         inline const value* resolve_for_get( const value* v, const pointer& p, std::size_t i, const bool throws )
         {
            assert( v );
            assert( i < p.size() );
            assert( v->kind() != kind::ADDITION );

            if( v->kind() == kind::REFERENCE ) {
               // if( !throws ) {
               //    return nullptr;
               // }
               throw std::runtime_error( "resolve for get across phase two reference" );
            }
            switch( p[ i ].t ) {
               case token::type::KEY:
                  v = &v->get_object().at( p[ i ].k );
                  break;
               case token::type::INDEX:
                  v = &v->get_array().at( p[ i ].i );
                  break;
               case token::type::APPEND:
                  throw std::runtime_error( "resolve for get append" );
            }
            assert( v->kind() == kind::ADDITION );

            if( i + 1 == p.size() ) {
               return v;
            }
            for( auto j = v->get_array().rbegin(); j != v->get_array().rend(); ++j ) {
               if( auto* w = resolve_for_get( &*j, p, i + 1, false ) ) {
                  return w;
               }
            }
            if( !throws ) {
               return nullptr;
            }
            throw std::runtime_error( "resolve for get found nothing" );
         }

         inline value* resolve_for_set( value* v, const pointer& p, std::size_t i, const bool throws )
         {
            assert( v );
            assert( i < p.size() );
            assert( v->kind() != kind::ADDITION );

            if( v->kind() == kind::REFERENCE ) {
               throw std::runtime_error( "resolve for set across phase two reference" );
            }
            switch( p[ i ].t ) {
               case token::type::KEY: {
                  v->prepare_object();
                  auto& o = v->unsafe_get_object();
                  const auto j = o.emplace( p[ i ].k, json::empty_array );
                  if( j.second ) {
                     j.first->second.set_kind( kind::ADDITION );
                  }
                  else {
                     assert( j.first->second.kind() == kind::ADDITION );  // TODO: Redundant with the assert() below; temporary.
                  }
                  v = &j.first->second;
               }  break;
               case token::type::INDEX:
                  v->prepare_array();
                  v = &v->get_array().at( p[ i ].i );
                  break;
               case token::type::APPEND: {
                  v->prepare_array();
                  auto& a = v->unsafe_get_array();
                  v = &a.emplace_back( json::empty_array );
                  v->set_kind( kind::ADDITION );
               }  break;
            }
            assert( v->kind() == kind::ADDITION );

            if( i + 1 == p.size() ) {
               return v;
            }
            for( auto j = v->get_array().rbegin(); j != v->get_array().rend(); ++j ) {
               if( auto *w = resolve_for_set( &*j, p, i + 1, false ) ) {
                  return w;
               }
            }
            if( !throws ) {
               return nullptr;
            }
            auto& j = v->get_array().emplace_back();
            return resolve_for_set( &j, p, i + 1, true );
         }

         inline const value& resolve_for_get( const value& v, const pointer& p )
         {
            assert( !p.empty() );

            return *resolve_for_get( &v, p, 0, true );
         }

         inline const value& resolve_and_pop_for_get( state& st )
         {
            assert( !st.stack.empty() );
            assert( !st.keys.empty() );
            assert( !st.keys.back().empty() );

            const auto& r = resolve_for_get( *st.stack.back(), st.keys.back() );
            st.keys.pop_back();
            return r;
         }

         inline value& resolve_for_set( value& v, const pointer& p )
         {
            assert( !p.empty() );

            return *resolve_for_set( &v, p, 0, true );
         }

         inline value& resolve_and_pop_for_set( state& st )
         {
            assert( !st.stack.empty() );
            assert( !st.keys.empty() );
            assert( !st.keys.back().empty() );

            auto& r = resolve_for_set( *st.stack.back(), st.keys.back() );
            st.keys.pop_back();
            return r;
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
