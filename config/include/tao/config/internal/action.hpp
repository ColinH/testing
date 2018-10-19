// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_ACTION_HPP
#define TAO_CONFIG_INTERNAL_ACTION_HPP

#include "control.hpp"
#include "delete.hpp"
#include "grammar.hpp"
#include "pegtl.hpp"
#include "state.hpp"
#include "utility.hpp"

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
            static void apply0( state& st )
            {
               assert( st.temp.type() == json::type::DISCARDED );

               st.temp.unsafe_assign_null();
            }
         };

         template<>
         struct action< rules::true_s >
         {
            static void apply0( state& st )
            {
               assert( st.temp.type() == json::type::DISCARDED );

               st.temp.unsafe_assign_boolean( true );
            }
         };

         template<>
         struct action< rules::false_s >
         {
            static void apply0( state& st )
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
         struct action< rules::phase1_append >
         {
            static void apply0( state& st )
            {
               assert( !st.keys.empty() );

               st.keys.back().emplace_back();
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
         struct action< rules::round_a >
         {
            static void apply0( state& st )
            {
               assert( !st.stack.empty() );
               assert( st.stack.back()->t );
               assert( st.stack.back()->is_array() );

               st.stack.emplace_back( &st.stack.back()->get_array().emplace_back( json::empty_array ) );
               st.stack.back()->t = annotation::REFERENCE;
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

         template<>
         struct action< rules::round_z >
         {
            static void apply0( state& st )
            {
               assert( st.stack.size() > 1 );

               st.stack.pop_back();
            }
         };

         template<>
         struct action< rules::curly_a >
         {
            static void apply0( state& st )
            {
               begin_container< json::empty_object_t >( st );
            }
         };

         template<>
         struct action< rules::key_member >
         {
            static void apply0( state& st )
            {
               if( st.temp.type() != json::type::DISCARDED ) {
                  assert( !st.temp.t );
                  assert( st.stack.size() > 1 );

                  resolve_and_pop_for_set( st ).emplace_back( std::move( st.temp ) );
               }
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
         struct action< rules::element_comma >
         {
            static void apply0( state& st )
            {
               if( st.temp.type() != json::type::DISCARDED ) {
                  assert( !st.temp.t );
                  assert( st.stack.size() > 1 );

                  st.stack.back()->emplace_back( std::move( st.temp ) );
                  st.temp.discard();
               }
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
         struct action< rules::value_plus >
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
         struct action< rules::delete_member >
         {
            static void apply0( state& st )
            {
               assert( !st.keys.empty() );

               delete_and_pop( st );
            }
         };

         template<>
         struct action< rules::filename_content >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               pegtl::file_input i2( in.string() );
               pegtl::parse_nested< grammar, action, control >( in, i2, st );
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
