/** 
 * @file test.cxx
 * @brief Test code and scaffolding for latResponse package.
 * @author J. Chiang
 *
 * $Header$
 */

#ifdef TRAP_FPE
#include <fenv.h>
#endif

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
   std::string extname;
   bool isFront;

   double inclination(20);

   double energy(100);
   astro::SkyDir srcDir(0, inclination);
   astro::SkyDir scZAxis(0, 0);
   astro::SkyDir scXAxis(0, 90);

   astro::SkyDir roiCenter(0, 5);
   double roi_radius(20);

   for (roi_radius = 0.5; roi_radius < 30; roi_radius += 1) {
      Psf foo(filename, extname="RPSF", isFront=true);
      irfInterface::AcceptanceCone my_cone(roiCenter, roi_radius);
      std::vector<irfInterface::AcceptanceCone *> cones;
      cones.push_back(&my_cone);

      std::cout << roi_radius << "  "
                << (foo.angularIntegral(energy, srcDir, scZAxis, scXAxis,cones)
                    /irfInterface::IPsf::psfIntegral(&foo, energy, srcDir, 
                                                     scZAxis, scXAxis, cones))
                << std::endl;
   }
}

int main() {
#ifdef TRAP_FPE
   feenableexcept (FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
#endif

//   compare_to_handoff_response();
   Psf_test();
}
