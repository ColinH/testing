// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_PARSE_FILE_HPP
#define TAO_CONFIG_PARSE_FILE_HPP

#include "internal/grammar.hpp"
#include "internal/value.hpp"

namespace tao
{
   namespace config
   {
      inline internal::value parse_file( const std::string& filename )
      {
         return internal::value( filename );  // TODO: Return a tao::config::value with the actually parsed file.
      }

   }  // namespace config

}  // namespace tao

#endif
