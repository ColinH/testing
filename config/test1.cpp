// Copyright (c) 2018 Dr. Colin Hirsch

#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <tao/json.hpp>

#include <tao/json/contrib/position.hpp>
#include <tao/json/contrib/traits.hpp>

namespace tao
{
   namespace pegtl = ::tao::json_pegtl;

   namespace config
   {
      enum node_kind
      {
         NORMAL,
         ADDITION,
         REFERENCE
      };

      struct annotation
      {
         json::pointer pointer;
         json::position position;

         node_kind kind = node_kind::NORMAL;
      };

      template< typename T >
      struct traits
         : public json::traits< T >
      {
      };

      template<>
      struct traits< void >
         : public json::traits< void >
      {
         template< typename Value >
         using public_base = annotation;
      };

      using value = json::basic_value< traits >;

      struct token
      {
         enum type
         {
            KEY,
            INDEX,
            APPEND
         };

         token()
            : t( type::APPEND )
         {
         }

         token( const std::size_t index )
            : t( type::INDEX ),
              i( index )
         {
         }

         token( const std::string& key )
            : t( type::KEY ),
              k( key )
         {
         }

         type t;
         std::size_t i;
         std::string k;
      };

      using pointer = std::vector< token >;

      namespace internal
      {
         std::string concat_string( const std::vector< value >& a )
         {
            std::ostringstream oss;
            for( const auto& e : a ) {
               switch( e.type() ) {
                  case json::type::STRING:
                     oss << e.unsafe_get_string();
                     continue;
                  case json::type::STRING_VIEW:
                     oss << e.unsafe_get_string_view();
                     continue;
                  default:
                     throw "something";
               }
            }
            return oss.str();
         }

         std::vector< std::byte > concat_binary( const std::vector< value >& a )
         {
            std::vector< std::byte > result;
            for( const auto& e : a ) {
               switch( e.type() ) {
                  case json::type::BINARY:
                     result.insert( result.end(), e.unsafe_get_binary().begin(), e.unsafe_get_binary().end() );
                     continue;
                  case json::type::BINARY_VIEW:
                     result.insert( result.end(), e.unsafe_get_binary_view().begin(), e.unsafe_get_binary_view().end() );
                     continue;
                  default:
                     throw "something";
               }
            }
            return result;
         }

         std::string concat_string( const value& v )
         {
            switch( v.type() ) {
               case json::type::STRING:
                  return v.unsafe_get_string();
               case json::type::STRING_VIEW:
                  return std::string( v.unsafe_get_string_view().data(), v.unsafe_get_string_view().size() );
               case json::type::ARRAY:
                  return concat_string( v.unsafe_get_array() );
               default:
                  throw "something";
            }
         }

         std::vector< value > concat_array( const std::vector< value >& a )
         {
            std::vector< value > result;
            for( const auto& e : a ) {
               switch( e.type() ) {
                  case json::type::ARRAY:
                     result.insert( result.end(), e.unsafe_get_array().begin(), e.unsafe_get_array().end() );
                     continue;
                  default:
                     throw "something";
               }
            }
            return result;
         }

         value concat( const value& v )
         {
            const auto& a = v.get_array();
            if( a.empty() ) {
               return value( json::null );
            }
            switch( a[ 0 ].type() ) {
               case json::type::ARRAY:
                  return value( concat_array( a ) );
               case json::type::STRING:
               case json::type::STRING_VIEW:
                  return value( concat_string( a ) );
               case json::type::BINARY:
               case json::type::BINARY_VIEW:
                  return value( concat_binary( a ) );
               default:
                  throw "something";
            }
         }

         // PHASE ONE

         namespace rules
         {
            namespace jaxn = ::tao::json::jaxn::internal::rules;

            using ws1 = jaxn::ws;  // NOTE: Also handles comments.

            struct wss : pegtl::star< ws1 > {};
            struct wsp : pegtl::plus< ws1 > {};

