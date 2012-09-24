/**
 * @file FitsTable.cxx
 * @brief Abstraction for reading IRF FITS binary tables.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>
#include <cctype>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "TDirectory.h"
#include "TH2F.h"

#include "facilities/Util.h"

#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "FitsTable.h"

namespace {
   void toUpper(std::string & name) {
      for (std::string::iterator it = name.begin(); it != name.end(); ++it) {
         *it = std::toupper(*it);
      }
   }
} // anonymous namespace

namespace handoff_response {

FitsTable::FitsTable(const std::string & filename,
                     const std::string & extname)
   : m_table(tip::IFileSvc::instance().readTable(filename, extname)),
     m_filename(filename) {
   m_fieldNames = m_table->getValidFields();
   for (size_t i(0); i < m_fieldNames.size(); i++) {
      ::toUpper(m_fieldNames.at(i));
   }
}

FitsTable::~FitsTable() {
   delete m_table;
}

void FitsTable::getVectorData(const std::string & fieldName,
                              std::vector<double> & values) const {
   values.clear();

   tip::Table::ConstIterator it(m_table->begin());
   tip::ConstTableRecord & row(*it);

   row[fieldName].get(values);
}

void FitsTable::
getTableData(const std::string & fieldName,
             std::vector< std::vector<double> > & values) const {
   std::vector<double> my_values;
   getVectorData(fieldName, my_values);
   size_t xdim, ydim;
   getTableDims(fieldName, xdim, ydim);
   size_t indx(0);
   for (size_t j(0); j < ydim; j++) {
      std::vector<double> row;
      for (size_t i(0); i < xdim; i++, indx++) {
         row.push_back(my_values.at(indx));
      }
      values.push_back(row);
   }
}

TH2F * FitsTable::tableData(const std::string & fieldName) const {
   std::string label(m_filename + "_" + fieldName);
   std::string title(fieldName + "; log energy; costheta");

   std::vector<double> elo, ehi, logE;
   getVectorData("ENERG_LO", elo);
   getVectorData("ENERG_HI", ehi);

   for (size_t k(0); k < elo.size(); k++) {
      logE.push_back(std::log10(elo.at(k)));
   }
   logE.push_back(std::log10(ehi.back()));

   std::vector<double> mu, muhi;
   getVectorData("CTHETA_LO", mu);
   getVectorData("CTHETA_HI", muhi);
   mu.push_back(muhi.back());

   std::vector< std::vector<double> > values;
   getTableData(fieldName, values);

   TH2F * foo = (TH2F *)(gDirectory->GetList()->FindObject(label.c_str()));
   delete foo;

   TH2F * h2 = new TH2F(label.c_str(), title.c_str(), 
                        logE.size()-1, &logE[0],
                        mu.size()-1, &mu[0]);

   for (size_t i(0); i < logE.size() - 1; i++) {
      for (size_t j(0); j < mu.size() - 1; j++) {
         h2->SetBinContent(i+1, j+1, values.at(j).at(i));
      }
   }

   return h2;
}

void FitsTable::getTableDims(const std::string & fieldName, 
                             size_t & xdim, size_t & ydim) const {
   std::ostringstream keyname;
   keyname << "TDIM" << fieldNum(fieldName);
   const tip::Header & header(m_table->getHeader());
   std::string keyval;
   header[keyname.str()].get(keyval);
   
// strip parentheses
   std::string::size_type endpos(keyval.find(")") - 1);
   std::string dims(keyval.substr(1, endpos));

   std::vector<std::string> tokens;
   facilities::Util::stringTokenize(dims, " ,", tokens);
   xdim = std::atoi(tokens.at(0).c_str());
   ydim = std::atoi(tokens.at(1).c_str());
}

size_t FitsTable::fieldNum(const std::string & fieldName) const {
   std::string name(fieldName);
   ::toUpper(name);
   for (size_t i = 0; i < m_fieldNames.size(); i++) {
      if (name == m_fieldNames.at(i)) {
         return i + 1;
      }
   }
   for (size_t i = 0; i < m_fieldNames.size(); i++) {
      std::cout << m_fieldNames.at(i) << std::endl;
   }
   throw std::runtime_error("FitsTable::fieldNum:\nField named " 
                            + fieldName + " not found.");
   return 0;
}

} //namespace handoff_response
