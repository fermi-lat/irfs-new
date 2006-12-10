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
    try {

        Setup s(argc, argv, false);  // will cd to current folder.


        std::string folder(s.root());
        bool make_plots(true);
#if 1 //front
        IrfAnalysis front(folder, 1, *s.py());
        front.fit(make_plots);
#endif
#if 1// back
        IrfAnalysis back(folder,  2, *s.py());
        back.fit(make_plots);
#endif
    }catch( const std::exception& e){
        std::cerr << "Caught exception "<< e.what() << std::endl;
        ret=1;
    }
    return ret;
}

/** @page makeirf The makeirf application

This application creates the irfs, and expects a single argument, to a folder containing two files:

- setup.txt
- goodEvent.root

The first is processed by the Setup class to extract values for the log file. 

*/
