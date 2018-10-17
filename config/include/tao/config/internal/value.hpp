// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_VALUE_HPP
#define TAO_CONFIG_INTERNAL_VALUE_HPP

#include "../external/json.hpp"

#include "traits.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         using value = json::basic_value< ::tao::config::internal::traits >;

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
