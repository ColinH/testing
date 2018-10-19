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
            assert( v );
            assert( v->t == annotation::ADDITION );

            switch( t.t ) {
               case token::NAME:
                  object_apply_all( v->get_array(), [ &t ]( value& v ){ v.get_object().erase( t.k ); } );
                  break;
               case token::INDEX:
                  array_apply( v->get_array(), t.i, []( auto& a, const std::size_t n ){ a.erase( a.begin() + n ); } );
                  break;
               case token::APPEND:
                  throw std::runtime_error( "delete has append in key" );  // TODO: Or delete last element?
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
                  object_apply_all( v->get_array(), [ &p, i ]( value& v ){ if( auto* x = v.find( p[ i ].k ) ) { delete_recursive( x, p, i + 1 ); } } );
                  break;
               case token::INDEX:
                  array_apply( v->get_array(), p[ i ].i, [ i, &p ]( auto& a, const std::size_t n ){ delete_recursive( a.data() + n, p, i + 1 ); } );
                  break;
               case token::APPEND:
                  throw std::runtime_error( "delete has append in key" );  // TODO: Or can we assert() here? Check the grammar.
            }
         }

         inline void delete_and_pop( state& st )
         {
            assert( !st.stack.empty() );
            assert( !st.keys.empty() );
            assert( !st.keys.back().empty() );

            delete_recursive( st.stack.front(), st.keys.back(), 0 );  // TODO: Or start at *( st.stack.end() - 2 )?
            st.keys.pop_back();
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
