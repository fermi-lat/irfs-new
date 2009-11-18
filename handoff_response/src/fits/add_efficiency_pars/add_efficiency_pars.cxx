/**
 * @file add_efficiency_pars.cxx
 * @brief Program for adding efficiency correction paramters to the 
 * effective area FITS files.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>

#include "facilities/Util.h"

#include "st_facilities/Util.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "../src/fits/FitsFile.h"

class AddPars : public st_app::StApp {

public:
   AddPars() : st_app::StApp(),
                m_pars(st_app::StApp::getParGroup("add_efficiency_pars")) {
      try {
         setVersion(s_cvs_id);
      } catch (std::exception & eObj) {
         std::cerr << eObj.what() << std::endl;
         std::exit(1);
      } catch (...) {
         std::cerr << "Caught unknown exception in AddPars constructor." 
                   << std::endl;
         std::exit(1);
      }
   }

   virtual ~AddPars() throw() {
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

   void readParValues(std::vector< std::vector<double> > & pars) const;

   void insertParValues(const std::vector< std::vector<double> > & pars) const;

   static std::string s_cvs_id;

};

std::string AddPars::s_cvs_id("$Name$");

st_app::StAppFactory<AddPars> myAppFactory("add_efficiency_pars");

void AddPars::banner() const {
   int verbosity = m_pars["chatter"];
   if (verbosity > 2) {
      st_app::StApp::banner();
   }
}

void AddPars::run() {
   m_pars.Prompt();
   m_pars.Save();

   std::vector< std::vector<double> > parValues;

   readParValues(parValues);

   insertParValues(parValues);
}

void AddPars::readParValues(std::vector< std::vector<double> > & pars) const {
   std::string parfile = m_pars["infile"];
   facilities::Util::expandEnvVar(&parfile);
   st_facilities::Util::file_ok(parfile);
   std::vector<std::string> lines;
   st_facilities::Util::readLines(parfile, lines, "#", true);

   pars.clear();
   for (size_t i(0); i < lines.size(); i++) {
      std::vector<std::string> tokens;
      bool clear;
      facilities::Util::stringTokenize(lines.at(i), " ", tokens, clear=true);
      std::vector<double> row;
      for (size_t j(0); j < tokens.size(); j++) {
         row.push_back(std::atof(tokens.at(j).c_str()));
      }
      pars.push_back(row);
   }
}

void AddPars::
insertParValues(const std::vector< std::vector<double> > & pars) const {
   bool newFile;
   size_t numRows;
   handoff_response::FitsFile aeff(m_pars["outfile"], "EFFICIENCY_PARAMS",
                                   "aeff.tpl", newFile=false,
                                   numRows=pars.size());

   for (size_t i(0); i < pars.size(); i++) {
      aeff.setVectorData("EFFICIENCY_PARS", pars.at(i), i+1);
   }
   aeff.close();
}
