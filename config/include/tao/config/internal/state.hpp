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
               : result( json::empty_object )
            {
               temp.discard();
               stack.emplace_back( &result );
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
