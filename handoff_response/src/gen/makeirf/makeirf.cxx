/**
 * @file irf.cxx  
 * @brief Analysis of Aeff, PSf and dispersion
 *
 * $Header$
*/

#include "../IrfAnalysis.h"
#include "../Setup.h"

#include <iostream>
#include <stdexcept>
#include <fstream>

int main(int argc, char* argv[]) {
   int ret(0);
   Setup s(argc, argv, false);  // will cd to current folder.
   
   std::string folder(s.root());
   bool make_plots(true);
   
   IrfAnalysis irf_analysis(folder, *s.py());
   irf_analysis.fit(make_plots);

   return ret;
}

/** @page makeirf The makeirf application

This application creates the irfs, and expects a single argument, to a folder containing two files:

- setup.txt
- goodEvent.root

The first is processed by the Setup class to extract values for the log file. 

*/
