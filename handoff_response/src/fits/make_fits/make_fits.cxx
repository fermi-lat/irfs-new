/**
 * @file make_aeff.cxx
 * @brief Program for converting ROOT tables to FITS.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <iostream>
#include <stdexcept>

#include "facilities/Util.h"

#include "st_facilities/Util.h"

#include "irfs/RootEval.h"

#include "fits/IrfTableMap.h"
#include "fits/FitsFile.h"

void parseName(const std::string & rootClassName,
               std::string & className, 
               std::string & section) {
   std::vector<std::string> tokens;
   facilities::Util::stringTokenize(rootClassName, "/", tokens);
   className = tokens.at(0);
   section = tokens.at(1);
}

void createFitsFiles(const std::string & rootClassName,
                     const std::string & rootfile) {
   using handoff_response::IrfTableMap;
   using handoff_response::FitsFile;

   std::string className, section;
   parseName(rootClassName, className, section);
   IrfTableMap irfTables(className + "::" + section, rootfile);
   std::string latclass(className + "_" + section);
      
// Effective area
   FitsFile aeff("aeff_" + latclass + ".fits", "EFFECTIVE AREA", "aeff.tpl");
   aeff.setGrid(irfTables["aeff"]);
   aeff.setTableData("EFFAREA", irfTables["aeff"].values());
   aeff.setClassName(latclass);
   aeff.close();

// Point spread function and angular deviation scaling parameters
   std::string psf_file("psf_" + latclass + ".fits");
   FitsFile psf(psf_file, "RPSF", "psf.tpl");
   psf.setGrid(irfTables["ncore"]);
   psf.setTableData("NCORE", irfTables["ncore"].values());
//    psf.setTableData("NTAIL", irfTables["ntail"].values());
   psf.setTableData("NTAIL", irfTables["ncore"].values());
//    psf.setTableData("SCORE", irfTables["score"].values());
//    psf.setTableData("STAIL", irfTables["stail"].values());
   psf.setTableData("SIGMA", irfTables["sigma"].values());
   psf.setTableData("GCORE", irfTables["gcore"].values());
   psf.setTableData("GTAIL", irfTables["gtail"].values());
   psf.setClassName(latclass);
   psf.close();
   
   double scaling_pars[] = {5.8e-2, 3.77e-4, 9.6e-2, 1.3e-3, -0.8};
   std::vector<double> scalingPars(scaling_pars, scaling_pars + 5);
   bool newFile;
   FitsFile psfScaling(psf_file, "PSF_SCALING_PARAMS", "psf.tpl", 
                       newFile=false);
   psfScaling.setTableData("PSFSCALE", scalingPars);
   psfScaling.close();

// Energy dispersion
   FitsFile edisp("edisp_" + latclass + ".fits", "ENERGY DISPERSION", 
                  "edisp.tpl");
   edisp.setGrid(irfTables["dnorm"]);
   edisp.setTableData("DNORM", irfTables["dnorm"].values());
   edisp.setTableData("LTAIL", irfTables["ltail"].values());
   edisp.setTableData("RWIDTH", irfTables["rwidth"].values());
   edisp.setTableData("NR2", irfTables["nr2"].values());
   edisp.setTableData("LT2", irfTables["lt2"].values());
   edisp.setTableData("RT2", irfTables["rt2"].values());
   edisp.setClassName(latclass);
   edisp.close();
}

void readClassNames(const std::string & rootfile,
                    std::vector<std::string> & classNames) {
   classNames.clear();
   typedef std::map<std::string, handoff_response::IrfEval *> IrfEvalMap_t;
   IrfEvalMap_t irfEvalMap;
   handoff_response::RootEval::createMap(rootfile, irfEvalMap);
   for (IrfEvalMap_t::const_iterator item(irfEvalMap.begin());
        item != irfEvalMap.end(); ++item) {
      classNames.push_back(item->first);
   }
}

int main(int iargc, char * argv[]) {
   try {
      std::string rootfile;
      if (iargc == 1) { /// use default input filename
         char * rootpath = ::getenv("HANDOFF_RESPONSEROOT");
         if (rootpath == 0) {
            throw std::runtime_error("HANDOFF_RESPONSEROOT not set");
         }
         rootfile = rootpath + std::string("/data/parameters.root");
      } else {
         rootfile = argv[1];
      }
      std::vector<std::string> classNames;
      readClassNames(rootfile, classNames);
      std::cout << "Creating FITS files for event classes: \n";
      for (std::vector<std::string>::const_iterator name(classNames.begin());
           name != classNames.end(); ++name) {
         std::cout << *name << std::endl;
         createFitsFiles(*name, rootfile);
      }
   } catch (std::exception & eobj) {
      std::cout << eobj.what() << std::endl;
   } 
   return 0;
}
