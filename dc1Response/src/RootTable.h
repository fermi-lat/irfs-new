/**
 * @file RootTable.h
 * 
 * $Header$
 */

#ifndef dc1Response_RootTable_h
#define dc1Response_RootTable_h

#include <string>
#include "TFile.h"
#include "TH2D.h"

namespace dc1Response {

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
#endif // dc1Response_RootTable_h
