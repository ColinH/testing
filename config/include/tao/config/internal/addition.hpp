// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_ADDITION_HH
#define TAO_CONFIG_INTERNAL_ADDITION_HH

#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline json::null_t null_addition( const std::vector< value >& a )
         {
            for( const auto& e : a ) {
               assert( !e.t );

               switch( e.type() ) {
                  case json::type::NULL_:
                     break;
                  default:
                     throw std::runtime_error( "inconsistent json types for null addition" );
               }
            }
            return json::null;
         }

         inline bool boolean_addition( const std::vector< value >& a )
         {
            bool result = false;

            for( const auto& e : a ) {
               assert( !e.t );

               switch( e.type() ) {
                  case json::type::BOOLEAN:
                     result |= e.unsafe_get_boolean();
                     break;
                  default:
                     throw std::runtime_error( "inconsistent json types for boolean addition" );
               }
            }
            return result;
         }

         inline double double_addition( const std::vector< value >& a )
         {
            double result = 0.0;

            for( const auto& e : a ) {
               assert( !e.t );

               switch( e.type() ) {
                  case json::type::DOUBLE:
                     result += e.unsafe_get_double();
                     break;
                  default:
                     throw std::runtime_error( "inconsistent json types for double addition" );
               }
            }
            return result;
         }

         inline __int128_t integer_addition( const std::vector< value >& a )
         {
            __int128_t result = 0;

            for( const auto& e : a ) {
               assert( !e.t );

               switch( e.type() ) {
                  case json::type::SIGNED:
                     result += e.unsafe_get_signed();
                     break;
                  case json::type::UNSIGNED:
                     result += e.unsafe_get_unsigned();
                     break;
                  default:
                     throw std::runtime_error( "inconsistent json types for integer addition" );
               }
            }
            return result;
         }

         inline std::string string_addition( const std::vector< value >& a )
         {
            std::ostringstream oss;

            for( const auto& e : a ) {
               assert( !e.t );

               switch( e.type() ) {
                  case json::type::STRING:
                     oss << e.unsafe_get_string();
                     continue;
                  case json::type::STRING_VIEW:
                     oss << e.unsafe_get_string_view();
                     continue;
                  default:
                     throw std::runtime_error( "inconsistent json types for string addition" );
               }
            }
            return oss.str();
         }

         inline std::vector< std::byte > binary_addition( const std::vector< value >& a )
         {
            std::vector< std::byte > result;

            for( const auto& e : a ) {
               assert( !e.t );

               switch( e.type() ) {
                  case json::type::BINARY:
                     result.insert( result.end(), e.unsafe_get_binary().begin(), e.unsafe_get_binary().end() );
                     continue;
                  case json::type::BINARY_VIEW:
                     result.insert( result.end(), e.unsafe_get_binary_view().begin(), e.unsafe_get_binary_view().end() );
                     continue;
                  default:
                     throw std::runtime_error( "inconsistent json types for binary addition" );
               }
            }
            return result;
         }

         inline std::vector< value > array_addition( const std::vector< value >& a )
         {
            std::vector< value > result;

            for( const auto& e : a ) {
               assert( !e.t );

               switch( e.type() ) {
                  case json::type::ARRAY:
                     result.insert( result.end(), e.unsafe_get_array().begin(), e.unsafe_get_array().end() );
                     continue;
                  default:
                     throw std::runtime_error( "inconsistent json types for array addition" );
               }
            }
            return result;
         }

         inline std::map< std::string, value > object_addition( const std::vector< value >& a )
         {
            std::map< std::string, value > result;

            for( const auto& e : a ) {
               assert( !e.t );

               switch( e.type() ) {
                  case json::type::OBJECT:
                     for( const auto& i : e.unsafe_get_object() ) {
                        result.insert_or_assign( i.first, i.second );  // TODO: We can do better than this for performance, but not with the same simplicity?
                     }
                     break;
                  default:
                     throw std::runtime_error( "inconsistent json types for object addition" );
               }
            }
            return result;
         }

         inline value integer_addition_value( const std::vector< value >& a )
         {
            const auto t = integer_addition( a );
            return ( t < 0 ) ? value( std::int64_t( t ) ) : value( std::uint64_t( t ) );  // TODO: Check for overflow etc.
         }

         inline value value_addition( const std::vector< value >& a )
         {
            assert( !a.empty() );

            switch( a[ 0 ].type() ) {
               case json::type::NULL_:
                  return value( null_addition( a ) );
               case json::type::ARRAY:
                  return value( array_addition( a ) );
               case json::type::OBJECT:
                  return value( object_addition( a ) );
               case json::type::BOOLEAN:
                  return value( boolean_addition( a ) );
               case json::type::SIGNED:
               case json::type::UNSIGNED:
                  return integer_addition_value( a );
               case json::type::STRING:
               case json::type::STRING_VIEW:
                  return value( string_addition( a ) );
               case json::type::BINARY:
               case json::type::BINARY_VIEW:
                  return value( binary_addition( a ) );
               default:
                  throw std::runtime_error( "invalid json type for addition" );
            }
         }

         inline value value_addition( const value& v )
         {
            if( !v.t ) {
               return v;
            }
            assert( v.t == annotation::ADDITION );
            assert( v.is_array() );

            return value_addition( v.unsafe_get_array() );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
