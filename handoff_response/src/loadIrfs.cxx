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

namespace handoff_response{

void loadIrfs(const std::string& name) 
{
    std::string filename(name);
    if( filename.empty()){
        std::string path(::getenv("HANDOFF_RESPONSEROOT"));
        filename = path+"/data/parameters.root";
    }
    TFile* m_file= new TFile(filename.c_str(), "readonly");
    if( !m_file->IsOpen() ) { throw std::invalid_argument("Could not load the file "+filename);}

    irfInterface::IrfsFactory * myFactory 
        = irfInterface::IrfsFactory::instance();

    std::vector<std::string> irfsNames;
    myFactory->getIrfsNames(irfsNames);

    TList * keys = m_file->GetListOfKeys();
    for( int i = 0; i< keys->GetEntries(); ++i){
        std::string eventclass ( keys->At(i)->GetName() );
        std::cout << "Loading irfs for event class "<< eventclass << std::endl;
        
        IrfLoader front(filename, eventclass+"/front");
        myFactory->addIrfs(eventclass+"/front", front.irfs(i), 2*i);

        IrfLoader back(filename, eventclass+"/back");
        myFactory->addIrfs(eventclass+"/back", back.irfs(i), 2*i+1);
    }
    std::cout << "done" << std::endl;
   
}   

}
