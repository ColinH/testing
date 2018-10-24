// Copyright (c) 2018 Dr. Colin Hirsch

#include <tao/config.hpp>

int main( int argc, char** argv )
{
   const tao::config::value v = tao::config::parse_files( std::vector< std::string >( argv + 1, argv + argc ) );
   tao::json::jaxn::to_stream( std::cout, v, 3 );
   std::cout << std::endl;
   return 0;
}
