// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_ANNOTATION_HPP
#define TAO_CONFIG_INTERNAL_ANNOTATION_HPP

#include <optional>

#include "../external/json.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct annotation
         {
            enum type : bool
            {
               ADDITION,
               REFERENCE
            };

            std::optional< type > t;
            config::position position;

            mutable bool phase2_recursion_marker = false;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
