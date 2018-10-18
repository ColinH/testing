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
         template< typename Value >
         class annotation
         {
         public:
            config::position position;

            void set_kind( const kind k )
            {
               assert( !m_kind || ( m_kind == k ) );  // TODO: Find out where it's already set.
               assert( static_cast< const Value& >( *this ).is_array() );

               m_kind = k;
            }

            const std::optional< internal::kind > kind() const noexcept
            {
               return m_kind;
            }

         private:
            std::optional< internal::kind > m_kind;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
