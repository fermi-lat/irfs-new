/**
 * @file EfficiencyFactor.cxx
 * @brief Function that returns the IRF-dependent efficiency
 * corrections as a function of livetime fraction.  This is based on 
 * ratios of the livetime fraction-dependent effective area (P6_v6_diff) to
 * the livetime averaged effective area (P6_V3_DIFFUSE).  See
 * http://confluence.slac.stanford.edu/display/DC2/Efficiency+Correction+Parametrization+as+a+function+of+livetime+fraction+using+MC+(P6_v6_diff)
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "facilities/Util.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_facilities/Util.h"

#include "irfInterface/EfficiencyFactor.h"

namespace irfInterface {

EfficiencyFactor::
EfficiencyFactor() : m_havePars(false) {
   char * parfile = ::getenv("EFFICIENCY_PAR_FILE");
   if (parfile != 0) {
      readPars(parfile);
      m_havePars = true;
   }
}

EfficiencyFactor::
EfficiencyFactor(const std::string & parfile) : m_havePars(true) {
   readPars(parfile);
}

void EfficiencyFactor::
readPars(std::string parfile) {
   facilities::Util::expandEnvVar(&parfile);
   st_facilities::Util::file_ok(parfile);
   std::vector<std::string> lines;
   st_facilities::Util::readLines(parfile, lines, "#", true);

   std::vector< std::vector<double> > parVectors;
   for (size_t i(0); i < lines.size(); i++) {
      std::vector<std::string> tokens;
      bool clear;
      facilities::Util::stringTokenize(lines.at(i), " ", tokens, clear=true);
      std::vector<double> pars;
      for (size_t j(0); j < tokens.size(); j++) {
         pars.push_back(std::atof(tokens.at(j).c_str()));
      }
      parVectors.push_back(pars);
   }

   m_p0_front = EfficiencyParameter(parVectors.at(0));
   m_p1_front = EfficiencyParameter(parVectors.at(1));
   m_p0_back = EfficiencyParameter(parVectors.at(2));
   m_p1_back = EfficiencyParameter(parVectors.at(3));
}


double EfficiencyFactor::operator()(double energy, double met) const {
   if (!m_havePars || m_start.empty()) {
      return 1;
   }
   double ltfrac;

// Find the interval corresponedng to the desired met.  Intervals may
// not be uniform, so must do a search.  
   double tmin(m_start.front());
   double tmax(m_stop.back());
   double tol(0);
   if (met < tmin - tol || met > tmax + tol) {
      std::ostringstream message;
      message << "Requested MET of " << met << " "
              << "lies outside the range of valid times in the "
              << "pointing/livetime history: " 
              << tmin << " to " << tmax << "MET s";
      throw std::runtime_error(message.str());
   }
   std::vector<double>::const_iterator it 
      = std::upper_bound(m_start.begin(), m_start.end(), met);
   size_t indx = it - m_start.begin() - 1;

   if (m_start.at(indx) <= met && met <= m_stop.at(indx)) {
      ltfrac = m_livetimefrac.at(indx);
   }

   return value(energy, ltfrac);
}

double EfficiencyFactor::value(double energy, double livetimefrac,
                               bool front) const {
   if (!m_havePars) {
      return 1;
   }

   double logE(std::log10(energy));

   if (front) {
      return m_p0_front(logE)*livetimefrac + m_p1_front(logE);
   } 
   return m_p0_back(logE)*livetimefrac + m_p1_back(logE);
}

double EfficiencyFactor::value(double energy, double livetimefrac) const {
   if (!m_havePars) {
      return 1;
   }

   // Since we do not have access to the front and back effective
   // areas separately, just return the average.
   return (value(energy, livetimefrac, true) + 
           value(energy, livetimefrac, false))/2.;
}

void EfficiencyFactor::
getLivetimeFactors(double energy, double & factor1, double & factor2) const {
   if (!m_havePars) {
      factor1 = 1;
      factor2 = 0;
      return;
   }
   double logE(std::log10(energy));

   // Since we do not have access to the front and back effective
   // areas separately, just return the averages.  We would really
   // want to return the averages weighted by effective area (as a function
   // of energy and theta).
   factor1 = (m_p1_front(logE) + m_p1_back(logE))/2.;
   factor2 = (m_p0_front(logE) + m_p0_back(logE))/2.;
}

void EfficiencyFactor::readFt2File(std::string ft2file) {
   facilities::Util::expandEnvVar(&ft2file);
   const tip::Table * scData = 
      tip::IFileSvc::instance().readTable(ft2file, "SC_DATA");
   tip::Table::ConstIterator it(scData->begin());
   tip::ConstTableRecord & row(*it);
   for ( ; it != scData->end(); ++it) {
      m_start.push_back(row["START"].get());
      m_stop.push_back(row["STOP"].get());
      double duration(m_stop.back() - m_start.back());
      m_livetimefrac.push_back(row["livetime"].get()/duration);
   }
   m_dt = (m_stop.back() - m_start.front())/m_start.size();
   delete scData;
}

void EfficiencyFactor::clearFt2Data() {
   m_start.clear();
   m_stop.clear();
   m_livetimefrac.clear();
}

} // namespace irfInterface
