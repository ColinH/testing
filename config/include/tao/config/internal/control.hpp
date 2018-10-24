// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_CONTROL_HPP
#define TAO_CONFIG_INTERNAL_CONTROL_HPP

#include "binary_state.hpp"
#include "changes.hpp"
#include "grammar.hpp"
#include "json.hpp"
#include "pegtl.hpp"
#include "state.hpp"
#include "string_state.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Rule >
         struct control
            : public pegtl::normal< Rule >
         {
         };

         template<>
         struct control< rules::binary_choice >
            : change_state_and_action< rules::binary_choice, binary_state, json::jaxn::internal::bunescape_action >
         {
         };

         template<>
         struct control< rules::string_choice >
            : change_state_and_action< rules::string_choice, string_state, json::jaxn::internal::unescape_action >
         {
         };

         template<>
         struct control< rules::phase2_key >
            : public pegtl::normal< rules::phase2_key >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               if( st.rstack.empty() ) {
                  st.rstack.emplace_back( &st.lstack.back()->emplace_back( entry::indirect( in ) ).get_indirect() );
               }
               else {
                  st.rstack.emplace_back( &st.rstack.back()->emplace_back( json::empty_array ) );
               }
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( !st.rstack.empty() );

               st.rstack.pop_back();
            }
         };

         template<>
         struct control< rules::element >
            : public pegtl::normal< rules::element >
         {
            template< typename Input >
            static void start( const Input&, state& st )
            {
               assert( st.rstack.empty() );
               assert( !st.astack.empty() );

               st.lstack.emplace_back( &st.astack.back()->emplace_back() );
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( st.rstack.empty() );
               assert( !st.astack.empty() );
               assert( !st.lstack.empty() );

               st.lstack.pop_back();
            }
         };

         template<>
         struct control< rules::element_list >
            : public pegtl::normal< rules::element_list >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.astack.emplace_back( &st.lstack.back()->emplace_back( entry::array( in ) ).get_array() );
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( !st.astack.empty() );

               st.astack.pop_back();
            }
         };

         template<>
         struct control< rules::member_list >
            : public pegtl::normal< rules::member_list >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.ostack.emplace_back( &st.lstack.back()->emplace_back( entry::object( in ) ).get_object() );
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( !st.ostack.empty() );

               st.ostack.pop_back();
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
