// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_RESOLVE_HPP
#define TAO_CONFIG_INTERNAL_RESOLVE_HPP

#include <cassert>

#include "kind.hpp"
#include "pointer.hpp"
#include "state.hpp"
#include "token.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline const value* resolve_for_get( const value* v, const pointer& p, const std::size_t b, const bool throws )
         {
            assert( v );

            for( std::size_t i = b; i < p.size(); ++i ) {
               const auto& k = p[ i ];
               if( !v->kind() ) {
                  switch( k.t ) {
                     case token::type::KEY:
                        v = &v->get_object().at( k.k );
                        break;
                     case token::type::INDEX:
                        v = &v->get_array().at( k.i );
                        break;
                     case token::type::APPEND:
                        throw "append not ok for copy";
                  }
                  continue;
               }
               switch( *v->kind() ) {
                  case kind::ADDITION:
                     for( auto j = v->get_array().rbegin(); j != v->get_array().rend(); ++j ) {
                        if( auto* w = resolve_for_get( &*j, p, i, false ) ) {
                           return w;
                        }
                     }
                     if( !throws ) {
                        return nullptr;
                     }
                     throw "nothing to copy found";
                  case kind::REFERENCE:
                     if( !throws ) {
                        return nullptr;
                     }
                     throw "attempt to copy from within phase two reference";
               }
            }
            return v;
         }

         inline value* resolve_for_set( value* v, const pointer& p, const std::size_t b, const bool throws )
         {
            assert( v );

            for( std::size_t i = b; i < p.size(); ++i ) {
               const auto& k = p[ i ];
               if( !v->kind() ) {
                  switch( k.t ) {
                     case token::type::KEY:
                        v->prepare_object();
                        v = &v->unsafe_get_object()[ k.k ];  // TODO: Use begin_addition() here correctly.
                        break;
                     case token::type::INDEX:
                        v->prepare_array();
                        v = &v->get_array().at( k.i );
                        break;
                     case token::type::APPEND: {
                        v->prepare_array();
                        auto& a = v->unsafe_get_array();
                        a.emplace_back( value() );
                        v = &a.back();  // TODO: Use begin_addition() here correctly.
                     }  break;
                  }
                  continue;
               }
               switch( *v->kind() ) {
                  case kind::ADDITION:
                     for( auto j = v->get_array().rbegin(); j != v->get_array().rend(); ++j ) {
                        if( auto *w = resolve_for_set( &*j, p, i, false ) ) {
                           return w;
                        }
                     }
                     if( !throws ) {
                        return nullptr;
                     }
                     throw "unable to find anywhere";
                  case kind::REFERENCE:
                     if( !throws ) {
                        return nullptr;
                     }
                     throw "attempt to insert or update within phase two reference";
               }
            }
            return v;
         }

         inline const value& resolve_for_get( const value& v, const pointer& p )
         {
            return *resolve_for_get( &v, p, 0, true );
         }

         inline const value& resolve_and_pop_for_get( state& st )
         {
            assert( !st.stack.empty() );
            assert( !st.keys.empty() );

            const auto& r = resolve_for_get( *st.stack.back(), st.keys.back() );
            st.keys.pop_back();
            return r;
         }

         inline value& resolve_for_set( value& v, const pointer& p )
         {
            return *resolve_for_set( &v, p, 0, true );
         }

         inline value& resolve_and_pop_for_set( state& st )
         {
            assert( !st.stack.empty() );
            assert( !st.keys.empty() );

            auto& r = resolve_for_set( *st.stack.back(), st.keys.back() );
            st.keys.pop_back();
            return r;
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
