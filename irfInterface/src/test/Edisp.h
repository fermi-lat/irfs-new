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

   Edisp() {}

   virtual ~Edisp() {}

   virtual double value(double,
                        double,
                        const astro::SkyDir &,
                        const astro::SkyDir &,
                        const astro::SkyDir &,
                        double) const {return 0;}

   virtual double value(double, double, 
                        double, double,
                        double) const {return 0;}

   virtual double appEnergy(double,
                            const astro::SkyDir &,
                            const astro::SkyDir &,
                            const astro::SkyDir &,
                            double) const {return 0;}

   virtual double integral(double, double, double,
                           const astro::SkyDir &,
                           const astro::SkyDir &,
                           const astro::SkyDir &,
                           double) const {return 0;}

   virtual double integral(double, double, double, 
                           double, double,
                           double) const {return 0;}

   virtual Edisp * clone() {return new Edisp(*this);}

};

#endif // irfInterface_Edisp_h
