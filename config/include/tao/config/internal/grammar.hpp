// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_GRAMMAR_HPP
#define TAO_CONFIG_INTERNAL_GRAMMAR_HPP

#include "pegtl.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         namespace rules
         {
            // NOTE: Some rules are defined multiple times with different names in
            // order to provide anchor points for different PEGTL actions without
            // using what we call "switching style" in the PEGTL documentation.

            namespace jaxn = ::tao::json::jaxn::internal::rules;

            using ws1 = jaxn::ws;  // Also handles comments.

            struct wss : pegtl::star< ws1 > {};
            struct wsp : pegtl::plus< ws1 > {};

            struct at : pegtl::one< '@' > {};  // Phase 1
            struct dot : pegtl::one< '.' > {};
            struct plus : pegtl::one< '+' > {};
            struct minus : pegtl::one< '-' > {};
            struct comma : pegtl::one< ',' > {};  // Optional
            struct equals : pegtl::one< ':', '=' > {};
            struct plus_equals : pegtl::string< '+', '=' > {};

            struct quote_1 : pegtl::one< '\'' > {};
            struct quote_2 : pegtl::one< '"' > {};
            struct curly_a : pegtl::one< '{' > {};
            struct curly_z : pegtl::one< '}' > {};
            struct round_a : pegtl::one< '(' > {};
            struct round_z : pegtl::one< ')' > {};
            struct square_a : pegtl::one< '[' > {};
            struct square_z : pegtl::one< ']' > {};

            struct null_s : pegtl::string< 'n', 'u', 'l', 'l' > {};
            struct true_s : pegtl::string< 't', 'r', 'u', 'e' > {};
            struct false_s : pegtl::string< 'f', 'a', 'l', 's', 'e' > {};

            struct copy_s : pegtl::string< 'c', 'o', 'p', 'y' > {};
            struct debug_s : pegtl::string< 'd', 'e', 'b', 'u', 'g' > {};
            struct stderr_s : pegtl::string< 's', 't', 'd', 'e', 'r', 'r' > {};
            struct stdout_s : pegtl::string< 's', 't', 'd', 'o', 'u', 't' > {};
            struct delete_s : pegtl::string< 'd', 'e', 'l', 'e', 't', 'e' > {};
            struct include_s : pegtl::string< 'i', 'n', 'c', 'l', 'u', 'd', 'e' > {};

            struct array;
            struct object;
            struct reference;
            struct value_list;
            struct value_part;

            struct identifier : pegtl::identifier {};  // TODO: More?

            struct member_comma : pegtl::opt< comma, wss > {};
            struct element_comma : pegtl::opt< comma, wss > {};

            struct phase1_name : identifier {};
            struct phase1_index : pegtl::plus< pegtl::digit > {};
            struct phase1_append : minus {};
            struct phase1_part : pegtl::sor< phase1_name, phase1_index, phase1_append > {};

            struct phase2_name : identifier {};
            struct phase2_index : pegtl::plus< pegtl::digit > {};
            struct phase2_part : pegtl::sor< reference, phase2_name, phase2_index > {};

            struct phase1_key : pegtl::list_must< phase1_part, dot > {};
            struct member_key : pegtl::seq< phase1_name, pegtl::star_must< dot, phase1_part > > {};
            struct phase2_key : pegtl::list_must< phase2_part, dot > {};

            struct reference : pegtl::if_must< round_a, phase2_key, round_z > {};

            struct number_value : pegtl::plus< pegtl::digit > {};

            struct copy_value : pegtl::if_must< copy_s, wsp, phase1_key > {};
            struct debug_value : pegtl::if_must< debug_s, wsp, phase1_key > {};
            struct at_value : pegtl::if_must< at, pegtl::sor< copy_value, debug_value > > {};

            struct value_part : pegtl::sor< null_s, true_s, false_s, array, object, reference, at_value, number_value > {};  // TODO: All the rest (binary, strings, proper numbers).

            struct value_list : pegtl::list< value_part, plus, ws1 > {};
            struct value_plus : pegtl::list< value_part, plus, ws1 > {};
            struct plus_member : pegtl::if_must< plus_equals, wss, value_plus > {};
            struct equal_member : pegtl::if_must< equals, wss, value_list > {};
            struct key_member : pegtl::if_must< member_key, wss, pegtl::sor< plus_member, equal_member > > {};

            struct filename_content : pegtl::star< pegtl::not_one< '"' > > {};  // TODO: Escaping?
            struct include_filename : pegtl::if_must< quote_2, filename_content, quote_2 > {};

            struct stderr_member: pegtl::if_must< stderr_s, wsp, phase1_key > {};
            struct stdout_member: pegtl::if_must< stdout_s, wsp, phase1_key > {};
            struct delete_member : pegtl::if_must< delete_s, wsp, phase1_key > {};
            struct include_member : pegtl::if_must< include_s, wsp, include_filename > {};
            struct at_member : pegtl::if_must< at, pegtl::sor< include_member, delete_member, stderr_member, stdout_member > > {};

            struct member : pegtl::sor< at_member, key_member > {};

            template< typename U > struct member_list_impl : pegtl::until< U, member, wss, member_comma > {};

            struct element_list : pegtl::until< square_z, value_list, wss, element_comma > {};
            struct member_list : member_list_impl< curly_z > {};
            struct grammar_list : member_list_impl< pegtl::eof > {};

            struct array : pegtl::if_must< square_a, wss, element_list > {};
            struct object : pegtl::if_must< curly_a, wss, member_list > {};

            struct grammar : pegtl::must< wss, grammar_list > {};

         }  // namespace rules

         using grammar = rules::grammar;

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