            struct dot : pegtl::one< '.' > {};
            struct plus : pegtl::one< '+' > {};
            struct minus : pegtl::one< '-' > {};
            struct comma : pegtl::one< ',' > {};  // Optional
            struct dollar : pegtl::one< '$' > {};  // Binary
            struct percent : pegtl::one< '%' > {};  // Phase One
            struct equals : pegtl::one< ':', '=' > {};

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
            struct delete_s : pegtl::string< 'd', 'e', 'l', 'e', 't', 'e' > {};
            struct include_s : pegtl::string< 'i', 'n', 'c', 'l', 'u', 'd', 'e' > {};

            struct identifier : pegtl::identifier {};  // TODO: Allow more here?

            struct array;
            struct object;
            struct reference;

            struct filename_content : pegtl::star< pegtl::not_one< '"' > > {};

            struct include_filename : pegtl::if_must< quote_2, filename_content, quote_2 > {};

            struct number_value : pegtl::plus< pegtl::digit > {};

            struct reference_ident : identifier {};

            struct reference_part : pegtl::sor< reference_ident, number_value, reference > {};

            struct reference : pegtl::if_must< round_a, pegtl::list< reference_part, dot >, round_z > {};

            struct percent_copy;

            struct value_part_percent : pegtl::if_must< percent, percent_copy > {};

            struct value_part : pegtl::sor< null_s, true_s, false_s, array, object, reference, number_value, value_part_percent > {};  // TODO: string, binary, more numbers

            struct number_key : pegtl::plus< pegtl::digit > {};

            struct append_key : minus {};

            struct percent_ident : identifier {};

            struct percent_part : pegtl::sor< append_key, percent_ident, number_key > {};  // TODO: Also quoted key parts.

            struct percent_key : pegtl::list< percent_part, dot > {};

            struct member_ident : identifier {};

            struct member_part : pegtl::sor< member_ident > {};  // TODO: Also quoted key parts.

            struct member_key : pegtl::seq< member_part, pegtl::star_must< dot, percent_part > > {};

            struct value;  //

            struct opt_member : pegtl::opt< comma, wss > {};

            struct opt_element : pegtl::opt< comma, wss > {};

            struct percent_copy : pegtl::if_must< copy_s, wss, percent_key, wss > {};

            struct percent_delete : pegtl::if_must< delete_s, wss, percent_key, wss > {};

            struct percent_include : pegtl::seq< include_s, wss, include_filename, wss > {};

            struct member_normal : pegtl::if_must< member_key, wss, equals, wss, value, wss > {};

            struct member_percent : pegtl::if_must< percent, pegtl::sor< percent_delete, percent_include > > {};

            struct member : pegtl::sor< member_percent, member_normal > {};

            struct member_list : pegtl::star< member, opt_member > {};

            struct object : pegtl::if_must< curly_a, wss, member_list, curly_z, wss > {};

            struct element_list : pegtl::until< square_z, value, wss, opt_element > {};

            struct array : pegtl::if_must< square_a, wss, element_list, wss > {};

            struct value_percent : pegtl::seq< percent, percent_include > {};

            struct value_addition : pegtl::list< value_part, plus, ws1 > {};

            struct value : pegtl::sor< value_percent, value_addition > {};

            struct grammar : pegtl::must< wss, member_list, pegtl::eof > {};

         }  // namespace rules

         struct grammar
            : public rules::grammar
         {
         };

         struct state
         {
            state()
               : result( json::empty_object )
            {
               temp.discard();
               stack.emplace_back( &result );
            }

            value temp;
            value result;

            std::vector< value* > stack;
            std::vector< pointer > keys;
         };

         const value* resolve_for_get( const value* v, const pointer& p, const std::size_t b = 0, const bool throws = true )
         {
            assert( v );

            for( std::size_t i = b; i < p.size(); ++i ) {
               const auto& k = p[ i ];
               switch( v->kind ) {
                  case node_kind::NORMAL:
                     switch( k.t ) {
                        case token::type::KEY:
                           v = &v->get_object().at( k.k );
                           break;
                        case token::type::INDEX:
                           v = &v->get_array().at( k.i );
                           break;
                        case token::type::APPEND:
                           throw "append not ok for copy";
                     }
                     break;
                  case node_kind::ADDITION:
                     for( auto j = v->get_array().rbegin(); j != v->get_array().rend(); ++j ) {
                        if( auto* w = resolve_for_get( &*j, p, i, false ) ) {
                           return w;
                        }
                     }
                     if( !throws ) {
                        return nullptr;
                     }
                     throw "nothing to copy found";
                  case node_kind::REFERENCE:
                     if( !throws ) {
                        return nullptr;
                     }
                     throw "attempt to copy from within phase two reference";
               }
            }
            return v;
         }

