/**
 * @file FitsTable.h
 * @brief Class to manage tabular IRF data which are assumed to be a
 * function of log10(E/MeV) and cos(theta) and are read from a FITS
 * file.
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#ifndef latResponse_FitsTable_h
#define latResponse_FitsTable_h

#include <map>
#include <vector>

namespace tip {
   class Table;
}

namespace latResponse {

class Bilinear;

class FitsTable {

public:

   FitsTable(const std::string & filename,
             const std::string & extname,
             const std::string & tablename,
             size_t nrow=0);

   FitsTable();

   FitsTable(const FitsTable & other);

   ~FitsTable();
      
   /// @brief lookup a value from the table
   /// @param logenergy log10(energy)
   /// @param costh cos(theta)
   /// @param interpolate [true] if true, make linear
   /// interpolation. Otherwise, return value for given cell.
   double value(double logenergy, double costh, bool interpolate=true) const;
    
   double maximum() const {
      return m_maxValue;
   }
   
   double minCosTheta() const {
      return m_minCosTheta;
   }

   static void getVectorData(const tip::Table * table,
                             const std::string & fieldName,
                             std::vector<float> & values,
                             size_t nrow=0);

protected:

   /// Disable copy assignment operator.
   FitsTable & operator=(const FitsTable &) {
      return *this;
   }

private:

   Bilinear * m_interpolator;

   std::vector<float> m_logEnergies; 
   std::vector<float> m_mus; 
   std::vector<float> m_values;

   std::vector<float> m_ebounds;
   std::vector<float> m_tbounds;
   
   float m_minCosTheta;

   float m_maxValue;

};

} // namespace latResponse

#endif // latResponse_FitsTable_h
