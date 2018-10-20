// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_STATE_HPP
#define TAO_CONFIG_INTERNAL_STATE_HPP

#include <vector>

#include "../external/json.hpp"

#include "pointer.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct state
         {
            state()
            {
               temp.discard();
               result.prepare_array();
               stack.emplace_back( &result );
               result.t = annotation::ADDITION;
               stack.emplace_back( &result.emplace_back( json::empty_object ) );
            }

            value temp;
            value result;

            std::vector< value* > stack;
            std::vector< pointer > keys;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
