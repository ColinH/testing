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

         template< template< typename... > class Traits >
         class phase2_impl
         {
         public:
            explicit
            phase2_impl( const value& root )
               : m_root( root )
            {
            }

            json::basic_value< Traits > regular( const value& v )
            {
               assert( !v.t );

               json::basic_value< Traits > t;

               switch( v.type() ) {
                  case json::type::NULL_:
                     t.unsafe_assign_null();
                     break;
                  case json::type::BOOLEAN:
                     t.unsafe_assign_boolean( v.unsafe_get_boolean() );
                     break;
                  case json::type::SIGNED:
                     t.unsafe_assign_signed( v.unsafe_get_signed() );
                     break;
                  case json::type::UNSIGNED:
                     t.unsafe_assign_unsigned( v.unsafe_get_unsigned() );
                     break;
                  case json::type::DOUBLE:
                     t.unsafe_assign_double( v.unsafe_get_double() );
                     break;
                  case json::type::STRING:
                     t.unsafe_assign_string( v.unsafe_get_string() );
                     break;
                  case json::type::BINARY:
                     t.unsafe_assign_binary( v.unsafe_get_binary() );
                     break;
                  case json::type::ARRAY:
                     t.prepare_array();
                     for( std::size_t i = 0; i < v.unsafe_get_array().size(); ++i ) {
                        m_stack.push_back( i );
                        t.emplace_back( addition( v.unsafe_get_array()[ i ] ) );
                        m_stack.pop_back();
                     }
                     break;
                  case json::type::OBJECT:
                     t.prepare_object();
                     for( auto& i : v.unsafe_get_object() ) {
                        m_stack.push_back( i.first );
                        t.emplace( i.first, addition( i.second ) );
                        m_stack.pop_back();
                     }
                     break;
                  default:
                     std::cerr << json::to_string( v.type() ) << std::endl;
                     throw std::runtime_error( "invalid json type in phase two" );
               }
               t.position = v.position;  // TODO: Will we need this?
               return t;
            }

            json::basic_value< Traits > addition( const value& v )
            {
               assert( v.t == annotation::ADDITION );

               std::vector< json::basic_value< Traits > > t;

               for( const auto& i : v.get_array() ) {
                  assert( i.t != annotation::ADDITION );

                  if( i.t == annotation::REFERENCE ) {
                     const auto a = phase2_reference( m_root, i );
                     assert( a.t == annotation::ADDITION );
                     t.emplace_back( addition( a ) );  // Assume associativity.
                  }
                  else {
                     t.emplace_back( regular( i ) );
                  }
               }
               auto r = value_addition( t );
               r.pointer = m_stack;
               r.position = v.position;  // TODO: Let's see how often this is wrong...
               return r;
            }

         private:
            config::pointer m_stack;

            const value& m_root;
         };

         template< template< typename... > class Traits >
         json::basic_value< Traits > phase2( const value& v )
         {
            // TODO: Protect against cycles (infinite recursion) -- set? counter? annotation?

            return phase2_impl< Traits >( v ).addition( v );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
