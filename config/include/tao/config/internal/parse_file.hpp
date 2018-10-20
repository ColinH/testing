// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_PARSE_FILE_HPP
#define TAO_CONFIG_INTERNAL_PARSE_FILE_HPP

#include <cassert>

#include "action.hpp"
#include "control.hpp"
#include "grammar.hpp"
#include "pegtl.hpp"
#include "phase2.hpp"
#include "state.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline void parse_file_impl( state& st, const std::string& filename )
         {
            assert( st.stack.size() == 2 );
            assert( st.stack.front() == &st.result );

            json_pegtl::file_input in( filename );
            st.result.set_position( in.position() );
            st.stack.back()->set_position( in.position() );
            json_pegtl::parse< grammar, action, control >( in, st );

            assert( st.stack.size() == 2 );
            assert( st.stack.front() == &st.result );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
