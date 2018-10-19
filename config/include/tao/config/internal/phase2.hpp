// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_PHASE2_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_HPP

#include <utility>
#include <vector>

#include "addition.hpp"
#include "resolve.hpp"
#include "utility.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline value phase2_regular( const value& r, const value& v );
         inline value phase2_addition( const value& r, const value& v );
         inline value phase2_reference( const value& r, const value& v );

         inline value phase2_regular( const value& r, const value& v )
         {
            assert( !v.t );

            switch( v.type() ) {
               case json::type::ARRAY: {
                  value t = json::empty_array;
                  for( auto& i : v.unsafe_get_array() ) {
                     t.emplace_back( phase2_addition( r, i ) );
                  }
                  return t;
               }  break;
               case json::type::OBJECT: {
                  value t = json::empty_object;
                  for( auto& i : v.unsafe_get_object() ) {
                     t.emplace( i.first, phase2_addition( r, i.second ) );
                  }
                  return t;
               }  break;
               default:
                  return v;
            }
         }

         inline value phase2_reference( const value& r, const value& v )
         {
            assert( v.t == annotation::REFERENCE );
            assert( v.is_array() );

            pointer p;

            for( auto& i : v.get_array() ) {
               assert( i.t != annotation::ADDITION );

               if( i.t == annotation::REFERENCE ) {
                  p.emplace_back( token_from_value( value_addition( phase2_reference( r, i ) ) ) );
               }
               else {
                  p.emplace_back( token_from_value( i ) );
               }
            }
            return resolve_for_get( r, p );
         }

         inline value phase2_addition( const value& r, const value& v )
         {
            assert( v.t == annotation::ADDITION );

            std::vector< value > t;

            for( const auto& i : v.get_array() ) {
               assert( i.t != annotation::ADDITION );

               if( i.t == annotation::REFERENCE ) {
                  const auto a = phase2_reference( r, i );
                  assert( a.t == annotation::ADDITION );
                  const auto& b = a.get_array();
                  t.insert( t.end(), b.begin(), b.end() );
               }
               else {
                  t.emplace_back( phase2_regular( r, i ) );
               }
            }
            return value_addition( t );
         }

         value phase2( const value& v )
         {
            // TODO: Protect against cycles (infinite recursion) -- set? counter? annotation?

            return phase2_addition( v, v );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
