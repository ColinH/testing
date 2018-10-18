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

         inline const value* resolve_for_get( const value* const v, const pointer& p, const std::size_t i )
         {
            if( i > p.size() ) {
               return nullptr;
            }
            assert( v );
            assert( v->t == annotation::ADDITION );

            if( i == p.size() ) {
               return v;
            }
            const auto& a = v->get_array();
            const auto s = a.size();

            for( std::size_t j = 0; j < s; ++j ) {
               const value& w = a[ s - j - 1 ];

               assert( w.t != annotation::ADDITION );

               if( w.t == annotation::REFERENCE ) {
                  throw std::runtime_error( "resolve for get across phase two reference" );
               }
               switch( p[ i ].t ) {
                  case token::KEY:
                     if ( auto *x = w.find( p[ i ].k ) ) {
                        return resolve_for_get( x, p, i + 1 );
                     }
                     break;
                  case token::INDEX:
                     if ( auto *x = array_find( w, p[ i ].i - array_size( a, s - j - 1 ) ) ) {
                        return resolve_for_get( x, p, i + 1 );
                     }
                     break;
                  case token::APPEND:
                     throw std::runtime_error( "resolve for get has append in key" );
               }
            }
            return nullptr;
         }

         inline value* resolve_for_set( value* const v, const pointer& p, const std::size_t i )
         {
            if( i > p.size() ) {
               return nullptr;
            }
            assert( v );
            assert( v->t == annotation::ADDITION );

            if( i == p.size() ) {
               return v;
            }
            auto& a = v->get_array();  // value_list

            if( p[ i ].t == token::APPEND ) {
               if( a.empty() ) {
                  a.emplace_back( json::empty_array );  // array value
               }
               auto& b = a.back().get_array();
               auto& c = b.emplace_back( json::empty_array );
               c.t = annotation::ADDITION;
               return resolve_for_set( &c, p, i + 1 );
            }
            const auto s = a.size();

            for( std::size_t j = 0; j < s; ++j ) {
               value& w = a[ s - j - 1 ];

               assert( w.t != annotation::ADDITION );

               if( w.t == annotation::REFERENCE ) {
                  throw std::runtime_error( "resolve for get across phase two reference" );
               }
               switch( p[ i ].t ) {
                  case token::KEY:
                     if ( auto* x = w.find( p[ i ].k ) ) {
                        return resolve_for_set( x, p, i + 1 );
                     }
                     break;
                  case token::INDEX:
                     if( auto* x = array_find( w, p[ i ].i - array_size( a, s - j - 1 ) ) ) {
                        return resolve_for_set( x, p, i + 1 );
                     }
                     break;
                  case token::APPEND:
                     assert( false );  // Handled above.
               }
            }
            if( p[ i ].t == token::INDEX ) {
               throw std::runtime_error( "resolve for get index not found" );
            }
            if( a.empty() ) {
               a.emplace_back( json::empty_object );  // object value
            }
            auto& b = a.back().get_object();
            auto d = b.emplace( p[ i ].k, json::empty_array );
            d.first->second.t = annotation::ADDITION;
            return resolve_for_set( &d.first->second, p, i + 1 );
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
