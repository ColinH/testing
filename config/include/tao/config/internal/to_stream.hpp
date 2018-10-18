// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_TO_STREAM_HPP
#define TAO_CONFIG_INTERNAL_TO_STREAM_HPP

#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         // TODO: Prevent duplicating json::events::from_value!

         template< typename Consumer >
         void from_value( std::ostream& os, Consumer& consumer, const value& v )
         {
            switch( v.type() ) {
               case json::type::UNINITIALIZED:
                  throw std::logic_error( "unable to produce events from uninitialized values" );  // NOLINT

               case json::type::DISCARDED:
                  throw std::logic_error( "unable to produce events from discarded values" );  // NOLINT

               case json::type::DESTROYED:
                  throw std::logic_error( "unable to produce events from destroyed values" );  // NOLINT

               case json::type::NULL_:
                  consumer.null();
                  return;

               case json::type::BOOLEAN:
                  consumer.boolean( v.unsafe_get_boolean() );
                  return;

               case json::type::SIGNED:
                  consumer.number( v.unsafe_get_signed() );
                  return;

               case json::type::UNSIGNED:
                  consumer.number( v.unsafe_get_unsigned() );
                  return;

               case json::type::DOUBLE:
                  consumer.number( v.unsafe_get_double() );
                  return;

               case json::type::STRING:
                  consumer.string( v.unsafe_get_string() );
                  return;

               case json::type::STRING_VIEW:
                  consumer.string( v.unsafe_get_string_view() );
                  return;

               case json::type::BINARY:
                  consumer.binary( v.unsafe_get_binary() );
                  return;

               case json::type::BINARY_VIEW:
                  consumer.binary( v.unsafe_get_binary_view() );
                  return;

               case json::type::ARRAY: {
                  const auto& a = v.unsafe_get_array();
                  const auto s = a.size();
                  consumer.begin_array( s );
                  if( v.kind() ) {
                     switch( *v.kind() ) {
                        case kind::ADDITION:
                           os << "+[";
                           break;
                        case kind::REFERENCE:
                           os << "*[";
                           break;
                     }
                  }
                  for( const auto& e : a ) {
                     internal::from_value( os, consumer, e );
                     consumer.element();
                  }
                  consumer.end_array( s );
                  return;
               }

               case json::type::OBJECT: {
                  const auto& o = v.unsafe_get_object();
                  const auto s = o.size();
                  consumer.begin_object( s );
                  for( const auto& e : o ) {
                     consumer.key( e.first );
                     internal::from_value( os, consumer, e.second );
                     consumer.member();
                  }
                  consumer.end_object( s );
                  return;
               }

               case json::type::VALUE_PTR:
                  internal::from_value( os, consumer, *v.unsafe_get_value_ptr() );
                  return;

               case json::type::OPAQUE_PTR:
                  assert( false );  // None of these used here.
            }
            assert( false );
         }

         void to_stream( std::ostream& os, const value& v, const std::size_t indent )
         {
            json::jaxn::events::to_pretty_stream consumer( os, indent );
            from_value( os, consumer, v );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
