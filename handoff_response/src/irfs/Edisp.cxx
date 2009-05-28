/** 
* @file Edisp.cxx
* @brief Implementation class Edisp.
* @author J. Chiang
*
* $Header$
*/
#include "Edisp.h"

#include "handoff_response/IrfEval.h"
#include "astro/SkyDir.h"

#include <cmath>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <sstream>


using namespace handoff_response;


Edisp::Edisp(handoff_response::IrfEval* eval)
: m_eval(eval)
{}

double Edisp::value(double appEnergy,
                    double energy, 
                    const astro::SkyDir &srcDir,
                    const astro::SkyDir &scZAxis,
                    const astro::SkyDir &, double) const 
{
    // Inclination wrt spacecraft z-axis in degrees.
    double theta = srcDir.difference(scZAxis)*180./M_PI;

    // The azimuthal angle is not used by the IrfBase irfs.
    static double phi(0);
    return value(appEnergy, energy, theta, phi);
}

double Edisp::value(double appEnergy, double energy,
                    double theta, double phi, double) const 
{

    return m_eval->dispersion(appEnergy, energy, theta, phi);
}

double Edisp::appEnergy(double energy,
                        const astro::SkyDir & srcDir,
                        const astro::SkyDir & scZAxis,
                        const astro::SkyDir &, double) const 
{

    return 0; ///@todo
}


double Edisp::integral(double emin, double emax, double energy,
                       const astro::SkyDir & srcDir, 
                       const astro::SkyDir & scZAxis,
                       const astro::SkyDir &, double) const 
{
    return integral(emin, emax, energy,
        srcDir.difference(scZAxis)*180./M_PI, 0);
}

double Edisp::integral(double emin, double emax, double energy, 
                       double theta, double phi, double) const 
{
    (void)(phi);
    if (theta < 0) {
        std::ostringstream message;
        message << "handoff::Edisp"
            << "::integral(double, double, double, double, double):\n"
            << "theta cannot be less than zero. "
            << "Value passed: " << theta;
        throw std::invalid_argument(message.str());
    }
    return 0;


}


