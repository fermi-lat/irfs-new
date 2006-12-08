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

#include "IrfTableMap.h"

namespace handoff_response {

IrfTableMap::IrfTableMap(const std::string & irfTables,
                         const std::string & inputfile) {
   std::string infile(dataPath() + "/" + inputfile);
   TFile * rootfile(new TFile(infile.c_str()));

   std::vector<std::string> tokens;
   facilities::Util::stringTokenize(irfTables, "::", tokens);

   TDirectory * irfs = 
      dynamic_cast<TDirectory *>(rootfile->Get(tokens.at(0).c_str()));
   TDirectory * section = 
      dynamic_cast<TDirectory *>(irfs->Get(tokens.at(1).c_str()));

   readTableNames(section);

   for (size_t i = 0; i < m_keys.size(); i++) {
      TH2F * table =
         dynamic_cast<TH2F *>(section->Get(m_keys.at(i).c_str()));
      m_tables[m_keys.at(i)] = IrfTable(table);
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

std::string IrfTableMap::dataPath() const {
   char * root_path = ::getenv("HANDOFF_RESPONSEROOT");
   if (!root_path) {
      throw std::runtime_error("HANDOFF_RESPONSEROOT env var is not set.");
   }
   return std::string(root_path) + "/data";
}

} //namespace handoff_response
