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

      FitsFile aeff("aeff.fits", "EFFECTIVE AREA", "aeff.tpl");
      aeff.setGrid(front["aeff"]);
      aeff.setVectorData("EFFAREA", front["aeff"].values());
      aeff.setKeyword("LATCLASS", "FRONTA");

      FitsFile psf("psf.fits", "POINT SPREAD FUNCTION", "psf.tpl");
      psf.setGrid(front["pnorm"]);
      psf.setVectorData("PNORM", front["pnorm"].values());
      psf.setVectorData("SIGMA", front["sigma"].values());
      psf.setVectorData("GCORE", front["gcore"].values());
      psf.setVectorData("GTAIL", front["gtail"].values());
      psf.setKeyword("LATCLASS", "FRONTA");

      FitsFile edisp("edisp.fits", "ENERGY DISPERSION", "edisp.tpl");
      edisp.setGrid(front["dnorm"]);
      edisp.setVectorData("LTAIL", front["ltail"].values());
      edisp.setVectorData("RWIDTH", front["rwidth"].values());
      edisp.setVectorData("NR2", front["nr2"].values());
      edisp.setVectorData("LT2", front["lt2"].values());
      edisp.setVectorData("RT2", front["rt2"].values());
      edisp.setKeyword("LATCLASS", "FRONTA");

   } catch (std::exception & eobj) {
      std::cout << eobj.what() << std::endl;
   }

   return 0;
}
