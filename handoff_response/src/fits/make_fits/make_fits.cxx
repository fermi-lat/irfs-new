/**
 * @file make_aeff.cxx
 * @brief Program for converting ROOT tables to FITS.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>
#include <cstdlib>

#include <iostream>
#include <stdexcept>

#include "facilities/Util.h"

#include "st_facilities/Util.h"

#include "st_stream/StreamFormatter.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "../src/irfs/RootEval.h"

#include "../src/fits/IrfTableMap.h"
#include "../src/fits/FitsFile.h"

class MakeFits : public st_app::StApp {
public:
   MakeFits() : st_app::StApp(),
                m_pars(st_app::StApp::getParGroup("make_fits")) {
      try {
         setVersion(s_cvs_id);
      } catch (std::exception & eObj) {
         std::cerr << eObj.what() << std::endl;
         std::exit(1);
      } catch (...) {
         std::cerr << "Caught unknown exception in MakeFits constructor." 
                   << std::endl;
         std::exit(1);
      }
   }

   virtual ~MakeFits() throw() {
      try {
      } catch (std::exception &eObj) {
         std::cerr << eObj.what() << std::endl;
      } catch (...) {
      }  
   }

   virtual void run();
   virtual void banner() const;

private:

   st_app::AppParGroup & m_pars;

   std::string par(const std::string & key) const;

   static std::string s_cvs_id;

   void parseName(const std::string & rootClassName,
                  std::string & className, 
                  std::string & section);

   void createFitsFiles(const std::string & rootClassName,
                        const std::string & rootfile);

   void readClassNames(const std::string & rootfile,
                       std::vector<std::string> & classNames);

};

std::string MakeFits::s_cvs_id("$Name$");

st_app::StAppFactory<MakeFits> myAppFactory("make_fits");

void MakeFits::banner() const {
   int verbosity = m_pars["chatter"];
   if (verbosity > 2) {
      st_app::StApp::banner();
   }
}

std::string MakeFits::par(const std::string & key) const {
   std::string value = m_pars[key.c_str()];
   return value;
}

void MakeFits::parseName(const std::string & rootClassName,
                         std::string & className, 
                         std::string & section) {
   std::vector<std::string> tokens;
   facilities::Util::stringTokenize(rootClassName, "/", tokens);
   className = tokens.at(0);
   section = tokens.at(1);
}

void MakeFits::createFitsFiles(const std::string & rootClassName,
                               const std::string & rootfile) {
   using handoff_response::IrfTableMap;
   using handoff_response::FitsFile;

   bool newFile;

   std::string className, section;
   parseName(rootClassName, className, section);
   IrfTableMap irfTables(className + "::" + section, rootfile);
   std::string irfVersion(par("IRF_version"));
   std::string latclass(className + "_" + section);
   std::string detname;
   if (rootClassName.find("front") != std::string::npos) {
      detname = "FRONT";
   } else if (rootClassName.find("back") != std::string::npos) {
      detname = "BACK";
   } else {
      throw std::runtime_error("Unrecognized detector name in class "
                               + rootClassName);
   }
      
// Effective area
   FitsFile aeff("aeff_" + latclass + ".fits", "EFFECTIVE AREA", "aeff.tpl");
   aeff.setGrid(irfTables["aeff"]);
   aeff.setTableData("EFFAREA", irfTables["aeff"].values());
   aeff.setCbdValue("VERSION", irfVersion);
   aeff.setCbdValue("CLASS", latclass);
   aeff.setKeyword("DETNAM", detname);
   aeff.close();

// Phi-dependence parameters
   FitsFile phi_dep("aeff_" + latclass + ".fits", "PHI_DEPENDENCE", "aeff.tpl",
                    newFile=false);
   phi_dep.setGrid(irfTables["phi_dep_0"]);
   phi_dep.setTableData("PHIDEP0", irfTables["phi_dep_0"].values());
   phi_dep.setTableData("PHIDEP1", irfTables["phi_dep_1"].values());
   phi_dep.setCbdValue("VERSION", irfVersion);
   phi_dep.setCbdValue("CLASS", latclass);
   phi_dep.setKeyword("DETNAM", detname);
   phi_dep.close();

// Point spread function and angular deviation scaling parameters
   std::string psf_file("psf_" + latclass + ".fits");
   FitsFile psf(psf_file, "RPSF", "psf.tpl");
   psf.setGrid(irfTables["ncore"]);
   psf.setTableData("NCORE", irfTables["ncore"].values());
   psf.setTableData("SIGMA", irfTables["sigma"].values());
   psf.setTableData("GCORE", irfTables["gcore"].values());
   psf.setTableData("GTAIL", irfTables["gtail"].values());
   psf.setCbdValue("VERSION", irfVersion);
   psf.setCbdValue("CLASS", latclass);
   psf.setKeyword("DETNAM", detname);
   psf.close();
   
   /// @bug These are hard-wired values from
   /// gen/PointSpreadFunction::scaleFactor!
   double scaling_pars[] = {5.8e-2, 3.77e-4, 9.6e-2, 1.3e-3, -0.8};
   std::vector<double> scalingPars(scaling_pars, scaling_pars + 5);
   FitsFile psfScaling(psf_file, "PSF_SCALING_PARAMS", "psf.tpl", 
                       newFile=false);
   psfScaling.setTableData("PSFSCALE", scalingPars);
   psfScaling.close();

// Energy dispersion
   std::string edisp_file("edisp_" + latclass + ".fits");
   FitsFile edisp(edisp_file, "ENERGY DISPERSION", "edisp.tpl");
   edisp.setGrid(irfTables["norm"]);
   edisp.setTableData("NORM", irfTables["norm"].values());
   edisp.setTableData("LS1", irfTables["ls1"].values());
   edisp.setTableData("RS1", irfTables["rs1"].values());
   edisp.setTableData("BIAS", irfTables["bias"].values());
   edisp.setTableData("LS2", irfTables["ls2"].values());
   edisp.setTableData("RS2", irfTables["rs2"].values());
   edisp.setCbdValue("VERSION", irfVersion);
   edisp.setCbdValue("CLASS", latclass);
   edisp.setKeyword("DETNAM", detname);
   edisp.setKeyword("EDISPVER", 2);
   edisp.close();

   /// @bug These are hard-wired values in gen/Dispersion::scaleFactor()!!
   double edisp_front[] = {0.0210, 0.058, -0.207, -0.213, 0.042, 0.564};
   double edisp_back[] = {0.0215, 0.0507, -0.22, -0.243, 0.065, 0.584};
   
   if (detname == "FRONT") {
      scalingPars = std::vector<double>(edisp_front, edisp_front + 6);
   } else { // BACK
      scalingPars = std::vector<double>(edisp_back, edisp_back + 6);
   }

   /// @bug Append other hard-wired values from gen/Dispersion 
   // anonymous namespace:
   scalingPars.push_back(1.6);
   scalingPars.push_back(0.6);
   scalingPars.push_back(1.5);

   FitsFile edispScaling(edisp_file, "EDISP_SCALING_PARAMS", "edisp.tpl",
                         newFile=false);
   edispScaling.setTableData("EDISPSCALE", scalingPars);
   edispScaling.close();
}

void MakeFits::readClassNames(const std::string & rootfile,
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

void MakeFits::run() {
   m_pars.Prompt();
   m_pars.Save();

   std::string rootfile(par("root_file"));
   std::vector<std::string> classNames;
   readClassNames(rootfile, classNames);
   st_stream::StreamFormatter formatter("MakeFits", "run", 2);
   formatter.info() << "Creating FITS files for event classes: \n";
   for (std::vector<std::string>::const_iterator name(classNames.begin());
        name != classNames.end(); ++name) {
      formatter.info() << *name << std::endl;
      createFitsFiles(*name, rootfile);
   }
}
