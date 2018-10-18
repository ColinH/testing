// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_PARSE_FILE_HPP
#define TAO_CONFIG_PARSE_FILE_HPP

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
         json_pegtl::file_input in( filename );
         json_pegtl::parse< internal::grammar, internal::action, internal::control >( in, st );
         return internal::value( filename );  // TODO: Return a tao::config::value with the actually parsed file.
      }

   }  // namespace config

}  // namespace tao

#endif
