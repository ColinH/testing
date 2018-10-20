// Copyright (c) 2018 Dr. Colin Hirsch

#include <tao/config.hpp>
#include <tao/config/internal/to_stream.hpp>

int main( int argc, char** argv )
{
   for( int i = 1; i < argc; ++i ) {
      const auto v = tao::config::parse_file( argv[ i ] );
      tao::config::to_stream( std::cout, v, 3 );
      std::cout << std::endl;
   }
   return 0;
}
