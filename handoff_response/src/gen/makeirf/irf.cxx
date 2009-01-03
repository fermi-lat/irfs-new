/** @file irf.cxx  
@brief Analysis of Aeff, PSf and dispersion

$Header$
*/

#include "../IrfAnalysis.h"
#include "../Setup.h"

#include <stdexcept>
#include <fstream>


//_____________________________________________________________________________

int main(int argc, char* argv[]){
   int ret=0;
   Setup s(argc, argv, false);  // will cd to current folder.
   
   std::string folder(s.root());
   bool make_plots(true);
   
   IrfAnalysis front(folder, 1, *s.py());
   front.fit(make_plots);
   
   IrfAnalysis back(folder,  2, *s.py());
   back.fit(make_plots);
   return ret;
}

/** @page makeirf The makeirf application

This application creates the irfs, and expects a single argument, to a folder containing two files:

- setup.txt
- goodEvent.root

The first is processed by the Setup class to extract values for the log file. 

*/
