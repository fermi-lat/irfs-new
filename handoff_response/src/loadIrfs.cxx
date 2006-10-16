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


// ROOT
#include "TDirectory.h"
#include "TFile.h"
#include "TKey.h"
#include "TList.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

namespace {
#ifdef WIN32
//#include "TPaletteAxis.h" 
//TPaletteAxis junk; 
#endif
}
namespace handoff_response{

    /** @class RootEventClassList
    @brief helper class to extract the class names from a ROOT file

    Encapsulate the ROOT dependence here to easily allow for eventual support of a CALDB option

    */
    class RootEventClassList : public std::vector<std::string> {
    public:
        RootEventClassList(std::string filename)
        {
            TFile* file= new TFile(filename.c_str(), "readonly");
            if( !file->IsOpen() ) { throw std::invalid_argument("Could not load the file "+filename);}
            TList * keys = file->GetListOfKeys();
            for( int i = 0; i< keys->GetEntries(); ++i){
                std::string eventclass ( keys->At(i)->GetName() );
                
                push_back(eventclass);
            }
        }

    private:
    };

    

void loadIrfs(const std::string& name) 
{
    bool verbose(true);

    std::string filename(name);
    // if no filename supplied, assume default
    if( filename.empty()){
        std::string path(::getenv("HANDOFF_RESPONSEROOT"));
        filename = path+"/data/parameters.root";
    }

    // get the list of names found in the file
    RootEventClassList classnames(filename);

    // the factory to add our IRFs to
    irfInterface::IrfsFactory * myFactory = irfInterface::IrfsFactory::instance();

    // assuming each evenclass has a front and a back, add them to the factory
    int i(0);
    for( std::vector<std::string>::const_iterator it = classnames.begin();
        it!=classnames.end(); ++it,++i){
        const std::string& eventclass= *it;
        if(verbose) std::cout << "Loading irfs for event class "<< eventclass << std::endl;
        
        IrfLoader front(filename, eventclass+"/front");
        myFactory->addIrfs(eventclass+"/front", front.irfs(i), 2*i);

        IrfLoader back(filename, eventclass+"/back");
        myFactory->addIrfs(eventclass+"/back", back.irfs(i), 2*i+1);
    }
    if(verbose) std::cout << "done" << std::endl;
}   

}
