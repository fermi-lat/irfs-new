/**
 * @file test_fitsconv.cxx
 * @brief Not real test code....scaffolding for development.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include <string>
#include <vector>

#include "fits/IrfTable.h"
#include "fits/IrfTableMap.h"

int main() {
   char * root_dir(::getenv("HANDOFF_RESPONSEROOT"));
   if (!root_dir) {
      std::cout << "HANDOFF_RESPONSEROOT not set" << std::endl;
      std::exit(1);
   }
   std::string root_file(std::string(root_dir) + "/data/parameters.root");

   handoff_response::IrfTableMap front("standard::front", root_file);

   for (size_t i = 0; i < front.keys().size(); i++) {
      std::cout << front.keys().at(i) << "\n";
   }

   const handoff_response::IrfTable & table(front["aeff"]);

   const std::vector<double> & xaxis(table.xaxis());
   for (size_t i = 0; i < xaxis.size(); i++) {
      std::cout << xaxis.at(i) << "  ";
   }
   std::cout << std::endl;

   const std::vector<double> & yaxis(table.yaxis());
   for (size_t i = 0; i < yaxis.size(); i++) {
      std::cout << yaxis.at(i) << "  ";
   }
   std::cout << std::endl;

   for (size_t i = 0; i < xaxis.size(); i++) {
      std::cout << table(i, 1) << "  ";
   }
   std::cout << std::endl;

   return 0;
}
