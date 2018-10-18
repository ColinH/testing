// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_UTILITY_HPP

#include <cassert>
#include <vector>

#include "pointer.hpp"
#include "resolve.hpp"
#include "state.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename T >
         void begin_container( state& st )
         {
            switch( st.stack.back()->type() ) {
               case json::type::ARRAY: {
                  assert( !st.stack.empty() );

                  auto& a = st.stack.back()->get_array();
                  auto& t = a.emplace_back( T{ 0 } );
                  st.stack.emplace_back( &t );
               }  break;
               case json::type::OBJECT:
                  st.stack.emplace_back( &( resolve_and_pop_for_set( st ) = T{ 0 } ) );  // TODO: Disentangle again?
                  break;
               default:
                  assert( false );
            }
         }

         inline void begin_addition( state& st )
         {
            begin_container< json::empty_array_t >( st );
            st.stack.back()->set_kind( kind::ADDITION );
         }

         inline pointer array_to_pointer( const std::vector< value >& v )
         {
            pointer result;

            for( const auto& i : v ) {
               //  assert( !i.kind );

               switch( i.type() ) {
                  case json::type::STRING:
                  case json::type::STRING_VIEW:
                     result.push_back( token( i.as< std::string >() ) );
                     break;
                  case json::type::SIGNED:
                  case json::type::UNSIGNED:
                     result.push_back( token( i.as< std::size_t >() ) );
                     break;
                  case json::type::NULL_:
                     result.push_back( token() );
                  default:
                     throw "TODO";
               }
            }
            return result;
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
