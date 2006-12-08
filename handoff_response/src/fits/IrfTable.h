/**
 * @file IrfTable.h
 * @brief Abstraction for a ROOT TH2F table.
 * 
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef handoff_response_IrfTables_h
#define handoff_response_IrfTables_h

#include <vector>

class TH2F;

namespace handoff_response {

/**
 * @class IrfTable
 * @brief Abstraction for a ROOT TH2F table.
 * 
 * @author J. Chiang
 */

class IrfTable {

public:

   IrfTable() {}

   IrfTable(TH2F * table);

   const std::vector<double> & xaxis() const {
      return m_xaxis;
   }

   const std::vector<double> & yaxis() const {
      return m_yaxis;
   }

   const std::vector<double> & values() const {
      return m_values;
   }

   double operator()(size_t i, size_t j) const {
      return m_values.at(index(i, j));
   }

   double operator()(double x, double y) const {
      return m_values.at(index(x, y));
   }

private:

   std::vector<double> m_xaxis;

   std::vector<double> m_yaxis;

   std::vector<double> m_values;

   size_t index(size_t i, size_t j) const;

   size_t index(double x, double y) const;
};

} // namespace handoff_response

#endif // handoff_response_IrfTables_h
