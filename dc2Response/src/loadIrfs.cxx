/**
 * @file loadIrfs.cxx
 * @brief Function to load the DC2 response functions into the
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

namespace dc2Response {

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

// ROOT versions.
   aeffFile = caldbPath + "ea/aeff_dc2.root";

// Front
   psfFile = caldbPath + "psf/psf_thin_parameters_dc2.root";
   edispFile = caldbPath + "edisp/edisp_thin_parameters_dc2.root";

   bool getFront(true);
   aeff = new Aeff(aeffFile, getFront);
   psf = new Psf(psfFile);
   edisp = new Edisp(edispFile);
   myFactory->addIrfs("DC2::Front",
                      new irfInterface::Irfs(aeff, psf, edisp, 0));

// Back
   psfFile = caldbPath + "psf/psf_thick_parameters_dc2.root";
   edispFile = caldbPath + "edisp/edisp_thick_parameters_dc2.root";

   aeff = new Aeff(aeffFile, !getFront);
   psf = new Psf(psfFile);
   edisp = new Edisp(edispFile);
   myFactory->addIrfs("DC2::Back",
                      new irfInterface::Irfs(aeff, psf, edisp, 1));
   
// // FITS file versions.

//    long hdu;
//    if (getenv("CALDB")) {
//       try {
//          irfUtil::Util::getCaldbFile("FRONT", "DETEFF", "DC2", aeffFile, hdu);
//          aeff = new AeffDC2(aeffFile, static_cast<int>(hdu));
//          irfUtil::Util::getCaldbFile("FRONT", "RPSF", "DC2", psfFile, hdu);
//          psf = new PsfDC2(psfFile, static_cast<int>(hdu), 5);
//          irfUtil::Util::getCaldbFile("FRONT", "EREDIS", "DC2", edispFile, hdu);
//          edisp = new EdispDC2(edispFile, static_cast<int>(hdu), 3);
//          myFactory->addIrfs("DC2::Front",
//                             new irfInterface::Irfs(aeff, psf, edisp, 0));

//          irfUtil::Util::getCaldbFile("BACK", "DETEFF", "DC2", aeffFile, hdu);
//          aeff = new AeffDC2(aeffFile, static_cast<int>(hdu));
//          irfUtil::Util::getCaldbFile("BACK", "RPSF", "DC2", psfFile, hdu);
//          psf = new PsfDC2(psfFile, static_cast<int>(hdu), 5);
//          irfUtil::Util::getCaldbFile("BACK", "EREDIS", "DC2", edispFile, hdu);
//          edisp = new EdispDC2(edispFile, static_cast<int>(hdu), 3);
//          myFactory->addIrfs("DC2::Back",
//                             new irfInterface::Irfs(aeff, psf, edisp, 1));
//       } catch (std::invalid_argument &eObj) {
//          std::cout << "IrfsFactory::addDC2Irfs:\n"
//                    << eObj.what() << std::endl;
//       } catch (...) {
//          std::cout << "IrfsFactory::addDC2Irfs:\n"
//                    << "unknown exception" << std::endl;
//       }
//    } else {
// // Front
//       hdu = 2;
//       aeffFile = caldbPath + "aeff_DC2.fits";
//       aeff = new AeffDC2(aeffFile, static_cast<int>(hdu));
//       psfFile = caldbPath + "psf_DC2.fits";
//       int npars(5);
//       psf = new PsfDC2(psfFile, hdu, npars);
//       edispFile = caldbPath + "edisp_DC2.fits";
//       npars = 3;
//       edisp = new EdispDC2(edispFile, hdu, npars);
//       myFactory->addIrfs("DC2::Front",
//                          new irfInterface::Irfs(aeff, psf, edisp, 0));

// // Back
//       hdu = 3;
//       aeff = new AeffDC2(aeffFile, static_cast<int>(hdu));
//       npars = 5;
//       psf = new PsfDC2(psfFile, hdu, npars);
//       npars = 3;
//       edisp = new EdispDC2(edispFile, hdu, npars);
//       myFactory->addIrfs("DC2::Back",
//                          new irfInterface::Irfs(aeff, psf, edisp, 1));
//    }
}

} // namespace dc2Response
