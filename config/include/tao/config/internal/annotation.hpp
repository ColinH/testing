// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_ANNOTATION_HPP
#define TAO_CONFIG_INTERNAL_ANNOTATION_HPP

#include <optional>

#include "../external/json.hpp"

#include "kind.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct annotation
         {
            config::position position;
            std::optional< internal::kind > kind;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
