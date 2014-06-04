/**
 * @file IrfTableMap.cxx
 * @brief Abstraction for handoff_response parameters file.
 * 
 * @author J. Chiang
 *
 * $Header$
 */

#include <stdexcept>

#include "TDirectory.h"
#include "TFile.h"
#include "TH2F.h"
#include "TKey.h"

#include "facilities/Util.h"

#include "st_facilities/Env.h"

#include "IrfTableMap.h"

namespace handoff_response {

IrfTableMap::IrfTableMap(const std::string & irfTables,
                         const std::string & infile) {

   TFile * rootfile(new TFile(infile.c_str()));

   std::vector<std::string> tokens;
   facilities::Util::stringTokenize(irfTables, "::", tokens);

   TDirectory * irfs = 
      dynamic_cast<TDirectory *>(rootfile->Get(tokens.at(0).c_str()));
   readTableNames(irfs);

   for (size_t i = 0; i < m_keys.size(); i++) {
      TH2F * table = dynamic_cast<TH2F *>(irfs->Get(m_keys.at(i).c_str()));
      if (table != 0) {
         m_tables[m_keys.at(i)] = IrfTable(table);
      } else {
         TH1F * th1f = dynamic_cast<TH1F *>(irfs->Get(m_keys.at(i).c_str()));
         m_tables[m_keys.at(i)] = IrfTable(th1f);
      }
   }
   delete rootfile;
}

void IrfTableMap::readTableNames(TDirectory * section) {
   m_keys.clear();
   TIter nextkey(section->GetListOfKeys());
   TKey * key;
   while ((key = reinterpret_cast<TKey *>(nextkey()))) {
      std::string name(key->GetName());
// Should use ROOT introspection (yeah, right) to determine which items
// are tables.  For now, skip parameters since we know it is a TTree.
      if (name != "parameters") {    
         m_keys.push_back(name);
      }
   }
}

const IrfTable & IrfTableMap::operator[](const std::string & tablename) const {
   std::map<std::string, IrfTable>::const_iterator table =
      m_tables.find(tablename);
   if (table == m_tables.end()) {
      throw std::runtime_error("Table named " + tablename + " not found.");
   }
   return table->second;
}

} //namespace handoff_response
