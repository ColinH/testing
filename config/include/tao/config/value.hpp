// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_VALUE_HPP
#define TAO_CONFIG_VALUE_HPP

#include "external/json.hpp"

#include "traits.hpp"

namespace tao
{
   namespace config
   {
      using value = json::basic_value< ::tao::config::traits >;

   }  // namespace config

}  // namespace tao

#endif
