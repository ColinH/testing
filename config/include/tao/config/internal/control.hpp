// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_CONTROL_HPP
#define TAO_CONFIG_INTERNAL_CONTROL_HPP

#include "pegtl.hpp"
#include "resolve.hpp"
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
               st.key.clear();
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
         struct control< rules::phase2_key >
            : public pegtl::normal< rules::phase2_key >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               assert( !st.stack.empty() );
               assert( st.stack.back()->t );
               assert( st.stack.back()->is_array() );

               st.stack.emplace_back( &st.stack.back()->emplace_back( json::empty_array ) );
               st.stack.back()->t = annotation::REFERENCE;
               st.stack.back()->set_position( in.position() );
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( st.stack.size() > 1 );

               st.stack.pop_back();
            }
         };

         template<>
         struct control< rules::element >
            : public pegtl::normal< rules::element >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               assert( !st.stack.empty() );
               assert( !st.stack.back()->t );
               assert( st.stack.back()->type() == json::type::ARRAY );

               st.stack.emplace_back( &st.stack.back()->emplace_back( json::empty_array ) );
               st.stack.back()->t = annotation::ADDITION;
               st.stack.back()->set_position( in.position() );
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
