// Copyright (c) 2018 Dr. Colin Hirsch

#ifndef TAO_CONFIG_INTERNAL_ACCESS_HPP
#define TAO_CONFIG_INTERNAL_ACCESS_HPP

#include <stdexcept>

#include "pointer.hpp"
#include "utility.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline list_t& access( list_t& l, const pointer& p );

         inline list_t& access_name( list_t& l, const std::string& k, const pointer& p )
         {
            for( auto& i : reverse( l ) ) {
               if( !i.is_object() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               const auto j = i.get_object().find( k );

               if( j != i.get_object().end() ) {
                  return access( j->second, p );
               }
            }
            throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
         }

         inline list_t& access_index( list_t& l, std::size_t n, const pointer& p )
         {
            for( auto& i : l ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               const auto s = i.get_array().size();

               if( n < s ) {
                  return access( i.get_array()[ n ], p );
               }
               n -= s;
            }
            throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
         }

         inline list_t& access_minus( list_t& l, const pointer& p )
         {
            for( auto& i : reverse( l ) ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               if( !i.get_array().empty() ) {
                  return access( i.get_array().back(), p );
               }
            }
            throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
         }

         inline list_t& access( list_t& l, const token& t, const pointer& p )
         {
            switch( t.t ) {
               case token::NAME:
                  return access_name( l, t.k, p );
               case token::INDEX:
                  return access_index( l, t.i, p );
               case token::STAR:
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );  // TODO: Proper exception messages everywhere.
               case token::MINUS:
                  return access_minus( l, p );
            }
            assert( false );
         }

         inline list_t& access( list_t& l, const pointer& p )
         {
            if( p.empty() ) {
               return l;
            }
            return access( l, p.front(), pop_front( p ) );
         }

         inline list_t& access( object_t& o, const token& t, const pointer& p )
         {
            switch( t.t ) {
               case token::NAME:
                  return access( o.at( t.k ), p );  // TODO: Proper exception message on key-not-found.
               case token::INDEX:
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               case token::STAR:
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               case token::MINUS:
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
            }
            assert( false );
         }

         inline list_t& access( object_t& o, const pointer& p )
         {
            assert( !p.empty() );

            return access( o, p.front(), pop_front( p ) );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
