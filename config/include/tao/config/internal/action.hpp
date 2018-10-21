// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_ACTION_HPP
#define TAO_CONFIG_INTERNAL_ACTION_HPP

#include "control.hpp"
#include "delete.hpp"
#include "grammar.hpp"
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
               assert( st.temp.type() == json::type::DISCARDED );

               st.temp.unsafe_assign_null();
            }
         };

         template<>
         struct action< rules::true_s >
         {
            template< typename Input >
            static void apply( const Input&, state& st )
            {
               assert( st.temp.type() == json::type::DISCARDED );

               st.temp.unsafe_assign_boolean( true );
            }
         };

         template<>
         struct action< rules::false_s >
         {
            template< typename Input >
            static void apply( const Input&, state& st )
            {
               assert( st.temp.type() == json::type::DISCARDED );

               st.temp.unsafe_assign_boolean( false );
            }
         };

         template<>
         struct action< rules::number_value >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( st.temp.type() == json::type::DISCARDED );

               st.temp.unsafe_assign_unsigned( std::stoul( in.string() ) );
            }
         };

         template<>
         struct action< rules::phase1_name >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.keys.empty() );

               st.keys.back().emplace_back( in.string() );
            }
         };

         template<>
         struct action< rules::phase1_index >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.keys.empty() );

               st.keys.back().emplace_back( std::stoul( in.string() ) );
            }
         };

         template<>
         struct action< rules::phase1_multi >
         {
            static void apply0( state& st )
            {
               assert( !st.keys.empty() );

               st.keys.back().emplace_back( token::MULTI );
            }
         };

         template<>
         struct action< rules::phase1_append >
         {
            static void apply0( state& st )
            {
               assert( !st.keys.empty() );

               st.keys.back().emplace_back( token::APPEND );
            }
         };

         template<>
         struct action< rules::phase2_name >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( st.temp.type() == json::type::DISCARDED );

               st.temp.unsafe_assign_string( in.string() );
            }
         };

         template<>
         struct action< rules::phase2_index >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( st.temp.type() == json::type::DISCARDED );

               st.temp.unsafe_assign_unsigned( std::stoul( in.string() ) );
            }
         };

         template<>
         struct action< rules::phase2_part >
         {
            static void apply0( state& st )
            {
               if( st.temp.type() != json::type::DISCARDED ) {
                  assert( !st.stack.empty() );

                  st.stack.back()->emplace_back( std::move( st.temp ) );
                  st.temp.discard();
               }
            }
         };

         template< typename T >
         void begin_container( state& st )
         {
            assert( !st.stack.empty() );
            assert( st.stack.back()->t == annotation::ADDITION );
            assert( st.stack.back()->is_array() );

            st.stack.emplace_back( &st.stack.back()->emplace_back( T{ 0 } ) );
         }

         template<>
         struct action< rules::curly_a >
         {
            static void apply0( state& st )
            {
               begin_container< json::empty_object_t >( st );
            }
         };

         template<>
         struct action< rules::curly_z >
         {
            static void apply0( state& st )
            {
               assert( st.stack.size() > 1 );

               st.stack.pop_back();
            }
         };

         template<>
         struct action< rules::square_a >
         {
            static void apply0( state& st )
            {
               begin_container< json::empty_array_t >( st );
            }
         };

         template<>
         struct action< rules::square_z >
         {
            static void apply0( state& st )
            {
               assert( st.stack.size() > 1 );

               st.stack.pop_back();
            }
         };

         template<>
         struct action< rules::value_part >
         {
            static void apply0( state& st )
            {
               if( st.temp.type() != json::type::DISCARDED ) {
                  assert( !st.stack.empty() );
                  assert( st.stack.back()->t == annotation::ADDITION );

                  st.stack.back()->emplace_back( std::move( st.temp ) );
                  st.temp.discard();
               }
            }
         };

         template<>
         struct action< rules::equals >
         {
            static void apply0( state& st )
            {
               assert( !st.stack.empty() );
               assert( !st.stack.back()->t );
               assert( st.stack.back()->type() == json::type::OBJECT );

               st.stack.emplace_back( &( resolve_and_pop_for_set( st ) = json::empty_array ) );
               st.stack.back()->t = annotation::ADDITION;
            }
         };

         template<>
         struct action< rules::plus_equals >
         {
            static void apply0( state& st )
            {
               assert( !st.stack.empty() );
               assert( !st.stack.back()->t );
               assert( st.stack.back()->is_object() );
               assert( st.stack.back()->type() == json::type::OBJECT );

               st.stack.emplace_back( &resolve_and_pop_for_set( st ) );
            }
         };

         template<>
         struct action< rules::element >
         {
            static void apply0( state& st )
            {
               assert( st.stack.size() > 1 );

               st.stack.pop_back();
            }
         };

         template<>
         struct action< rules::value_list >
         {
            static void apply0( state& st )
            {
               assert( st.stack.size() > 1 );

               st.stack.pop_back();
            }
         };

         template<>
         struct action< rules::phase1_content >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               st.temp = in.string();  // TODO: Escaping...
            }
         };

         template<>
         struct action< rules::env_value >
         {
            static void apply0( state& st )
            {
               assert( !st.stack.empty() );
               assert( st.stack.back()->t == annotation::ADDITION );
               assert( st.stack.back()->is_array() );

               const auto s = st.temp.get_string();
               st.temp = get_env( s );
            }
         };

         template<>
         struct action< rules::copy_value >
         {
            static void apply0( state& st )
            {
               assert( !st.stack.empty() );
               assert( st.stack.back()->t == annotation::ADDITION );
               assert( st.stack.back()->is_array() );
               assert( st.temp.type() == json::type::DISCARDED );

               auto& a = st.stack.back()->get_array();
               const auto& v = resolve_and_pop_for_get( st ).get_array();
               a.insert( a.end(), v.begin(), v.end() );
            }
         };

         template<>
         struct action< rules::read_value >
         {
            static void apply0( state& st )
            {
               assert( !st.stack.empty() );
               assert( st.stack.back()->t == annotation::ADDITION );
               assert( st.stack.back()->is_array() );

               const auto s = st.temp.get_string();
               st.temp = read_file( s );
            }
         };

         template<>
         struct action< rules::debug_value >
         {
            static void apply0( state& st )
            {
               assert( !st.stack.empty() );
               assert( st.stack.back()->t == annotation::ADDITION );
               assert( st.stack.back()->is_array() );
               assert( st.temp.type() == json::type::DISCARDED );

               std::ostringstream o;
               const auto& v = resolve_and_pop_for_get( st );
               to_stream( o, v );
               st.temp.unsafe_assign_string( o.str() );
            }
         };

         template<>
         struct action< rules::parse_value >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               pegtl::file_input i2( st.temp.get_string() );
               st.temp.discard();
               pegtl::parse_nested< rules::value, action, control >( in, i2, st );
            }
         };

         template<>
         struct action< rules::shell_value >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.stack.empty() );
               assert( st.stack.back()->t == annotation::ADDITION );
               assert( st.stack.back()->is_array() );

               const auto c = st.temp.get_string();
               st.temp.discard();
               const auto s = shell_popen( c );
               pegtl::memory_input i2( s, c );
               pegtl::parse_nested< rules::value, action, control >( in, i2, st );
            }
         };

         template<>
         struct action< rules::stderr_member >
         {
            static void apply0( state& st )
            {
               to_stream( std::cerr, resolve_and_pop_for_get( st ), 3 );
               std::cerr << std::endl;
            }
         };

         template<>
         struct action< rules::delete_member >
         {
            static void apply0( state& st )
            {
               assert( !st.keys.empty() );

               delete_and_pop( st );
            }
         };

         template<>
         struct action< rules::include_member >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               pegtl::file_input i2( st.temp.get_string() );
               st.temp.discard();
               pegtl::parse_nested< grammar, action, control >( in, i2, st );
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
