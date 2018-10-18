// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_POINTER_HPP
#define TAO_CONFIG_INTERNAL_POINTER_HPP

#include <vector>

#include "token.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         using pointer = std::vector< token >;

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
