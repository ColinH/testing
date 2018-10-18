// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_PARSE_FILE_HPP
#define TAO_CONFIG_PARSE_FILE_HPP

#include <cassert>

#include "internal/action.hpp"
#include "internal/control.hpp"
#include "internal/grammar.hpp"
#include "internal/pegtl.hpp"
#include "internal/state.hpp"
#include "internal/value.hpp"

namespace tao
{
   namespace config
   {
      inline internal::value parse_file( const std::string& filename )
      {
         internal::state st;
         assert( st.stack.size() == 1 );
         assert( st.stack.back() == &st.result );
         json_pegtl::file_input in( filename );
         json_pegtl::parse< internal::grammar, internal::action, internal::control >( in, st );
         assert( st.stack.size() == 1 );
         assert( st.stack.back() == &st.result );
         //         internal::handle_references( st.result );
         //         internal::handle_additions( st.result );
         return std::move( st.result );  // TODO: Change type of result from config::internal::value to config::value...
      }

   }  // namespace config

}  // namespace tao

#endif
