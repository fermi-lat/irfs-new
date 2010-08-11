/**
 * @file ParTables.h
 * @brief Class to manage tables of parameters for IRF tables.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_ParTables_h
#define latResponse_ParTables_h

#include <map>
#include <string>
#include <vector>

#include "latResponse/FitsTable.h"

namespace latResponse {

/**
 * @class ParTables
 * @brief Class to manage tables of parameters for IRF tables.
 */

class ParTables {

public:

   ParTables(const std::string & fitsfile,
             const std::string & extname,
             size_t nrow=0);

   const FitsTable & operator[](const std::string & parName) const;

   const std::vector<std::string> & parNames() const {
      return m_parNames;
   }

   void getPars(double loge, double costh, double * pars,
                bool interpolate=true) const;

   void getParVector(const std::string & parName,
                     std::vector<double> & pars) const;

   void getCornerPars(double logE, double costh, double & tt, double & uu,
                      std::vector<std::vector<double> > & parVectors) const;


private:

   std::vector<std::string> m_parNames;
   std::map<std::string, FitsTable> m_parTables;

};

} // namespace latResponse

#endif // latResponse_ParTables_h
