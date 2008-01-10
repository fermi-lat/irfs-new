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


using namespace handoff_response;



IrfLoader::IrfLoader(const std::string & filename, const std::string & eventclass) 
: m_irfeval( new handoff_response::IrfEval( filename, eventclass) )
{
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


