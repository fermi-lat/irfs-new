/**
 * @file PsfScaling.cxx
 * @brief Energy scaling of angular deviation for fitting Psf.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "PsfScaling.h"

namespace {
   double sqr(double x) {
      return x*x;
   }
}

namespace dc2Response {

PsfScaling::PsfScaling(const std::vector<double> & pars, bool useFront) 
   : m_pars(pars), m_useFront(useFront) {}

PsfScaling::PsfScaling(const std::string & psfFile) {
   const tip::Table * table =
      tip::IFileSvc::instance().readTable(psfFile, "PSF_SCALING_PARAMS");
   tip::ConstTableRecord & row(*table->begin());
   row["PSFSCALE"].get(m_pars);
   std::string latClass;
   table->getHeader()["LATCLASS"].get(latClass);
   if (latClass.find("FRONT") != std::string::npos) {
      m_useFront = true;
   } else {
      m_useFront = false;
   }
   delete table;
}

double PsfScaling::operator()(double McEnergy, double McZDir) const {
   double t(powerLawScaling(McEnergy));
   double x(trendline(McEnergy));
   double scaleFactor(1.);
   if (m_useFront) {
      scaleFactor *= (m_pars[4]*x*x - m_pars[5]*x + m_pars[6]);
      scaleFactor *= zfactor(McZDir)*std::sqrt(::sqr(m_pars[7]*t) +
                                               ::sqr(m_pars[8]));
   } else {
      scaleFactor *= (m_pars[9]*x + m_pars[10]);
      scaleFactor *= zfactor(McZDir)*std::sqrt(::sqr(m_pars[11]*t) +
                                               ::sqr(m_pars[12]));
   }
   return scaleFactor*180./M_PI;
}

double PsfScaling::powerLawScaling(double McEnergy) const {
   return std::pow(McEnergy/100., m_pars[0]);
}

double PsfScaling::zfactor(double McZDir) const {
   return 1. + m_pars[1]*(1. - std::fabs(McZDir));
}

double PsfScaling::trendline(double McEnergy) const {
   return m_pars[2]*std::log10(McEnergy/100.) + m_pars[3];
}


} //namespace dc2Response
