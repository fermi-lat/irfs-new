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

#include "st_facilities/Env.h"

#include "irfInterface/IrfsFactory.h"
#include "irfUtil/Util.h"

#include "Aeff.h"
#include "Psf.h"
#include "Edisp.h"

namespace dc2Response {

void loadIrfs() {
   irfInterface::IAeff * aeff;
   irfInterface::IPsf * psf;
   irfInterface::IEdisp * edisp;

   std::string dataPath(st_facilities::Env::getDataDir("dc2Response"));

   irfInterface::IrfsFactory * myFactory 
      = irfInterface::IrfsFactory::instance();

   std::string aeffFile = st_facilities::Env::
      appendFileName(dataPath, "effarea_sup04All-FRONTA.fits");
   std::string psfFile = st_facilities::Env::appendFileName(dataPath,
                                                            "psf_FRONTA.fits");
   std::string edispFile = st_facilities::Env::appendFileName(dataPath,
                                                              "edisp.fits");

   aeff = new Aeff(aeffFile, "EFFECTIVE AREA");
   psf = new Psf(psfFile, "POINT SPREAD FUNCTION");
   edisp = new Edisp(edispFile, "ENERGY REDISTRIBUTION");
   myFactory->addIrfs("DC2::FrontA",
                      new irfInterface::Irfs(aeff, psf, edisp, 0));

   aeffFile = st_facilities::Env::
      appendFileName(dataPath, "effarea_sup04All-BACKA.fits");
   psfFile = st_facilities::Env::appendFileName(dataPath, "psf_BACKA.fits");

   aeff = new Aeff(aeffFile, "EFFECTIVE AREA");
   psf = new Psf(psfFile, "POINT SPREAD FUNCTION");
   edisp = new Edisp(edispFile, "ENERGY REDISTRIBUTION");
   myFactory->addIrfs("DC2::BackA",
                      new irfInterface::Irfs(aeff, psf, edisp, 1));

   aeffFile = st_facilities::Env::
      appendFileName(dataPath, "effarea_sup04All-FRONTB.fits");
   psfFile = st_facilities::Env::appendFileName(dataPath, "psf_FRONTB.fits");

   aeff = new Aeff(aeffFile, "EFFECTIVE AREA");
   psf = new Psf(psfFile, "POINT SPREAD FUNCTION");
   edisp = new Edisp(edispFile, "ENERGY REDISTRIBUTION");
   myFactory->addIrfs("DC2::FrontB",
                      new irfInterface::Irfs(aeff, psf, edisp, 2));

   aeffFile = st_facilities::Env::
      appendFileName(dataPath, "effarea_sup04All-BACKB.fits");
   psfFile = st_facilities::Env::appendFileName(dataPath, "psf_BACKB.fits");

   aeff = new Aeff(aeffFile, "EFFECTIVE AREA");
   psf = new Psf(psfFile, "POINT SPREAD FUNCTION");
   edisp = new Edisp(edispFile, "ENERGY REDISTRIBUTION");
   myFactory->addIrfs("DC2::BackB",
                      new irfInterface::Irfs(aeff, psf, edisp, 3));
}

} // namespace dc2Response
