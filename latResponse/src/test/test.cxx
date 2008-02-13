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

#include <cassert>
#include <cmath>

#include <iostream>
#include <string>
#include <vector>

#include "facilities/Util.h"

#include "irfInterface/Irfs.h"
#include "irfInterface/IrfsFactory.h"

#include "irfLoader/Loader.h"

#include "handoff_response/../src/irfs/FitsTable.h"
#include "handoff_response/../src/irfs/Table.h"

#include "latResponse/FitsTable.h"
#include "latResponse/IrfLoader.h"

#include "Aeff.h"
#include "CaldbDate.h"
#include "Edisp.h"
#include "Psf.h"

using namespace latResponse;

void compare_to_handoff_response() {
   std::string rootPath(::getenv("LATRESPONSEROOT"));

//   std::string filename(rootPath + "/data/psf_Pass5_v0_front.fits");
   std::string filename(rootPath + "/data/psf_Pass5_v0_back.fits");
   std::string extension("RPSF");
//   std::string tablename("NCORE");
   std::string tablename("SIGMA");
   
   FitsTable table(filename, extension, tablename);

   handoff_response::FitsTable fitsTable(filename, extension);
   handoff_response::Table old_table(fitsTable.tableData(tablename));
      
   bool interpolate;
   std::cout << "Comparing Table results to handoff_response:" << std::endl;
   for (double mu(table.minCosTheta()+0.06); mu <= 0.97; mu += 0.1) {
      for (double logE(1.5); logE < 5.5; logE += 0.25) {
         std::cout << mu << "  " << logE << std::endl;
         std::cout << table.value(logE, mu) << "  "
                   << old_table.value(logE, mu) << "  "
                   << table.value(logE, mu, interpolate=false) << "  "
                   << old_table.value(logE, mu, interpolate=false) 
                   << std::endl;
      }
   }
   std::cout << std::endl;
}

void Psf_test() {
   irfInterface::IrfsFactory * myFactory = 
      irfInterface::IrfsFactory::instance();

//   irfInterface::Irfs * irfs(myFactory->create("P5_v0_source/front"));
   irfInterface::Irfs * irfs(myFactory->create("P5_v0_source/back"));
   
   irfInterface::IPsf * psf_p5(irfs->psf());

   std::string rootPath(::getenv("LATRESPONSEROOT"));
//   std::string filename(rootPath + "/data/psf_Pass5_v0_front.fits");
   std::string filename(rootPath + "/data/psf_Pass5_v0_back.fits");
   bool isFront;

   double inclination(30);

   double energy(130);
   astro::SkyDir srcDir(0, inclination);
   astro::SkyDir scZAxis(0, 0);
   astro::SkyDir scXAxis(0, 90);

   astro::SkyDir roiCenter(0, 30);
   double roi_radius(20);

//   irfInterface::Irfs * my_irfs(myFactory->create("PASS5_v0::FRONT"));
   irfInterface::Irfs * my_irfs(myFactory->create("PASS5_v0::BACK"));
   irfInterface::IPsf & my_psf(*(my_irfs->psf()));

// //   Psf my_psf(filename, isFront=true);
//    Psf my_psf(filename, isFront=false);
   std::cout << "Comparing Psf values: "  << std::endl;
   for (double sep(0.1); sep < 20.; sep += 1) {
      std::cout << sep << "  "
                << my_psf.value(sep, energy, inclination, 0) << "  "
                << psf_p5->value(sep, energy, inclination, 0) << std::endl;
   }
   std::cout << std::endl;

   std::cout << "Comparing Psf integrals: "  << std::endl;
   for (roi_radius = 0.5; roi_radius < 30; roi_radius += 1) {
      irfInterface::AcceptanceCone my_cone(roiCenter, roi_radius);
      std::vector<irfInterface::AcceptanceCone *> cones;
      cones.push_back(&my_cone);

      double ref_value = 
         irfInterface::IPsf::psfIntegral(&my_psf, energy, srcDir, 
                                         scZAxis, scXAxis, cones);

      double ref_value2 = 
         irfInterface::IPsf::psfIntegral(psf_p5, energy, srcDir, 
                                         scZAxis, scXAxis, cones);

      std::cout << roi_radius << "  "
                << my_psf.angularIntegral(energy, srcDir, scZAxis, 
                                          scXAxis, cones) << "  "
                << ref_value << "  "
                << psf_p5->angularIntegral(energy, srcDir, scZAxis, 
                                           scXAxis, cones) << "  "
                << ref_value2 << std::endl;
   }
   std::cout << std::endl;
   delete irfs;
}

void Aeff_test() {
   irfInterface::IrfsFactory * myFactory = 
      irfInterface::IrfsFactory::instance();
   irfInterface::Irfs * irfs(myFactory->create("P5_v0_source/back"));

   irfInterface::IAeff & aeff_p5(*irfs->aeff());
   
   std::string rootPath(::getenv("LATRESPONSEROOT"));
   std::string filename(rootPath + "/data/aeff_Pass5_v0_back.fits");
   Aeff aeff(filename);

   double theta(60);
   double emin(30);
   double emax(3e5);
   size_t nee(20);
   double estep(std::log(emax/emin)/(nee - 1));
   std::cout << "Comparing Effective Areas: " << std::endl;
   for (size_t k(0); k < nee; k++) {
      double energy(emin*std::exp(estep*k));
      std::cout << energy << "  "
                << aeff.value(energy, theta, 0) << "  "
                << aeff_p5.value(energy, theta, 0) << std::endl;
   }
   std::cout << std::endl;
   delete irfs;
}

void Edisp_test() {
   irfInterface::IrfsFactory * myFactory = 
      irfInterface::IrfsFactory::instance();
   irfInterface::Irfs * irfs(myFactory->create("P5_v0_source/front"));

   irfInterface::IEdisp & edisp_p5(*irfs->edisp());
   
   std::string rootPath(::getenv("LATRESPONSEROOT"));
   std::string filename(rootPath + "/data/edisp_Pass5_v0_front.fits");
   Edisp edisp(filename);

   double e0(100);
   double theta(10);
   double emin(e0*0.7);
   double emax(e0*1.3);
   size_t nee(20);
   double estep((emax - emin)/(nee - 1));
   std::cout << "Comparing energy dispersion values: " << std::endl;
   for (size_t k(0); k < nee; k++) {
      double energy(emin + estep*k);
      std::cout << energy << "  "
                << edisp.value(energy, e0, theta, 0) << "  "
                << edisp_p5.value(energy, e0, theta, 0) << std::endl;
   }
   std::cout << std::endl;
   delete irfs;
}

void IrfLoader_test() {
   IrfLoader irfLoader;
   irfLoader.loadIrfs();

   irfInterface::IrfsFactory * myFactory = 
      irfInterface::IrfsFactory::instance();

   myFactory->create("PASS4::FRONT");
}

void CaldbDate_test() {
   CaldbDate date1("2007-10-01");
   CaldbDate date2("2007-11-01");
   assert(date1 < date2);
   assert(date2 > date1);
}

int main() {
#ifdef TRAP_FPE
   feenableexcept (FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
#endif

   irfLoader::Loader_go();

//     compare_to_handoff_response();
   Psf_test();
//     Aeff_test();
//    Edisp_test();

//    IrfLoader_test();

//    CaldbDate_test();

}
