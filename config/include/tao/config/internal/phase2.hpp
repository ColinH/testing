// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_PHASE2_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_HPP

#include <utility>
#include <vector>

#include "addition.hpp"
#include "resolve.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline token token_from_value( const value& v )
         {
            assert( !v.t );

            switch( v.type() ) {
               case json::type::STRING:
               case json::type::STRING_VIEW:
                  return token( v.as< std::string >() );
                  break;
               case json::type::SIGNED:
               case json::type::UNSIGNED:
                  return token( v.as< std::size_t >() );
                  break;
               default:
                  throw std::runtime_error( "invalid json type for reference" );
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

         class phase2_guard
         {
         public:
            explicit
            phase2_guard( const value& v )
               : m_v( v )
            {
               if( m_v.phase2_recursion_marker ) {
                  throw std::runtime_error( "recursion in phase 2 detected" );
               }
               m_v.phase2_recursion_marker = true;
            }

            ~phase2_guard()
            {
               m_v.phase2_recursion_marker = false;
            }

            phase2_guard( phase2_guard&& ) = delete;
            phase2_guard( const phase2_guard& ) = delete;

            void operator=( phase2_guard&& ) = delete;
            void operator=( const phase2_guard& ) = delete;

         private:
            const value& m_v;
         };

         template< template< typename... > class Traits >
         class phase2_impl
         {
         public:
            explicit
            phase2_impl( const value& root )
               : m_root( root )
            {
            }

            json::basic_value< Traits > phase2()
            {
               json::basic_value< Traits > r = addition( m_root );
               pointers( r, config::pointer() );
               return r;
            }

         private:
            void pointers( json::basic_value< Traits >& r, const config::pointer& p )
            {
               switch( r.type() ) {
                  case json::type::ARRAY:
                     for( std::size_t i = 0; i < r.unsafe_get_array().size(); ++i ) {
                        pointers( r[ i ], p + i );
                     }
                     break;
                  case json::type::OBJECT:
                     for( auto& i : r.unsafe_get_object() ) {
                        pointers( i.second, p + i.first );
                     }
                     break;
                  default:
                     break;
               }
               r.pointer = p;
            }

            json::basic_value< Traits > regular( const value& v )
            {
               const phase2_guard _( v );

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
                     for( auto& i : v.unsafe_get_array() ) {
                        t.emplace_back( addition( i ) );
                     }
                     break;
                  case json::type::OBJECT:
                     t.prepare_object();
                     for( auto& i : v.unsafe_get_object() ) {
                        t.emplace( i.first, addition( i.second ) );
                     }
                     break;
                  default:
                     std::cerr << json::to_string( v.type() ) << std::endl;
                     throw std::runtime_error( "invalid json type in phase two" );
               }
               //               t.position = v.position;  // TODO: Will we need this?
               return t;
            }

            json::basic_value< Traits > addition( const value& v )
            {
               const phase2_guard _( v );

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
               //               r.position = v.position;  // TODO: Let's see how often this is wrong...
               return r;
            }

         private:
            const value& m_root;
         };

         template< template< typename... > class Traits >
         json::basic_value< Traits > phase2( const value& v )
         {
            return phase2_impl< Traits >( v ).phase2();
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
