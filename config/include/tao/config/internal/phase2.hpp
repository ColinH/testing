// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_PHASE2_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_HPP

#include <iostream>
#include "to_stream.hpp"

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
         inline void phase2_regular( const value& r, value& v );
         inline void phase2_addition( const value& r, value& v );
         inline void phase2_reference( const value& r, value& v );

         inline void phase2_regular( const value& r, value& v )
         {
            assert( !v.t );

            switch( v.type() ) {
               case json::type::ARRAY:
                  for( auto& i : v.unsafe_get_array() ) {
                     phase2_addition( r, i );
                  }
                  break;
               case json::type::OBJECT:
                  for( auto& i : v.unsafe_get_object() ) {
                     phase2_addition( r, i.second );
                  }
                  break;
               default:
                  break;
            }
         }

         inline void phase2_reference( const value& r, value& v )
         {
            assert( v.t == annotation::REFERENCE );

            pointer p;

            for( auto& i : v.get_array() ) {
               assert( i.t != annotation::ADDITION );

               if( i.t == annotation::REFERENCE ) {
                  phase2_reference( r, i );
                  p.emplace_back( token_from_value( value_addition( i.get_array() ) ) );
               }
               else {
                  p.emplace_back( token_from_value( i ) );
               }
            }
            v = resolve_for_get( r, p );  // TODO: Fix this.
         }

         inline void phase2_addition( const value& r, value& v )
         {
            assert( v.t == annotation::ADDITION );

            std::vector< value > t;

            for( auto& i : v.get_array() ) {
               assert( i.t != annotation::ADDITION );

               if( i.t == annotation::REFERENCE ) {
                  phase2_reference( r, i );
                  const auto& a = i.get_array();
                  t.insert( t.end(), a.begin(), a.end() );
               }
               else {
                  phase2_regular( r, i );
                  t.emplace_back( std::move( i ) );
               }
            }
            v = value_addition( t );
         }

         inline void phase2( value& v )
         {
            // TODO: Protect against cycles (infinite recursion).

            std::cout << "<<< START <<<" << std::endl;
            to_stream( std::cout, v, 3 );
            std::cout << ">>> START >>>" << std::endl;

            phase2_addition( v, v );

            std::cout << "<<< FINISH <<<" << std::endl;
            to_stream( std::cout, v, 3 );
            std::cout << ">>> FINISH >>>" << std::endl;
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
