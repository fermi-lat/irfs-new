/**
 * @file ParTables.cxx
 * @brief Implementation for class to manage tables of parameters for
 * IRF tables.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <sstream>
#include <stdexcept>

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "latResponse/ParTables.h"

namespace latResponse {

ParTables::ParTables(const std::string & fitsfile,
                     const std::string & extname,
                     size_t nrow) {
   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());
   const tip::Table * table(fileSvc.readTable(fitsfile, extname));
   const std::vector<std::string> & validFields(table->getValidFields());

   // The first four columns *must* be "ENERG_LO", "ENERG_HI", "CTHETA_LO",
   // "CTHETA_HI", in that order.
   char * boundsName[] = {"energ_lo", "energ_hi", "ctheta_lo", "ctheta_hi"};
   for (size_t i(0); i < 4; i++) {
      if (validFields.at(i) != boundsName[i]) {
         std::ostringstream message;
         message << "latResponse::ParTables::ParTables: "
                 << "invalid header in " << fitsfile << "  "
                 << validFields.at(i) << "  " << i;
         throw std::runtime_error(message.str());
      }
   }

   // Read in the table values for the remaining columns.
   for (size_t i(4); i < validFields.size(); i++) {
      const std::string & tablename(validFields.at(i));
      m_parNames.push_back(tablename);
      m_parTables.insert(
         std::map<std::string, FitsTable>::
         value_type(tablename, FitsTable(fitsfile, extname, tablename, nrow)));
   }
   delete table;
}

const FitsTable & ParTables::operator[](const std::string & parName) const {
   std::map<std::string, FitsTable>::const_iterator table =
      m_parTables.find(parName);
   if (table == m_parTables.end()) {
      throw std::runtime_error("latResponse::ParTables::operator[]: "
                               "table name not found.");
   }
   return table->second;
}

void ParTables::getPars(double loge, double costh, double * pars,
                        bool interpolate) const {
   for (size_t i(0); i < m_parNames.size(); i++) {
      pars[i] = operator[](m_parNames.at(i)).value(loge, costh, interpolate);
   }
}

void ParTables::getParVector(const std::string & parName,
                             std::vector<double> & pars) const {
   operator[](parName).getValues(pars);
}

} // namespace latResponse
