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

#include "st_facilities/Util.h"

#include "fits/IrfTableMap.h"
#include "fits/FitsFile.h"

using handoff_response::IrfTableMap;
using handoff_response::FitsFile;

void createFiles(const std::string & className,
                 const std::string & section,
                 const std::string & rootfile="parameters.root") {
   IrfTableMap irfTables(className + "::" + section, rootfile);
   std::string latclass(className + "_" + section);
      
// Effective area
   FitsFile aeff("aeff_" + latclass + ".fits", "EFFECTIVE AREA", "aeff.tpl");
   aeff.setGrid(irfTables["aeff"]);
   aeff.setTableData("EFFAREA", irfTables["aeff"].values());
   aeff.setKeyword("LATCLASS", latclass);
   aeff.close();

// Point spread function and angular deviation scaling parameters
   std::string psf_file("psf_" + latclass + ".fits");
   FitsFile psf(psf_file, "RPSF", "psf.tpl");
   psf.setGrid(irfTables["ncore"]);
   psf.setTableData("NCORE", irfTables["ncore"].values());
   psf.setTableData("NTAIL", irfTables["ntail"].values());
   psf.setTableData("SIGMA", irfTables["sigma"].values());
   psf.setTableData("GCORE", irfTables["gcore"].values());
   psf.setTableData("GTAIL", irfTables["gtail"].values());
   psf.setKeyword("LATCLASS", latclass);
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
   edisp.setKeyword("LATCLASS", latclass);
   edisp.close();
}

void readClassNames(const std::string & infile,
                    std::vector<std::string> & classNames) {
   std::vector<std::string> lines;
   st_facilities::Util::readLines(infile, lines);
   classNames.clear();
   for (size_t i = 0; i < lines.size(); i++) {
      std::vector<std::string> tokens;
      facilities::Util::stringTokenize(lines.at(i), " \t", tokens);
      classNames.push_back(tokens.front());
   }
}

int main(int iargc, char * argv[]) {
   try {
      std::string infile;
      if (iargc == 1) { 
         // use default input filename
         char * rootpath = ::getenv("HANDOFF_RESPONSEROOT");
         if (rootpath == 0) {
            throw std::runtime_error("HANDOFF_RESPONSEROOT not set");
         }
         infile = rootpath + std::string("/data/classnames.dat");
      } else {
         infile = argv[1];
      }
      std::vector<std::string> classNames;
      readClassNames(infile, classNames);
      for (std::vector<std::string>::const_iterator name(classNames.begin());
           name != classNames.end(); ++name) {
         createFiles(*name, "front");
         createFiles(*name, "back");
      }
   } catch (std::exception & eobj) {
      std::cout << eobj.what() << std::endl;
   } 
   return 0;
}
