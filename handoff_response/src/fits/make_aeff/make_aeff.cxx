#include <iostream>

#include "st_facilities/Env.h"
#include "st_facilities/Util.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "fits/IrfTableMap.h"

using handoff_response::IrfTableMap;

int main() {
   try {
   handoff_response::IrfTableMap front("standard::front");

   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());

   std::string outfile("aeff.fits");
   std::string templateFile = 
      st_facilities::Env::getDataDir("handoff_response") + "/aeff.tpl";
   if (st_facilities::Util::fileExists(outfile)) {
      std::remove(outfile.c_str());
   }
   fileSvc.createFile(outfile, templateFile);

   tip::Table * table = fileSvc.editTable(outfile, "EFFECTIVE AREA");

   table->setNumRecords(1);

   tip::TableRecord & row(*table->begin());

   std::vector<double> elo;
   std::vector<double> ehi;

   const std::vector<double> & loge(front["aeff"].xaxis());
   for (size_t k = 0; k < loge.size()-1; k++) {
      elo.push_back(std::pow(10., loge.at(k)));
      ehi.push_back(std::pow(10., loge.at(k+1)));
   }

   std::vector<double> ctlo;
   std::vector<double> cthi;
   const std::vector<double> & costheta(front["aeff"].yaxis());
   for (size_t i = 0; i < costheta.size()-1; i++) {
      ctlo.push_back(costheta.at(i));
      cthi.push_back(costheta.at(i+1));
   }

   const std::vector<double> & aeff(front["aeff"].values());

   const std::vector<std::string> & fields(table->getValidFields());

   for (size_t i = 0; i < fields.size(); i++) {
      std::cout << fields.at(i) << std::endl;
   }

   row["energ_lo"].setNumElements(elo.size());
   row["energ_lo"].set(elo);
   row["energ_hi"].setNumElements(ehi.size());
   row["energ_hi"].set(ehi);

   row["ctheta_lo"].setNumElements(ctlo.size());
   row["ctheta_lo"].set(ctlo);
   row["ctheta_hi"].setNumElements(cthi.size());
   row["ctheta_hi"].set(cthi);

   row["effarea"].setNumElements(aeff.size());
   row["effarea"].set(aeff);

   delete table;
   } catch (std::exception & eobj) {
      std::cout << eobj.what() << std::endl;
   }

   return 0;
}
