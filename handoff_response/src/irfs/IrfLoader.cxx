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



IrfLoader::IrfLoader(const std::string & filename) 
{
    if( filename.find(".root")>0){
        handoff_response::RootEval::createMap( filename, m_evals );
   }else{
        throw std::invalid_argument("IrfLoader: cannot process file "+filename
            +". Only ROOT files are currently supported.");
    }

}
IrfLoader::~IrfLoader()
{
    // should we delete the IrfEval guy? Maybe not.
}


irfInterface::Irfs * IrfLoader::irfs(std::string name , int index)
{
    IrfEval* eval = m_evals[name];

    irfInterface::IAeff* aeff = new Aeff(eval);
    irfInterface::IPsf* psf = new Psf(eval);
    irfInterface::IEdisp* disp = new Edisp(eval);

    return new irfInterface::Irfs( aeff, psf, disp, index);
}


