// Copyright (c) 2018 Dr. Colin Hirsch

#include <tao/config.hpp>

int main( int argc, char** argv )
{
   if( const auto p = tao::json_pegtl::analyze< tao::config::internal::grammar >( true ) ) {
      std::cerr << "PEGTL grammar analysis found " << p << " problems" << std::endl;
   }
   for( int i = 1; i < argc; ++i ) {
      tao::config::check_file( argv[ i ] );
   }
   return 0;
}
