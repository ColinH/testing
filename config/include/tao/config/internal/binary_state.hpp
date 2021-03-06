// Copyright (c) 2017-2018 Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_BINARY_STATE_HPP
#define TAO_CONFIG_INTERNAL_BINARY_STATE_HPP

#include <cstddef>
#include <utility>
#include <vector>

#include "pegtl.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct binary_state
         {
            template< typename Input >
            explicit binary_state( const Input& in )
               : m_position( in.position() )
            {
            }

            binary_state( const binary_state& ) = delete;
            binary_state( binary_state&& ) = delete;

            ~binary_state() = default;

            void operator=( const binary_state& ) = delete;
            void operator=( binary_state&& ) = delete;

            template< typename Input, typename Consumer >
            void success( const Input&, Consumer& consumer )
            {
               consumer.binary( *this, std::move( value ) );
            }

            const pegtl::position& position() const noexcept
            {
               return m_position;
            }

            std::vector< std::byte > value;

            const pegtl::position m_position;
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
