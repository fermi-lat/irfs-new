/**
 * @file IrfTableMap.h
 * @brief Abstraction for handoff_response parameters file.
 * 
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef handoff_response_IrfTableMap_h
#define handoff_response_IrfTableMap_h

#include <map>
#include <string>
#include <vector>

#include "IrfTable.h"

class TDirectory;

namespace handoff_response {

/**
 * @class IrfTableMap
 * @brief Abstraction for handoff_response parameters file.
 * 
 * @author J. Chiang
 */

class IrfTableMap {

public:
   
   IrfTableMap(const std::string & irfTables,
               const std::string & rootfile="parameters.root");

   const std::vector<std::string> & keys() const {
      return m_keys;
   }

   const IrfTable & operator[](const std::string & tablename) const;

private:

   std::map<std::string, IrfTable> m_tables;

   std::vector<std::string> m_keys;

   void readTableNames(TDirectory * section);

};

} // namespace handoff_response

#endif // handoff_response_IrfTableMap_h
