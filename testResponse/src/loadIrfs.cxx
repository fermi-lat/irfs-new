/**
 * @file loadIrfs.cxx
 * @brief Function to load the test response functions into the
 * IrfsFactory instance.
 * @author J. Chiang
 *
 * $Header$
 */

#include <stdexcept>
#include <string>
#include <vector>

#include "irfInterface/IrfsFactory.h"
#include "irfUtil/Util.h"

#include "Aeff.h"
#include "Psf.h"
#include "Edisp.h"

namespace testResponse {

void loadIrfs() {
   irfInterface::IAeff *aeff;
   irfInterface::IPsf *psf;
   irfInterface::IEdisp *edisp;

   std::string aeffFile;
   std::string psfFile;
   std::string edispFile;
   std::string caldbPath = std::string(::getenv("CALDBROOT"))
      + "/CALDB/data/glast/lat/bcf/";

   irfInterface::IrfsFactory * myFactory 
      = irfInterface::IrfsFactory::instance();

   try {
      aeff = new Aeff(5.5e3, 120., 2.);
      double front_params[] = {3.78, 1.81, 0.8, 5e4, 9.};
      std::vector<double> frontParams(front_params, front_params+5);
      psf = new Psf(frontParams);
      edisp = new Edisp();
      myFactory->addIrfs("testIrfs::Front",
                         new irfInterface::Irfs(aeff, psf, edisp, 0));
      
      aeff = new Aeff(4.3e3, 120., 2.);
      double back_params[] = {6.80, 4.03, 0.85, 2.75e4, 9.};
      std::vector<double> backParams(back_params, back_params+5);
      psf = new Psf(backParams);
      edisp = new Edisp();
      myFactory->addIrfs("testIrfs::Back",
                         new irfInterface::Irfs(aeff, psf, edisp, 1));
   } catch (std::exception &eObj) {
      std::cout << "testResponse::loadIrfs:\n"
                << eObj.what() << std::endl;
   } catch (...) {
      std::cout << "testResponse::loadIrfs:\n"
                << "unknown exception" << std::endl;
   }
}

} // namespace testResponse
