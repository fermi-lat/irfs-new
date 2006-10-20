/**
 * @file loadIrfs.cxx
 * @brief Function to load the handoff response functions into the
 * IrfsFactory instance.
 *
 * $Header$
 */


#include "irfInterface/IrfsFactory.h"

#include "handoff_response/IrfEval.h"
#include "handoff_response/loadIrfs.h"
#include "irfs/IrfLoader.h"


#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>


namespace handoff_response{

    

void loadIrfs(const std::string& name, bool verbose) 
{
    std::string filename(name);
    // if no filename supplied, assume default
    if( filename.empty()){
        std::string path(::getenv("HANDOFF_RESPONSEROOT"));
        filename = path+"/data/parameters.root";
    }

    // get the list of names found in the file
    IrfLoader loader(filename);

    // the factory to add our IRFs to
    irfInterface::IrfsFactory * myFactory = irfInterface::IrfsFactory::instance();

// event classes within each set of IRFs have to be unique within
// that set of IRFs, and the event class is given by the
// irfInterface::Irfs::irfID() function and set in the Irfs constructor:
// Irfs(IAeff *aeff, IPsf *psf, IEdisp *edisp, int irfID),
// Front vs Back constitute two separate classes, so the argument to
// IrfLoader::irfs() must be different for each. 

    int id(0);
    for( IrfLoader::const_iterator it = loader.begin(); it!=loader.end(); ++it){
        const std::string& eventclass= it->first;

        if(verbose) std::cout << "Loading irfs for event class "<< eventclass << std::endl;
        
        myFactory->addIrfs(eventclass, loader.irfs(eventclass,id++), verbose);
    }
}   

}
