/**
 * @file loadIrfs.cxx
 * @brief Function to load the DC1 response functions into the
 * IrfsFactory instance.
 * @author J. Chiang
 *
 * $Header$
 */

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

#include "irfInterface/IrfsFactory.h"
#include "irfUtil/Util.h"

#include "AeffDC1.h"
#include "PsfDC1.h"
#include "EdispDC1.h"

namespace dc1Response {

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
   
   std::vector<std::string> irfsNames;
   myFactory->getIrfsNames(irfsNames);

   long hdu;
   if (getenv("CALDB")) {
      try {
         if ( !std::count(irfsNames.begin(), irfsNames.end(), "DC1::Front") ) {
            irfUtil::Util::getCaldbFile("FRONT", "DETEFF", "DC1",
                                        aeffFile, hdu);
            aeff = new AeffDC1(aeffFile, static_cast<int>(hdu));
            irfUtil::Util::getCaldbFile("FRONT", "RPSF", "DC1", psfFile, hdu);
            psf = new PsfDC1(psfFile, static_cast<int>(hdu), 5);
            irfUtil::Util::getCaldbFile("FRONT", "EREDIS", "DC1",
                                        edispFile, hdu);
            edisp = new EdispDC1(edispFile, static_cast<int>(hdu), 3);
            myFactory->addIrfs("DC1::Front",
                               new irfInterface::Irfs(aeff, psf, edisp, 0));
         }
         if ( !std::count(irfsNames.begin(), irfsNames.end(), "DC1::Back") ) {
            irfUtil::Util::getCaldbFile("BACK", "DETEFF", "DC1",
                                        aeffFile, hdu);
            aeff = new AeffDC1(aeffFile, static_cast<int>(hdu));
            irfUtil::Util::getCaldbFile("BACK", "RPSF", "DC1", psfFile, hdu);
            psf = new PsfDC1(psfFile, static_cast<int>(hdu), 5);
            irfUtil::Util::getCaldbFile("BACK", "EREDIS", "DC1",
                                        edispFile, hdu);
            edisp = new EdispDC1(edispFile, static_cast<int>(hdu), 3);
            myFactory->addIrfs("DC1::Back",
                               new irfInterface::Irfs(aeff, psf, edisp, 1));
         }
      } catch (std::invalid_argument &eObj) {
         std::cout << "IrfsFactory::addDC1Irfs:\n"
                   << eObj.what() << std::endl;
      } catch (...) {
         std::cout << "IrfsFactory::addDC1Irfs:\n"
                   << "unknown exception" << std::endl;
      }
   } else {
// Front
      int npars(5);
      if ( !std::count(irfsNames.begin(), irfsNames.end(), "DC1::Front") ) {
         hdu = 2;
         aeffFile = caldbPath + "ea/aeff_DC1.fits";
         aeff = new AeffDC1(aeffFile, static_cast<int>(hdu));
         psfFile = caldbPath + "psf/psf_DC1.fits";
         psf = new PsfDC1(psfFile, hdu, npars);
         edispFile = caldbPath + "edisp/edisp_DC1.fits";
         npars = 3;
         edisp = new EdispDC1(edispFile, hdu, npars);
         myFactory->addIrfs("DC1::Front",
                            new irfInterface::Irfs(aeff, psf, edisp, 0));
      }

// Back
      if ( !std::count(irfsNames.begin(), irfsNames.end(), "DC1::Back") ) {
         hdu = 3;
         aeff = new AeffDC1(aeffFile, static_cast<int>(hdu));
         npars = 5;
         psf = new PsfDC1(psfFile, hdu, npars);
         npars = 3;
         edisp = new EdispDC1(edispFile, hdu, npars);
         myFactory->addIrfs("DC1::Back",
                            new irfInterface::Irfs(aeff, psf, edisp, 1));
      }
   }
}

} // namespace dc1Response
