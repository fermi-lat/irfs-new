/** 
* @file Aeff.cxx
* @brief Implementation of class Aeff
* @author J. Chiang
* 
* $Header$
*/

#include "Aeff.h"
#include "handoff_response/IrfEval.h"

#include "astro/SkyDir.h"

#include <cmath>
#include <stdexcept>


using namespace handoff_response;


Aeff::Aeff(handoff_response::IrfEval* eval): m_eval(eval)
{}


double Aeff::value(double energy, 
                   const astro::SkyDir & srcDir, 
                   const astro::SkyDir & scZAxis,
                   const astro::SkyDir &) const 
{
    // Inclination wrt spacecraft z-axis in radians.
    double theta = srcDir.difference(scZAxis);
    theta *= 180./M_PI;
    return value(energy, theta, 0.);
}

double Aeff::value(double energy, double theta, double phi) const {

    return m_eval->aeff(energy, theta, phi);
}

double Aeff::upperLimit() const {
    return m_eval->aeffmax();
}

