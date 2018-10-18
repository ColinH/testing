// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_CONTROL_HPP
#define TAO_CONFIG_INTERNAL_CONTROL_HPP

#include "pegtl.hpp"
#include "utility.hpp"

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

         template< typename Rule >
         struct key_control
            : public pegtl::normal< Rule >
         {
            template< typename Input >
            static void start( const Input&, state& st )
            {
               st.keys.push_back( pointer() );
            }

            template< typename Input >
            static void failure( const Input&, state& st )
            {
               st.keys.pop_back();
            }
         };

         template<>
         struct control< rules::phase1_key >
            : public key_control< rules::phase1_key >
         {
         };

         template<>
         struct control< rules::member_key >
            : public key_control< rules::member_key >
         {
         };

         template<>
         struct control< rules::value_list >
            : public pegtl::normal< rules::value_list >
         {
            template< typename Input >
            static void start( const Input&, state& st )
            {
               begin_addition( st );
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
