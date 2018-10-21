// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_DELETE_HPP
#define TAO_CONFIG_INTERNAL_DELETE_HPP

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
         inline void delete_final( value* const v, const token& t )
         {
            switch( t.t ) {
               case token::NAME:
                  object_apply_all( v, [ &k = t.k ]( value& v ){ v.get_object().erase( k ); } );
                  break;
               case token::INDEX:
                  array_apply_one( v, t.i, []( auto& a, const std::size_t n ){ a.erase( a.begin() + n ); } );
                  break;
               case token::MULTI:
                  container_apply_all( v, []( value& v ){ if( v.is_array() ){ v.unsafe_get_array().clear(); } else if( v.is_object() ) { v.unsafe_get_object().clear(); } else { assert( false ); } } );
                  break;
               case token::APPEND:
                  array_apply_last( v, []( auto& a, const std::size_t n ){ a.erase( a.begin() + n ); } );
                  break;
            }
         }

         inline void delete_recursive( value* const v, const pointer& p, const std::size_t i )
         {
            assert( v );
            assert( v->t == annotation::ADDITION );
            assert( i < p.size() );

            if( i + 1 == p.size() ) {
               delete_final( v, p.back() );
               return;
            }
            switch( p[ i ].t ) {
               case token::NAME:
                  object_apply_all( v, [ i, &p ]( value& v ){ if( auto* x = v.find( p[ i ].k ) ) { delete_recursive( x, p, i + 1 ); } } );
                  break;
               case token::INDEX:
                  array_apply_one( v, p[ i ].i, [ i, &p ]( auto& a, const std::size_t n ){ delete_recursive( a.data() + n, p, i + 1 ); } );
                  break;
               case token::MULTI:
                  container_apply_all_all( v, [ i, &p ]( value& v ){ delete_recursive( &v, p, i + 1 ); } );
                  break;
               case token::APPEND:
                  array_apply_last( v, [ i, &p ]( auto& a, const std::size_t n ){ delete_recursive( a.data() + n, p, i + 1 ); } );
                  break;
            }
         }

         inline void delete_key( state& st )
         {
            assert( !st.stack.empty() );
            assert( ( st.stack.size() & 1 ) == 0 );
            assert( !st.key.empty() );

            delete_recursive( *( st.stack.end() - 2 ), st.key, 0 );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
