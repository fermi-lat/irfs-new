/** 
 * @file test.cxx
 * @brief Test code and scaffolding for latResponse package.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <iostream>
#include <string>
#include <vector>

#include "handoff_response/../src/irfs/FitsTable.h"
#include "handoff_response/../src/irfs/Table.h"

#include "latResponse/FitsTable.h"
#include "Psf.h"

using namespace latResponse;

void compare_to_handoff_response() {
   std::string rootPath(::getenv("LATRESPONSEROOT"));

   std::string filename(rootPath + "/data/psf_Pass5_v0_front.fits");
   std::string extension("RPSF");
   std::string tablename("NCORE");
   
   FitsTable table(filename, extension, tablename);

   handoff_response::FitsTable fitsTable(filename, extension);
   handoff_response::Table old_table(fitsTable.tableData(tablename));
      
   for (double mu(table.minCosTheta()); mu <= 1.; mu += 0.1) {
      for (double logE(1.5); logE < 5.5; logE += 0.25) {
         std::cout << mu << "  " << logE << "  " 
                   << table.value(logE, mu) << "  "
                   << old_table.value(logE, mu) << "  "
                   << table.value(logE, mu, false) << "  "
                   << old_table.value(logE, mu, false) << std::endl;
      }
   }
}

void Psf_test() {
   std::string rootPath(::getenv("LATRESPONSEROOT"));
   std::string filename(rootPath + "/data/psf_Pass5_v0_front.fits");
   Psf foo(filename);
}

int main() {
   compare_to_handoff_response();
   Psf_test();
}
