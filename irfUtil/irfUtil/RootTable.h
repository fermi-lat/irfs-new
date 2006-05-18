/**
 * @file RootTable.h
 * 
 * $Header$
 */

#ifndef irfUtil_RootTable_h
#define irfUtil_RootTable_h

#include <string>
#include "TFile.h"
#include "TH2D.h"

namespace irfUtil {

/**
 * @class RootTable
 * @brief Provide a more natural interface to ROOT's TH2D objects.
 * @todo Interpolate rather than giving the value of the bin.
 *
 * $Header$
 */

class RootTable {

public:
   /// @param filename ROOT file name.
   /// @param th2name The name of the TH2 object.
   RootTable(const std::string & filename, const std::string & th2name);
  
   ~RootTable();

   double operator()(double energy, double theta) const;

private:
   TFile* m_rootFile;
   TH2D* m_th2;
};

}
#endif // irfUtil_RootTable_h
