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

            std::size_t line;
            std::size_t byte_in_line;
            std::string source;

            std::optional< type > t;

            mutable bool phase2_recursion_marker = false;

            template< typename P >
            void set_position( const P& p )
            {
               line = p.line;
               byte_in_line = p.byte_in_line;
               source = p.source;
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
