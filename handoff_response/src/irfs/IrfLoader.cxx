/** 
 * @file IrfLoader.cxx
 * @brief Implementation for IrfLoader  class.
 * @author J. Chiang
 * 
 * $Header$
 */
#include "IrfLoader.h"

#include "Aeff.h"
#include "Psf.h"
#include "Edisp.h"

#include "irfInterface/Irfs.h"

#include <stdexcept>
#include "RootEval.h"

using namespace handoff_response;



IrfLoader::IrfLoader(const std::string & filename, const std::string & eventclass) 
{
    if( filename.find(".root")>0){
        m_irfeval= new handoff_response::RootEval( filename, eventclass);
    }else{
        throw std::invalid_argument("IrfLoader: cannot process file "+filename
            +". Only ROOT files are currently supported.");
    }

}

IrfLoader::~IrfLoader()
{
    // should we delete the IrfEval guy? Maybe not.
}

irfInterface::Irfs * IrfLoader::irfs(int index)
{

    irfInterface::IAeff* aeff = new Aeff(m_irfeval);
    irfInterface::IPsf* psf = new Psf(m_irfeval);
    irfInterface::IEdisp* disp = new Edisp(m_irfeval);

    return new irfInterface::Irfs( aeff, psf, disp, index);
}


