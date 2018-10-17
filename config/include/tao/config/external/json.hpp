// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_EXTERNAL_JSON_HPP
#define TAO_CONFIG_EXTERNAL_JSON_HPP

#include <tao/json.hpp>

#include <tao/json/contrib/position.hpp>
#include <tao/json/contrib/traits.hpp>

namespace tao
{
   namespace config
   {
      using pointer = json::pointer;
      using position = json::position;

   }  // namespace config

}  // namespace tao

#endif
