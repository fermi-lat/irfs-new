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

        Setup s(argc, argv);  // will cd to current folder.

        MyAnalysis::s_input_filename = "goodEvent.root";
        std::ofstream logfile("log.txt");

        logfile << "Original file: " << s[0] << std::endl;
        logfile << "Cuts:          " << s[1] << std::endl;

        std::string folder(s.root());
        bool make_plots(false);
#if 1 //front
        IrfAnalysis front(folder, 1, logfile);
        front.fit(make_plots, "../parameters.root");
#endif
#if 1// back
        IrfAnalysis back(folder,  2, logfile);
        back.fit(make_plots,  "../parameters.root");
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
