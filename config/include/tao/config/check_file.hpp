// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_CHECK_FILE_HPP
#define TAO_CONFIG_CHECK_FILE_HPP

#include "internal/grammar.hpp"
#include "internal/pegtl.hpp"

namespace tao
{
   namespace config
   {
      inline void check_file( const std::string& filename )
      {
         json_pegtl::file_input in( filename );
         json_pegtl::parse< internal::grammar >( in );
      }

   }  // namespace config

}  // namespace tao

#endif
