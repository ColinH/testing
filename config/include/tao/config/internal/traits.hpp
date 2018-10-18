// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_TRAITS_HPP

#include "../external/json.hpp"

#include "annotation.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename T >
         struct traits
            : public json::traits< T >
         {
         };

         template<>
         struct traits< void >
            : public json::traits< void >
         {
            template< typename Value >
            using public_base = annotation;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
