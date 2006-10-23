/** 
 * @file Edisp.h
 * @brief Concrete IEdisp subclass.
 * @author J. Chiang
 * 
 * $Header$
 */

#ifndef irfInterface_Edisp_h
#define irfInterface_Edisp_h

#include "irfInterface/IEdisp.h"

/** 
 * @class Edisp
 *
 * @author J. Chiang
 *    
 * $Header$
 */

class Edisp : public irfInterface::IEdisp {
    
public:

   Edisp(double resolution=0.1);

   virtual ~Edisp() {}

   virtual double value(double appEnergy,
                        double energy,
                        const astro::SkyDir & srcDir,
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir & scXAxis,
                        double time=0) const;

   virtual double value(double appEnergy, double energy, 
                        double theta, double phi,
                        double time=0) const;

   virtual Edisp * clone() {return new Edisp(*this);}

private:

   double m_resolution;

};

#endif // irfInterface_Edisp_h
