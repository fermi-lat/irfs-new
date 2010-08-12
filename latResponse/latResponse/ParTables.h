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
                      std::vector<double> & cornerEnergies,
                      std::vector<std::vector<double> > & parVectors) const;

   const std::vector<double> & logEnergies() const {
      return m_parTables.begin()->second.logEnergies();
//      return m_parTables.begin()->logEnergies();
   }

   const std::vector<double> & costhetas() const {
      return m_parTables.begin()->second.costhetas();
//      return m_parTables.begin()->costhetas();
   }

   void getPars(size_t ilogE, size_t icosth, std::vector<double> & pars) const;
   
   void setPars(size_t ilogE, size_t icosth, const std::vector<double> & pars);

private:

   std::vector<std::string> m_parNames;
   std::map<std::string, FitsTable> m_parTables;
//   std::vector<FitsTable> m_parTables;

};

} // namespace latResponse

#endif // latResponse_ParTables_h
