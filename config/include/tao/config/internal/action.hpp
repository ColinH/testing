// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_ACTION_HPP
#define TAO_CONFIG_INTERNAL_ACTION_HPP

#include <sstream>

#include "access.hpp"
#include "assign.hpp"
#include "control.hpp"
#include "erase.hpp"
#include "grammar.hpp"
#include "json.hpp"
#include "pegtl.hpp"
#include "state.hpp"
#include "system.hpp"
#include "to_stream.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Rule >
         struct action
            : public pegtl::nothing< Rule >
         {
         };

         template<>
         struct action< rules::null_s >
         {
            template< typename Input >
            static void apply( const Input&, state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back( entry::atom( json::null ) );
            }
         };

         template<>
         struct action< rules::true_s >
         {
            template< typename Input >
            static void apply( const Input&, state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back( entry::atom( true ) );
            }
         };

         template<>
         struct action< rules::false_s >
         {
            template< typename Input >
            static void apply( const Input&, state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back( entry::atom( false ) );
            }
         };

         template<>
         struct action< rules::string_content >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back( entry::atom( in.string() ) );  // TODO: Escaping...
            }
         };

         template<>
         struct action< rules::number_value >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back( entry::atom( std::stoul( in.string() ) ) );
            }
         };

         template<>
         struct action< rules::env_value >
         {
            static void apply0( state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back( entry::atom( get_env( st.str ) ) );
            }
         };

         template<>
         struct action< rules::read_value >
         {
            static void apply0( state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back( entry::atom( read_file( st.str ) ) );
            }
         };

         template<>
         struct action< rules::json_value >
         {
            static void apply0( state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back( entry::atom( json::parse_file( st.str ) ) );
            }
         };

         template<>
         struct action< rules::jaxn_value >
         {
            static void apply0( state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back( entry::atom( json::jaxn::parse_file( st.str ) ) );
            }
         };

         template<>
         struct action< rules::cbor_value >
         {
            static void apply0( state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back( entry::atom( json::cbor::parse_file( st.str ) ) );
            }
         };

         template<>
         struct action< rules::msgpack_value >
         {
            static void apply0( state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back( entry::atom( json::msgpack::parse_file( st.str ) ) );
            }
         };

         template<>
         struct action< rules::ubjson_value >
         {
            static void apply0( state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->emplace_back( entry::atom( json::ubjson::parse_file( st.str ) ) );
            }
         };

         template<>
         struct action< rules::debug_value >
         {
            static void apply0( state& st )
            {
               assert( !st.ostack.empty() );
               assert( !st.lstack.empty() );

               std::ostringstream oss;
               to_stream( oss, access( *st.ostack.back(), st.key ) );
               st.lstack.back()->emplace_back( entry::atom( oss.str() ) );

               st.key.clear();
            }
         };

         template<>
         struct action< rules::parse_value >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               pegtl::file_input i2( st.str );
               pegtl::parse_nested< rules::value, action, control >( in, i2, st );
            }
         };

         template<>
         struct action< rules::shell_value >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               pegtl::string_input i2( shell_popen( st.str ), st.str );
               pegtl::parse_nested< rules::value, action, control >( in, i2, st );
            }
         };

         template<>
         struct action< rules::copy_value >
         {
            static void apply0( state& st )
            {
               assert( !st.key.empty() );
               assert( !st.ostack.empty() );
               assert( !st.lstack.empty() );

               auto& d = *st.lstack.back();
               auto& s = access( *st.ostack.front(), st.key );

               d.insert( d.end(), s.begin(), s.end() );

               st.key.clear();
            }
         };

         template<>
         struct action< rules::phase1_name >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               st.key.emplace_back( in.string() );
            }
         };

         template<>
         struct action< rules::phase1_index >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               st.key.emplace_back( std::stoul( in.string() ) );
            }
         };

         template<>
         struct action< rules::phase1_multi >
         {
            static void apply0( state& st )
            {
               st.key.emplace_back( token::STAR );
            }
         };

         template<>
         struct action< rules::phase1_append >
         {
            static void apply0( state& st )
            {
               st.key.emplace_back( token::MINUS );
            }
         };

         template<>
         struct action< rules::phase1_content >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               st.str = in.string();  // TODO: Escaping...
            }
         };

         template<>
         struct action< rules::phase2_name >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( in.string() );
            }
         };

         template<>
         struct action< rules::phase2_index >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( std::stoul( in.string() ) );
            }
         };

         template<>
         struct action< rules::equals >
         {
            static void apply0( state& st )
            {
               assert( !st.ostack.empty() );

               st.lstack.emplace_back( &assign( *st.ostack.back(), st.key ) );
               st.lstack.back()->clear();

               st.key.clear();
            }
         };

         template<>
         struct action< rules::plus_equals >
         {
            static void apply0( state& st )
            {
               assert( !st.ostack.empty() );

               st.lstack.emplace_back( &assign( *st.ostack.back(), st.key ) );

               st.key.clear();
            }
         };

         template<>
         struct action< rules::key_member >
         {
            static void apply0( state& st )
            {
               assert( !st.ostack.empty() );
               assert( !st.lstack.empty() );

               st.lstack.pop_back();
            }
         };

         template<>
         struct action< rules::erase_member >
         {
            static void apply0( state& st )
            {
               assert( !st.key.empty() );
               assert( !st.ostack.empty() );

               erase( *st.ostack.back(), st.key );

               st.key.clear();
            }
         };

         template<>
         struct action< rules::stderr_member >
         {
            static void apply0( state& st )
            {
               assert( !st.key.empty() );
               assert( !st.ostack.empty() );

               to_stream( std::cerr, access( *st.ostack.back(), st.key ), 3 );
               std::cerr << std::endl;

               st.key.clear();
            }
         };

         template<>
         struct action< rules::include_member >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               pegtl::file_input i2( st.str );
               pegtl::parse_nested< grammar, action, control >( in, i2, st );
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
