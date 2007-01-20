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

void load_irfs() {
   irfInterface::IAeff *aeff;
   irfInterface::IPsf *psf;
   irfInterface::IEdisp *edisp;

   std::string aeffFile;
   std::string psfFile;
   std::string edispFile;

   irfInterface::IrfsFactory * myFactory 
      = irfInterface::IrfsFactory::instance();
   
   std::vector<std::string> irfsNames;
   myFactory->getIrfsNames(irfsNames);

   long hdu;

   char * caldbroot = ::getenv("CALDB");
   if (!caldbroot) {
      throw std::runtime_error("CALDB is not set");
   }

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
}

} // namespace dc1Response
