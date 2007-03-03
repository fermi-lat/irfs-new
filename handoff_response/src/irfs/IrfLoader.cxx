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
#include "FitsEval.h"

using namespace handoff_response;

IrfLoader::IrfLoader(const std::string & filename) 
{
   std::string::size_type pos(filename.find(".root"));
   if (pos != std::string::npos) {
      RootEval::createMap(filename, m_evals);
   } else { // assume filename is a class name and retrieve FITS files.
      FitsEval::createMap(filename, m_evals);
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


