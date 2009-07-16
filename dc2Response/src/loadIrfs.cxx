/**
 * @file loadIrfs.cxx
 * @brief Function to load the DC2 response functions into the
 * IrfsFactory instance.
 * @author J. Chiang
 *
 * $Header$
 */

#include <algorithm>
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

   if (!std::count(irfsNames.begin(), irfsNames.end(), "DC2::FrontA")) {
      irfUtil::Util::getCaldbFile("FRONTA", "EFF_AREA", "DC2",
                                  aeffFile, hdu, "GLAST", "LAT",
                                  "NONE", "2006-03-01", "00:00:00");
      irfUtil::Util::getCaldbFile("FRONTA", "PSF", "DC2", 
                                  psfFile, hdu, "GLAST", "LAT",
                                  "NONE", "2006-03-01", "00:00:00");
      irfUtil::Util::getCaldbFile("FRONTA", "EDISP", "DC2",
                                  edispFile, hdu, "GLAST", "LAT",
                                  "NONE", "2006-03-01", "00:00:00");

      aeff = new Aeff(aeffFile, "EFFECTIVE AREA");
      psf = new Psf(psfFile, "POINT SPREAD FUNCTION");
      edisp = new Edisp(edispFile, "ENERGY REDISTRIBUTION");

      myFactory->addIrfs("DC2::FrontA",
                         new irfInterface::Irfs(aeff, psf, edisp, 0));
   }

   if (!std::count(irfsNames.begin(), irfsNames.end(), "DC2::BackA")) {
      irfUtil::Util::getCaldbFile("BACKA", "EFF_AREA", "DC2",
                                  aeffFile, hdu, "GLAST", "LAT",
                                  "NONE", "2006-03-01", "00:00:00");
      irfUtil::Util::getCaldbFile("BACKA", "PSF", "DC2", 
                                  psfFile, hdu, "GLAST", "LAT",
                                  "NONE", "2006-03-01", "00:00:00");

      aeff = new Aeff(aeffFile, "EFFECTIVE AREA");
      psf = new Psf(psfFile, "POINT SPREAD FUNCTION");
// There is only one energy dispersion file, so reuse existing guy.
      edisp = new Edisp(edispFile, "ENERGY REDISTRIBUTION"); 

      myFactory->addIrfs("DC2::BackA",
                         new irfInterface::Irfs(aeff, psf, edisp, 1));
   }

   if (!std::count(irfsNames.begin(), irfsNames.end(), "DC2::FrontB")) {
      irfUtil::Util::getCaldbFile("FRONTB", "EFF_AREA", "DC2",
                                  aeffFile, hdu, "GLAST", "LAT",
                                  "NONE", "2006-03-01", "00:00:00");
      irfUtil::Util::getCaldbFile("FRONTB", "PSF", "DC2", 
                                  psfFile, hdu, "GLAST", "LAT",
                                  "NONE", "2006-03-01", "00:00:00");

      aeff = new Aeff(aeffFile, "EFFECTIVE AREA");
      psf = new Psf(psfFile, "POINT SPREAD FUNCTION");
      edisp = new Edisp(edispFile, "ENERGY REDISTRIBUTION"); 

      myFactory->addIrfs("DC2::FrontB",
                         new irfInterface::Irfs(aeff, psf, edisp, 2));
   }
   if (!std::count(irfsNames.begin(), irfsNames.end(), "DC2::BackB")) {
      irfUtil::Util::getCaldbFile("BACKB", "EFF_AREA", "DC2",
                                  aeffFile, hdu, "GLAST", "LAT",
                                  "NONE", "2006-03-01", "00:00:00");
      irfUtil::Util::getCaldbFile("BACKB", "PSF", "DC2", 
                                  psfFile, hdu, "GLAST", "LAT",
                                  "NONE", "2006-03-01", "00:00:00");

      aeff = new Aeff(aeffFile, "EFFECTIVE AREA");
      psf = new Psf(psfFile, "POINT SPREAD FUNCTION");
      edisp = new Edisp(edispFile, "ENERGY REDISTRIBUTION"); 

      myFactory->addIrfs("DC2::BackB",
                         new irfInterface::Irfs(aeff, psf, edisp, 3));
   }
}   

} // namespace dc2Response
