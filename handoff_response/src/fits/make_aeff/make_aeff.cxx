/**
 * @file make_aeff.cxx
 * @brief Sample program for converting ROOT tables to FITS.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <iostream>
#include <stdexcept>

#include "fits/IrfTableMap.h"
#include "fits/FitsFile.h"

using handoff_response::IrfTableMap;
using handoff_response::FitsFile;

int main() {
   try {
      IrfTableMap front("standard::front");
      FitsFile aeff_file("aeff.fits", "EFFECTIVE AREA", "aeff.tpl");

      std::vector<double> elo;
      std::vector<double> ehi;
      
      const std::vector<double> & loge(front["aeff"].xaxis());
      for (size_t k = 0; k < loge.size()-1; k++) {
         elo.push_back(std::pow(10., loge.at(k)));
         ehi.push_back(std::pow(10., loge.at(k+1)));
      }

      aeff_file.setVectorData("ENERG_LO", elo);
      aeff_file.setVectorData("ENERG_HI", ehi);
      
      std::vector<double> ctlo;
      std::vector<double> cthi;
      const std::vector<double> & costheta(front["aeff"].yaxis());
      for (size_t i = 0; i < costheta.size()-1; i++) {
         ctlo.push_back(costheta.at(i));
         cthi.push_back(costheta.at(i+1));
      }

      aeff_file.setVectorData("CTHETA_LO", ctlo);
      aeff_file.setVectorData("CTHETA_HI", cthi);

      aeff_file.setVectorData("EFFAREA", front["aeff"].values());

   } catch (std::exception & eobj) {
      std::cout << eobj.what() << std::endl;
   }

   return 0;
}
