// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_UTILITY_HPP

#include <cassert>
#include <stdexcept>

#include "json.hpp"
#include "token.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Container >
         struct reverse
         {
            explicit
            reverse( Container& l )
               : m_c( l )
            {
            }

            reverse( reverse&& ) = delete;
            reverse( const reverse& ) = delete;

            void operator=( reverse&& ) = delete;
            void operator=( const reverse& ) = delete;

            auto begin()
            {
               return m_c.rbegin();
            }

            auto end()
            {
               return m_c.rend();
            }

         private:
            Container& m_c;
         };

         template< typename Container >
         reverse( Container& )->reverse< Container >;

         inline token token_from_value( const json::value& v )
         {
            switch( v.type() ) {
               case json::type::STRING:
               case json::type::STRING_VIEW:
                  return token( v.as< std::string >() );
               case json::type::SIGNED:
               case json::type::UNSIGNED:
                  return token( v.as< std::size_t >() );
               default:
                  throw std::runtime_error( "invalid json type for token" );
            }
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