         value* resolve_for_set( value* v, const pointer& p, const std::size_t b = 0, const bool throws = true )
         {
            assert( v );

            for( std::size_t i = b; i < p.size(); ++i ) {
               const auto& k = p[ i ];
               switch( v->kind ) {
                  case node_kind::NORMAL:
                     switch( k.t ) {
                        case token::type::KEY:
                           v->prepare_object();
                           v = &v->unsafe_get_object()[ k.k ];
                           break;
                        case token::type::INDEX:
                           v->prepare_array();
                           v = &v->get_array().at( k.i );
                           break;
                        case token::type::APPEND: {
                           v->prepare_array();
                           auto& a = v->unsafe_get_array();
                           a.emplace_back( value() );
                           v = &a.back();
                        }  break;
                     }
                     break;
                  case node_kind::ADDITION:
                     for( auto j = v->get_array().rbegin(); j != v->get_array().rend(); ++j ) {
                        if( auto *w = resolve_for_set( &*j, p, i, false ) ) {
                           return w;
                        }
                     }
                     if( !throws ) {
                        return nullptr;
                     }
                     throw "unable to find anywhere";
                  case node_kind::REFERENCE:
                     if( !throws ) {
                        return nullptr;
                     }
                     throw "attempt to insert or update within phase two reference";
               }
            }
            return v;
         }

         template< typename Rule >
         struct control
            : public pegtl::normal< Rule >
         {
         };

         template<>
         struct control< rules::percent_key >
            : public pegtl::normal< rules::percent_key >
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
         struct control< rules::member_key >
            : public pegtl::normal< rules::member_key >
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

         template< typename T >
         void begin_container( state& st )
         {
            switch( st.stack.back()->type() ) {
               case json::type::ARRAY: {
                  assert( !st.stack.empty() );

                  auto& a = st.stack.back()->get_array();
                  auto& t = a.emplace_back( T{ 0 } );
                  st.stack.emplace_back( &t );
               }  break;
               case json::type::OBJECT: {
                  assert( !st.keys.empty() );
                  assert( !st.stack.empty() );

                  auto& t = *resolve_for_set( st.stack.back(), st.keys.back() );
                  t = T{ 0 };
                  st.stack.emplace_back( &t );

                  st.keys.pop_back();
               }  break;
               default:
                  assert( false );
            }
         }

         template<>
         struct control< rules::value_addition >
            : public pegtl::normal< rules::value_addition >
         {
            template< typename Input >
            static void start( const Input&, state& st )
            {
               begin_container< json::empty_array_t >( st );
               st.stack.back()->kind = node_kind::ADDITION;
            }
         };

         template< typename Rule >
         struct action
            : public pegtl::nothing< Rule >
         {
         };

         template<>
         struct action< rules::value_part >
         {
            static void apply0( state& st )
            {
               if( st.temp.type() != json::type::DISCARDED ) {
                  st.stack.back()->emplace_back( std::move( st.temp ) );
                  st.temp.discard();
               }
            }
         };

