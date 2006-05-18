/**
 * @file RootTable.cxx
 *
 * $Header$
 */

#include <iostream>
#include <stdexcept>
#include <string>

#include "irfUtil/RootTable.h"

namespace irfUtil {

RootTable::RootTable(const std::string & filename, 
                     const std::string & th2name) {
   m_rootFile = new TFile(filename.c_str());
   m_th2 = (TH2D*) m_rootFile->Get(th2name.c_str());
   if (m_th2 == 0) {
      std::string message = "irfUtil::AeffRootTable: "
         + th2name + " not found in " + filename;
      throw std::runtime_error(message);
   }
}

double RootTable::operator()(double energy, double theta) const {
   double logEnergy = log10(energy);
   double mu = -cos(theta);

   int ebin = m_th2->GetXaxis()->FindBin(logEnergy);
   int tbin = m_th2->GetYaxis()->FindBin(mu);

   return m_th2->GetBinContent(ebin,tbin);
}

RootTable::~RootTable() {
   m_rootFile->Close(); 
   delete m_rootFile; 
}

} // namespace irfUtil
