/**
 * @file FitsEval.cxx
 * @brief Interface class to access and evalute FITS versions of IRF tables.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <cctype>
#include <stdexcept>

#include "facilities/commonUtilities.h"

#include "st_facilities/Env.h"

#include "irfUtil/Util.h"

#include "../gen/PointSpreadFunction.h"
#include "../gen/Dispersion.h"

#include "FitsEval.h"
#include "FitsTable.h"

#include "Table.h"
namespace {
   std::string fullpath(const std::string & basename) {
      char * rootPath = ::getenv("HANDOFF_IRF_DIR");
      if (rootPath == 0) {
         throw std::runtime_error("HANDOFF_IRF_DIR env var not set.");
      }
      return st_facilities::Env::appendFileName(std::string(rootPath),
                                                basename);
   }
   void toUpper(std::string & name) {
      for (std::string::iterator it = name.begin(); it != name.end(); ++it) {
         *it = std::toupper(*it);
      }
   }
   std::string caldbFile(const std::string & respname,
                         std::string section,
                         const std::string & version,
                         const std::string & date) {
      toUpper(section);
      std::string filename;
      long hdu;
      irfUtil::Util::getCaldbFile(section, respname, version,
                                  filename, hdu, "GLAST", "LAT",
                                  "NONE", date, "00:00:00");
      return filename;
   }
}

namespace handoff_response {

FitsEval::FitsEval(const std::string & className,
                   const std::string & section,
                   const std::string & version,
                   const std::string & date,
                   bool forceCaldb) 
   : RootEval(className + "/" + section), m_className(className), 
     m_section(section), m_version(version), m_date(date), 
     m_forceCaldb(forceCaldb) {
   m_dataPath = facilities::commonUtilities::getDataPath("handoff_response");
   readAeff();
   readEdisp();
   readPsf();
}

FitsEval::~FitsEval() {
   delete m_aeff;
   for (size_t i(0); i < m_dispTables.size(); i++) {
      delete m_dispTables.at(i);
   }
   for (size_t i(0); i < m_psfTables.size(); i++) {
      delete m_psfTables.at(i);
   }
}

void FitsEval::readAeff() {
   FitsTable aeff(aeffFile(), "EFFECTIVE AREA");
   m_aeff = new Table(aeff.tableData("EFFAREA"));
}

void FitsEval::readEdisp() {
   FitsTable edisp(edispFile(), "ENERGY DISPERSION");
   const std::vector<std::string> & names(Dispersion::pnames);
   m_dispTables.clear();
   for (size_t i(0); i < names.size(); i++) {
      m_dispTables.push_back(new Table(edisp.tableData(names.at(i))));
   }
}

void FitsEval::readPsf() {
   FitsTable psf(psfFile(), "RPSF");
   const std::vector<std::string> & names(PointSpreadFunction::pnames);
   m_psfTables.clear();
   for (size_t i(0); i < names.size(); i++) {
      m_psfTables.push_back(new Table(psf.tableData(names.at(i))));
   }
}

std::string FitsEval::aeffFile() const {
//    if (::getenv("HANDOFF_IRF_DIR") && !m_forceCaldb) {
//       return ::fullpath("aeff_" + m_className + "_" + m_section + ".fits");
//    }
//    return caldbFile("EFF_AREA", m_section, m_version, m_date);
   return facilities::commonUtilities::
      joinPath(m_dataPath, "aeff_P6_v1_trans_" + m_section + ".fits");
}

std::string FitsEval::edispFile() const {
//    if (::getenv("HANDOFF_IRF_DIR") && !m_forceCaldb) {
//       return ::fullpath("edisp_" + m_className + "_" + m_section + ".fits");
//       std::cout<<"read (1) "<<::fullpath("edisp_" + m_className + "_" + m_section + ".fits")<<std::endl;
//    }
//    return caldbFile("EDISP", m_section, m_version, m_date);
//    std::cout<<"read (2) "<<caldbFile("EDISP", m_section, m_version, m_date)<<std::endl;
   return facilities::commonUtilities::
      joinPath(m_dataPath, "edisp_P6_v1_trans_" + m_section + ".fits");
}

std::string FitsEval::psfFile() const {
//    if (::getenv("HANDOFF_IRF_DIR") && !m_forceCaldb) {
//       return ::fullpath("psf_" + m_className + "_" + m_section + ".fits");
//    }
//    return caldbFile("RPSF", m_section, m_version, m_date);
   return facilities::commonUtilities::
      joinPath(m_dataPath, "psf_P6_v1_trans_" + m_section + ".fits");
}

void FitsEval::createMap(const std::string & className,
                         std::map<std::string, IrfEval *> & evals) {
   evals.clear();
   addToMap(className, evals);
}

void FitsEval::addToMap(const std::string & className,
                        std::map<std::string, IrfEval *> & evals) {
   evals[className + "/front"] = new FitsEval(className, "front");
   evals[className + "/back"] = new FitsEval(className, "back");
}



} // namespace handoff_response
