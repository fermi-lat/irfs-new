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

   long hdu;
   if (getenv("CALDB")) {
      try {
         irfUtil::Util::getCaldbFile("FRONT", "DETEFF", "DC1", aeffFile, hdu);
         aeff = new Aeff(aeffFile, static_cast<int>(hdu));
         double front_params[] = {3.78, 1.81, 0.8, 5e4, 0.4};
         std::vector<double> frontParams(front_params, front_params+5);
         psf = new Psf(frontParams);
         edisp = new Edisp();
         myFactory->addIrfs("testIrfs::Front",
                            new irfInterface::Irfs(aeff, psf, edisp, 0));

         irfUtil::Util::getCaldbFile("BACK", "DETEFF", "DC1", aeffFile, hdu);
         aeff = new Aeff(aeffFile, static_cast<int>(hdu));
         double back_params[] = {6.80, 4.03, 0.85, 2.75e4, 0.4};
         std::vector<double> backParams(back_params, back_params+5);
         psf = new Psf(backParams);
         edisp = new Edisp();
         myFactory->addIrfs("testIrfs::Back",
                            new irfInterface::Irfs(aeff, psf, edisp, 1));
      } catch (std::invalid_argument &eObj) {
         std::cout << "testResponse::loadIrfs:\n"
                   << eObj.what() << std::endl;
      } catch (...) {
         std::cout << "testResponse::loadIrfs:\n"
                   << "unknown exception" << std::endl;
      }
   } else {
      throw std::runtime_error("CALDB environment variable not set.");
   }
}

} // namespace testResponse
