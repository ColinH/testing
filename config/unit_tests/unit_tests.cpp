// Copyright (c) 2018 Dr. Colin Hirsch

#include <tao/config.hpp>

namespace tao
{
   int analyse()
   {
      if( const auto p = tao::json_pegtl::analyze< tao::config::internal::grammar >( true ) ) {
         std::cerr << "PEGTL grammar analysis found " << p << " problems!" << std::endl;
         return p;
      }
      return 0;
   }

   int failed = analyse();

   void unit_test( const std::string& name )
   {
      const auto c = config::parse_file( name + ".config" );
      const auto j = json::jaxn::basic_parse_file< config::traits >( name + "_only_data.jaxn" );
      if( c != j ) {
         ++failed;
         std::cerr << "Testcase '" << name << "' failed!" << std::endl;
      }
   }

}  // namespace tao

int main( int argc, char** argv )
{
   for( int i = 1; i < argc; ++i ) {
      tao::unit_test( argv[ i ] );
   }
   if( ! tao::failed ) {
      std::cerr << "All testcases passed." << std::endl;
   }
   return tao::failed;
}
