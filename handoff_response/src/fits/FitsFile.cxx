/**
 * @file FitsFile.cxx
 * @brief Write out binary table data for handoff_response FITS files.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "st_facilities/Env.h"
#include "st_facilities/FitsUtil.h"
#include "st_facilities/Util.h"

#include "FitsFile.h"
#include "IrfTable.h"

namespace {
   void toUpper(std::string & name) {
      for (std::string::iterator it = name.begin(); it != name.end(); ++it) {
         *it = std::toupper(*it);
      }
   }
} // anonymous namespace

namespace handoff_response {

FitsFile::FitsFile(const std::string & outfile, 
                   const std::string & extname,
                   const std::string & templateFile,
                   bool newFile, size_t numRows) 
   : m_fptr(0), m_numRows(numRows), m_outfile(outfile), m_extname(extname) {
   prepareFile(outfile, extname, templateFile, newFile);
   int status(0);
   std::string filename(outfile + "[" + extname +"]");
   fits_open_file(&m_fptr, filename.c_str(), READWRITE, &status);
   fitsReportError(status, "FitsFile::FitsFile");
}

FitsFile::~FitsFile() throw() {
   try {
      if (m_fptr) {
         close();
      }
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
   } catch (...) {
   }
}

void FitsFile::close() {
   int status(0);
   fits_close_file(m_fptr, &status);
   fitsReportError(status, "FitsFile::~FitsFile");
   m_fptr = 0;
   st_facilities::FitsUtil::writeChecksums(m_outfile);
   setDateKeyword();
}

void FitsFile::setDateKeyword() {
   std::string date(st_facilities::Util::currentTime().getGregorianDate());
   setKeyword("DATE", date);
}

void FitsFile::setVectorData(const std::string & fieldname,
                             const std::vector<double> & data,
                             size_t row) {
   if (row > m_numRows) {
      throw std::runtime_error("FitsFile::setVectorData:\n"
                               "Attempt to fill row beyond end of table.");
   }

   int status(0);
   int colnum(fieldNum(fieldname));
   fits_modify_vector_len(m_fptr, colnum, data.size(), &status);
   fitsReportError(status, "FitsFile::setVectorData");

   fits_write_col(m_fptr, TDOUBLE, colnum, row, 1, data.size(),
                  &const_cast<std::vector<double> &>(data)[0],
                  &status);
   fitsReportError(status, "FitsFile::setVectorData");
}

void FitsFile::setTableData(const std::string & fieldname,
                            const std::vector<double> & data,
                            size_t row) {
   setVectorData(fieldname, data, row);
   int colnum = fieldNum(fieldname);
   std::ostringstream tdimkey;
   tdimkey << "TDIM" << colnum;
   setKeyword(tdimkey.str(), m_tdim);
}

void FitsFile::setGrid(const IrfTable & table) {
   setGrid(table.xaxis(), table.yaxis());
}

void FitsFile::setGrid(const std::vector<double> & logEs,
                       const std::vector<double> & mus) {
   std::vector<double> elo;
   std::vector<double> ehi;
   for (size_t k = 0; k < logEs.size()-1; k++) {
      elo.push_back(std::pow(10., logEs.at(k)));
      ehi.push_back(std::pow(10., logEs.at(k+1)));
   }
   setVectorData("ENERG_LO", elo);
   setVectorData("ENERG_HI", ehi);

   std::vector<double> mulo;
   std::vector<double> muhi;
   for (size_t k = 0; k < mus.size()-1; k++) {
      mulo.push_back(mus.at(k));
      muhi.push_back(mus.at(k+1));
   }
   setVectorData("CTHETA_LO", mulo);
   setVectorData("CTHETA_HI", muhi);

   std::ostringstream tdim;
   tdim << "(" << elo.size() << ", "
        << mulo.size() << ")";
   m_tdim = tdim.str();
}

int FitsFile::fieldNum(const std::string & fieldName) const {
   for (size_t i = 0; i < m_fieldNames.size(); i++) {
      if (fieldName == m_fieldNames.at(i)) {
         return i + 1;
      }
   }
   throw std::runtime_error("FitsFile::fieldNum:\nField named " 
                            + fieldName + " not found.");
   return 0;
}

void FitsFile::prepareFile(const std::string & outfile, 
                           const std::string & extname,
                           const std::string & templateFile,
                           bool newFile) {

   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());
   
   if (newFile) {
      if (st_facilities::Util::fileExists(outfile)) {
         std::remove(outfile.c_str());
      }
      std::string tplFile = 
         st_facilities::Env::appendFileName(
            st_facilities::Env::getDataDir("handoff_response"), templateFile);
      fileSvc.createFile(outfile, tplFile);
   }

   tip::Table * table = fileSvc.editTable(outfile, extname);
   table->setNumRecords(m_numRows);

   m_fieldNames = table->getValidFields();
   for (size_t i = 0; i < m_fieldNames.size(); i++) {
      ::toUpper(m_fieldNames.at(i));
   }

   readBoundaryKeywords(table);

   delete table;
}

void FitsFile::readBoundaryKeywords(const tip::Table * table) {
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

void FitsFile::setCbdValue(const std::string & cbdKey,
                           const std::string & cbdValue) {
   std::string keyValue(cbdKey + "(" + cbdValue + ")");

   std::vector<std::string> emptyKeys;

   std::map<std::string, std::string>::iterator it(m_cbdValues.begin());
   for ( ; it != m_cbdValues.end(); ++it) {
      if (it->second.find(cbdKey) != std::string::npos) {
         setKeyword(it->first, keyValue);
         it->second = keyValue;
         return;
      }
      if (it->second == "NONE") {
         emptyKeys.push_back(it->first);
      }
   }

   if (!emptyKeys.empty()) {
      setKeyword(emptyKeys.at(0), keyValue);
   } else {
      throw std::runtime_error("No more remaining CBD keywords to "
                               "contain the requested key-value pair.");
   }
}

void FitsFile::fitsReportError(int status, const std::string & routine) const {
   if (status == 0) {
      return;
   }
   fits_report_error(stderr, status);
   std::ostringstream message;
   message << routine << ": CFITSIO error " << status;
   throw std::runtime_error(message.str());
}

} // namespace handoff_response
