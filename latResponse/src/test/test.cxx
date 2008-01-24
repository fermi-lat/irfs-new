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

#include "irfInterface/Irfs.h"
#include "irfInterface/IrfsFactory.h"

#include "irfLoader/Loader.h"

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
      
   bool interpolate;
   for (double mu(table.minCosTheta()); mu <= 1.; mu += 0.1) {
      for (double logE(1.5); logE < 5.5; logE += 0.25) {
         std::cout << mu << "  " << logE << "  " 
                   << table.value(logE, mu) << "  "
                   << old_table.value(logE, mu) << "  "
                   << table.value(logE, mu, interpolate=false) << "  "
                   << old_table.value(logE, mu, interpolate=false) 
                   << std::endl;
      }
   }
}

void Psf_test() {
   irfInterface::IrfsFactory * myFactory = 
      irfInterface::IrfsFactory::instance();

   irfInterface::Irfs * irfs(myFactory->create("P5_v0_source/front"));
   
   irfInterface::IPsf * psf_p5(irfs->psf());

   std::string rootPath(::getenv("LATRESPONSEROOT"));
   std::string filename(rootPath + "/data/psf_Pass5_v0_front.fits");
   std::string extname;
   bool isFront;

   double inclination(10);

   double energy(100);
   astro::SkyDir srcDir(0, inclination);
   astro::SkyDir scZAxis(0, 0);
   astro::SkyDir scXAxis(0, 90);

   astro::SkyDir roiCenter(0, 5);
   double roi_radius(20);

   Psf my_psf(filename, extname="RPSF", isFront=true);
   for (double sep(0.1); sep < 20.; sep += 1) {
      std::cout << sep << "  "
                << my_psf.value(sep, energy, inclination, 0) << "  "
                << psf_p5->value(sep, energy, inclination, 0) << std::endl;
   }

   for (roi_radius = 0.5; roi_radius < 30; roi_radius += 1) {
      Psf foo(filename, extname="RPSF", isFront=true);
      irfInterface::AcceptanceCone my_cone(roiCenter, roi_radius);
      std::vector<irfInterface::AcceptanceCone *> cones;
      cones.push_back(&my_cone);

      double ref_value = 
         irfInterface::IPsf::psfIntegral(&foo, energy, srcDir, 
                                         scZAxis, scXAxis, cones);

      std::cout << roi_radius << "  "
                << foo.angularIntegral(energy, srcDir, scZAxis, 
                                       scXAxis, cones)/ref_value << "  "
                << psf_p5->angularIntegral(energy, srcDir, scZAxis, 
                                           scXAxis, cones)/ref_value
                << std::endl;
   }
}



int main() {
#ifdef TRAP_FPE
   feenableexcept (FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
#endif

   irfLoader::Loader_go();

   compare_to_handoff_response();

   Psf_test();
}
