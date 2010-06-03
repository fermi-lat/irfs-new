/**
 * @file loadIrfs.cxx
 * @brief Function to load the GLAST25 response functions into the
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

#include "AeffGlast25.h"
#include "PsfGlast25.h"
#include "EdispGlast25.h"

namespace g25Response {

void loadIrfs() {
   std::string aeffFile;
   std::string psfFile;
   std::string edispFile;
   
   irfInterface::IAeff *aeff;
   irfInterface::IPsf *psf;
   irfInterface::IEdisp *edisp;

   irfInterface::IrfsFactory * myFactory 
      = irfInterface::IrfsFactory::instance();

   std::vector<std::string> irfsNames;
   myFactory->getIrfsNames(irfsNames);

   std::string version("GLAST25");

   std::vector<std::string> hduNames;
   int irfID[] = {0, 1, 2};
                      
   if (getenv("CALDB")) {
      long hdu;
      try {
         if ( !std::count(irfsNames.begin(), irfsNames.end(),
                          "Glast25::Front") ) {
            irfUtil::Util::getCaldbFile("FRONT", "DETEFF", 
                                        version, aeffFile, hdu);
            aeff = new AeffGlast25(aeffFile, static_cast<int>(hdu));
            irfUtil::Util::getCaldbFile("FRONT", "RPSF", version, 
                                        psfFile, hdu);
            psf = new PsfGlast25(psfFile, static_cast<int>(hdu));
            irfUtil::Util::getCaldbFile("FRONT", "EREDIS", 
                                        version, edispFile, hdu);
            edisp = new EdispGlast25(edispFile, static_cast<int>(hdu));
            myFactory->addIrfs("Glast25::Front", 
                               new irfInterface::Irfs(aeff, psf, edisp, 0));
         }
         
         if ( !std::count(irfsNames.begin(), irfsNames.end(),
                          "Glast25::Back") ) {
            irfUtil::Util::getCaldbFile("BACK", "DETEFF", version,
                                        aeffFile, hdu);
            aeff = new AeffGlast25(aeffFile, static_cast<int>(hdu));
            irfUtil::Util::getCaldbFile("BACK", "RPSF", version, psfFile, hdu);
            psf = new PsfGlast25(psfFile, static_cast<int>(hdu));
            irfUtil::Util::getCaldbFile("BACK", "EREDIS", 
                                        version, edispFile, hdu);
            edisp = new EdispGlast25(edispFile, static_cast<int>(hdu));
            myFactory->addIrfs("Glast25::Back", 
                               new irfInterface::Irfs(aeff, psf, edisp, 1));
         }
         
         if ( !std::count(irfsNames.begin(), irfsNames.end(),
                          "Glast25::Combined") ) {
            irfUtil::Util::getCaldbFile("COMBINED", "DETEFF", 
                                        version, aeffFile, hdu);
            aeff = new AeffGlast25(aeffFile, static_cast<int>(hdu));
            irfUtil::Util::getCaldbFile("COMBINED", "RPSF", 
                                        version, psfFile, hdu);
            psf = new PsfGlast25(psfFile, static_cast<int>(hdu));
            irfUtil::Util::getCaldbFile("COMBINED", "EREDIS", 
                                        version, edispFile, hdu);
            edisp = new EdispGlast25(edispFile, static_cast<int>(hdu));
            myFactory->addIrfs("Glast25::Combined", 
                               new irfInterface::Irfs(aeff, psf, edisp, 2));
         }
      } catch (std::invalid_argument &eObj) {
         std::cout << "IrfsFactory::addGlast25Irfs:\n"
                   << eObj.what() << std::endl;
      } catch (...) {
         std::cout << "IrfsFactory::addGlast25Irfs:\n"
                   << "unknown exception" << std::endl;
      }
   } else {
      std::string caldbPath = std::string(::getenv("CALDBROOT"))
         + "/CALDB/data/glast/lat/bcf/";
      aeffFile = caldbPath + "ea/aeff_lat.fits";
      psfFile = caldbPath + "psf/psf_lat.fits";

// The standard GLAST 25 response functions.
      hduNames.push_back("Glast25::Front");
      hduNames.push_back("Glast25::Back");
      hduNames.push_back("Glast25::Combined");
      for (int hdu = 2; hdu < 5; hdu++) {
         aeff = new AeffGlast25(aeffFile, hdu);
         psf = new PsfGlast25(psfFile, hdu);
         edisp = new EdispGlast25();
         myFactory->addIrfs(hduNames[hdu-2], 
                            new irfInterface::Irfs(aeff, psf, edisp, 
                                                   irfID[hdu-2]));
      }
   }

// Add a set of irfs with 10% energy resolution.
   if ( !std::count(irfsNames.begin(), irfsNames.end(),
                    "Glast25::Front") ) {
      hduNames.clear();
      hduNames.push_back("Glast25::Front_10");
      hduNames.push_back("Glast25::Back_10");
      hduNames.push_back("Glast25::Combined_10");
      for (int hdu = 2; hdu < 5; hdu++) {
         aeff = new AeffGlast25(aeffFile, hdu);
         psf = new PsfGlast25(psfFile, hdu);
         edisp = new EdispGlast25(0.1);
         myFactory->addIrfs(hduNames[hdu-2], 
                            new irfInterface::Irfs(aeff, psf, edisp, 
                                                   irfID[hdu-2]));
      }

// Add a set of irfs with a flat effective area
      int hdu = 4;
      aeff = new AeffGlast25(aeffFile, hdu, -1);
      psf = new PsfGlast25(psfFile, hdu);
      edisp = new EdispGlast25();
      myFactory->addIrfs("Glast25::FlatAeff", 
                         new irfInterface::Irfs(aeff, psf, edisp,
                                                irfID[hdu-2]));
   }
}

} // namespace g25Response
