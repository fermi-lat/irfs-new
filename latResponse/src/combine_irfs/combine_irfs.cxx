/**
 * @file combine_irfs.cxx
 * @brief Combine IRF FITS files so that IRFs for event classes can be
 * combined in a single FITS file.
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_facilities/FitsUtil.h"
#include "st_facilities/Util.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

class CombineIrfs : public st_app::StApp {
public:
   CombineIrfs() : st_app::StApp(),
                   m_pars(st_app::StApp::getParGroup("combine_irfs")) {
      try {
         setVersion(s_cvs_id);
      } catch (std::exception & eObj) {
         std::cerr << eObj.what() << std::endl;
         std::exit(1);
      } catch (...) {
         std::cerr << "Caught unknown exception in CombineIrfs constructor." 
                   << std::endl;
         std::exit(1);
      }
   }

   virtual ~CombineIrfs() throw() {
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

   std::map<std::string, std::string> m_cbdValues;

   void readBoundaryKeywords(const tip::Table * table);

   void setCbdValue(tip::Header & header,
                    const std::string & cbdKey,
                    const std::string & cbdValue);
};

std::string CombineIrfs::s_cvs_id("$Name$");

st_app::StAppFactory<CombineIrfs> myAppFactory("combine_irfs");

void CombineIrfs::banner() const {
   int verbosity = m_pars["chatter"];
   if (verbosity > 2) {
      st_app::StApp::banner();
   }
}

std::string CombineIrfs::par(const std::string & key) const {
   std::string value = m_pars[key.c_str()];
   return value;
}

void CombineIrfs::run() {
   m_pars.Prompt();
   m_pars.Save();

   std::vector<std::string> infiles;
   st_facilities::Util::readLines(par("filelist"), infiles);

   std::string outfile(par("outfile"));

   std::string extname;
   st_facilities::FitsUtil::getFitsHduName(infiles.front(), 2, extname);

   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());

   fileSvc.createFile(outfile, infiles.front());

   tip::Table * outtable(fileSvc.editTable(outfile, extname));

   outtable->setNumRecords(infiles.size());

   tip::Table::Iterator output_itor(outtable->begin());
   tip::Table::Record & output(*output_itor);

   for (size_t i(0); i < infiles.size(); i++) {
      const tip::Table * table = 
         fileSvc.readTable(infiles.at(i), extname);
      tip::Table::ConstIterator input_itor(table->begin());
      tip::ConstTableRecord & input(*input_itor);
      output = input;
      ++output_itor;
      delete table;
   }

   readBoundaryKeywords(outtable);

   setCbdValue(outtable->getHeader(), "VERSION", par("class"));
   setCbdValue(outtable->getHeader(), "CLASS", par("class"));

   delete outtable;
}

void CombineIrfs::readBoundaryKeywords(const tip::Table * table) {
   const char * cbd_keys[] = {"CBD10001", "CBD20001", "CBD30001", 
                              "CBD40001", "CBD50001", "CBD60001", 
                              "CBD70001", "CBD80001", "CBD90001",
                              NULL};
   const tip::Header & header(table->getHeader());

   std::vector< std::pair<std::string, std::string> > keyvals;
   header.get(cbd_keys, keyvals);
   for (size_t i(0); i < keyvals.size(); i++) {
      m_cbdValues[keyvals.at(i).first] = keyvals.at(i).second;
   }
}

void CombineIrfs::setCbdValue(tip::Header & header,
                              const std::string & cbdKey,
                              const std::string & cbdValue) {
   std::string keyValue(cbdKey + "(" + cbdValue + ")");

   std::vector<std::string> emptyKeys;

   std::map<std::string, std::string>::iterator it(m_cbdValues.begin());
   for ( ; it != m_cbdValues.end(); ++it) {
      if (it->second.find(cbdKey) != std::string::npos) {
         header[it->first].set(keyValue);
         it->second = keyValue;
         return;
      }
      if (it->second == "NONE") {
         emptyKeys.push_back(it->first);
      }
   }

   if (!emptyKeys.empty()) {
      header[emptyKeys.at(0)].set(keyValue);
   } else {
      throw std::runtime_error("No more remaining CBD keywords to "
                               "contain the requested key-value pair.");
   }
}
