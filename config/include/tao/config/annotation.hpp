// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_ANNOTATION_HPP
#define TAO_CONFIG_ANNOTATION_HPP

#include "external/json.hpp"

namespace tao
{
   namespace config
   {
      struct annotation
      {
         config::pointer pointer;
         config::position position;
      };

   }  // namespace config

}  // namespace tao

#endif
