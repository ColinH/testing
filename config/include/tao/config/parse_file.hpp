// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_PARSE_FILE_HPP
#define TAO_CONFIG_PARSE_FILE_HPP

#include <cassert>

#include "internal/action.hpp"
#include "internal/control.hpp"
#include "internal/grammar.hpp"
#include "internal/pegtl.hpp"
#include "internal/phase2.hpp"
#include "internal/state.hpp"
#include "internal/value.hpp"

namespace tao
{
   namespace config
   {
      inline internal::value parse_file( const std::string& filename )
      {
         internal::state st;

         assert( st.stack.size() == 2 );
         assert( st.stack.front() == &st.result );

         json_pegtl::file_input in( filename );
         st.result.position.set_position( in.position() );
         st.stack.back()->position.set_position( in.position() );
         json_pegtl::parse< internal::grammar, internal::action, internal::control >( in, st );

         assert( st.stack.size() == 2 );
         assert( st.stack.front() == &st.result );

         internal::phase2( st.result );
         return std::move( st.result );  // TODO: Change type of result from config::internal::value to config::value (or basic_value< user_traits >).
      }

   }  // namespace config

}  // namespace tao

#endif
