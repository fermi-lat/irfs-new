/**
 * @file Aeff.h
 * @brief Aeff class declaration.
 * @author J. Chiang
 *
 * $Header$
 */
  
#ifndef handoff_Aeff_h
#define handoff_Aeff_h

#include "handoff_response/IrfEval.h"

#include <string>
#include <vector>

#include "irfInterface/IAeff.h"

namespace handoff_response{

/**
 * @class Aeff
 * @brief declare IAeff subclass for handoff effective area
 * @author J. Chiang
 *
 */
  
class Aeff : public irfInterface::IAeff {

public:

    Aeff(handoff_response::IrfEval* eval);


   virtual ~Aeff() {}

   virtual double value(double energy, 
                        const astro::SkyDir &srcDir, 
                        const astro::SkyDir &scZAxis,
                        const astro::SkyDir &scXAxis) const;

   virtual double value(double energy, double theta, double phi) const;

   virtual Aeff * clone() {return new Aeff(*this);}

   virtual double upperLimit() const;

private:


    handoff_response::IrfEval* m_eval;

};

}
#endif // handoff_Aeff_h