         template<> struct action< rules::value_addition >
         {
            static void apply0( state& st )
            {
               assert( st.stack.size() > 1 );
               st.stack.pop_back();
            }
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
         struct action< rules::curly_a >
         {
            static void apply0( state& st )
            {
               begin_container< json::empty_object_t >( st );
            }
         };

         template<>
         struct action< rules::member_normal >
         {
            static void apply0( state& st )
            {
               if( st.temp.type() != json::type::DISCARDED ) {
                  assert( !st.keys.empty() );
                  assert( !st.keys.back().empty() );
                  assert( !st.stack.empty() );

                  *resolve_for_set( st.stack.back(), st.keys.back() ) = std::move( st.temp );

                  st.temp.discard();
                  st.keys.pop_back();
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
         struct action< rules::round_a >
         {
            static void apply0( state& st )
            {
               begin_container< json::empty_array_t >( st );
               st.stack.back()->kind = node_kind::REFERENCE;
            }
         };

         template<>
         struct action< rules::reference_part >
         {
            static void apply0( state& st )
            {
               if( st.temp.type() != json::type::DISCARDED ) {
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
         struct action< rules::square_a >
         {
            static void apply0( state& st )
            {
               begin_container< json::empty_array_t >( st );
            }
         };

         template<>
         struct action< rules::opt_element >
         {
            static void apply0( state& st )
            {
               if( st.temp.type() != json::type::DISCARDED ) {
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
         struct action< rules::member_ident >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.keys.empty() );

               st.keys.back().push_back( token( in.string() ) );
            }
         };

         template<>
         struct action< rules::percent_ident >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.keys.empty() );

               st.keys.back().push_back( token( in.string() ) );
            }
         };

         template<>
         struct action< rules::reference_ident >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( st.temp.type() == json::type::DISCARDED );

               st.temp.unsafe_assign_string( in.string() );
            }
         };

         template<>
         struct action< rules::append_key >
         {
            static void apply0( state& st )
            {
               assert( !st.keys.empty() );

               st.keys.back().push_back( token() );
            }
         };

         template<>
         struct action< rules::number_key >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.keys.empty() );

               st.keys.back().push_back( token( std::stoul( in.string() ) ) );
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

         template<>
         struct action< rules::percent_copy >
         {
            static void apply0( state& st )
            {
               assert( st.temp.type() == json::type::DISCARDED );
               assert( !st.keys.empty() );
               assert( !st.stack.empty() );

               st.temp = *resolve_for_get( st.stack.front(), st.keys.back() );

               st.keys.pop_back();
            }
         };

         // PHASE TWO

         void resolve_references( const value& r, value& v );

         void resolve_references_normal( const value& r, value& v )
         {
            switch( v.type() ) {
               case json::type::ARRAY:
                  for( auto& i : v.get_array() ) {
                     resolve_references( r, i );
                  }
                  break;
               case json::type::OBJECT:
                  for( auto& i : v.get_object() ) {
                     resolve_references( r, i.second );
                  }
                  break;
               default:
                  break;
            }
         }

         pointer vector_to_pointer( const std::vector< value >& v )
         {
            pointer result;

            for( const auto& i : v ) {
               assert( i.kind == node_kind::NORMAL );

               if( i.is_string_type() ) {
                  result.push_back( token( i.as< std::string >() ) );
               }
               else if( i.is_number() ) {
                  result.push_back( token( i.as< std::size_t >() ) );
               }
               else {
                  throw "we are not caring about sensible error messages just yet";
               }
            }
            return result;
         }

         void resolve_references_reference( const value& r, value& v )
         {
            for( auto& i : v.get_array() ) {
               resolve_references( r, i );
            }
            const auto p = vector_to_pointer( v.get_array() );
            v = *resolve_for_get( &r, p );
         }

         void resolve_references( const value& r, value& v )
         {
            switch( v.kind ) {
               case node_kind::NORMAL:
                  resolve_references_normal( r, v );
                  break;
               case node_kind::ADDITION:
                  resolve_references_normal( r, v );  // Additions are transparent to the reference resolving pass.
                  break;
               case node_kind::REFERENCE:
                  resolve_references_reference( r, v );
                  break;
            }
         }

         void resolve_references( value& v )
         {
            resolve_references( v, v );
         }

         void resolve_additions( value& )
         {
         }

      }  // namespace internal

      value parse_file( const std::string& filename )
      {
         internal::state st;
         pegtl::file_input in( filename );
         pegtl::parse< internal::grammar, internal::action, internal::control >( in, st );
         assert( st.stack.size() == 1 );
         assert( st.stack.back() == &st.result );
         internal::resolve_references( st.result );
         internal::resolve_additions( st.result );
         return std::move( st.result );
      }

   }  // namespace config

}  // namespace tao

int main( int argc, char** argv )
{
   for( int i = 1; i < argc; ++i ) {
      const auto v = tao::config::parse_file( argv[ i ] );
      tao::json::to_stream( std::cout, v, 3 );
      std::cout << std::endl;
   }
   return 0;
}
