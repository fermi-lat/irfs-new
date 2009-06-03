/**
 * @file EfficiencyFactor.cxx
 * @brief Static function that returns the IRF-dependent efficiency
 * correction as a function of livetime fraction based on fits to
 * Vela, Crab, Geminga data (D. Paneque).
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <iostream>
#include <stdexcept>

#include "facilities/Util.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_facilities/Util.h"

#include "irfInterface/EfficiencyFactor.h"

namespace {
   std::string remove_white_space(const std::string & input) {
      std::string output("");
      for (std::string::const_iterator it(input.begin()); 
           it != input.end(); ++it) {
         if (!isspace(*it)) {
            output.push_back(*it);
         }
      }
      return output;
   }
}

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

   std::map<std::string, std::string> parmap;

   for (size_t i(0); i < lines.size(); i++) {
      facilities::Util::keyValueTokenize(::remove_white_space(lines.at(i)),
                                         ",", parmap, "=", false);
   }

   m_offset_p0 = std::atof(parmap["offset_p0"].c_str());
   m_offset_p1 = std::atof(parmap["offset_p1"].c_str());
   m_slope_p0 = std::atof(parmap["slope_p0"].c_str());
   m_slope_p1 = std::atof(parmap["slope_p1"].c_str());
   m_rate_p0 = std::atof(parmap["rate_p0"].c_str());
   m_rate_p1 = std::atof(parmap["rate_p1"].c_str());
}


double EfficiencyFactor::operator()(double energy, double met) const {
   if (!m_havePars || m_start.empty()) {
      return 1;
   }
   double ltfrac;

   size_t indx = static_cast<size_t>((met - m_start.front())/m_dt);
// Intervals may not be uniform, so must do a search.  The offsets
// from the computed index should be constant and small over large
// ranges, so a linear search from the computed point is reasonable.
   indx = std::min(indx, m_start.size()-1);
   if (m_start.at(indx) > met) {
      while (m_start.at(indx) > met && indx > 0) {
         indx--;
      }
      if (m_start.at(indx) <= met && met <= m_stop.at(indx)) {
         ltfrac = m_livetimefrac.at(indx);
      } else { // This may occur if there are gaps in the FT2 file.
         std::ostringstream message;
         message << "Input MET value " << met 
                 << " lies outside any interval in the spacecraft data.";
         throw std::runtime_error(message.str());
      }
   }
// Ensure we have not fallen short of the desired interval.
   while (m_start.at(indx) < met && indx < m_start.size()) {
      ++indx;
   }
   --indx;
   if (m_start.at(indx) <= met && met <= m_stop.at(indx)) {
      ltfrac = m_livetimefrac.at(indx);
   }

   return value(energy, ltfrac);
}

double EfficiencyFactor::value(double energy, double livetimefrac) const {
   if (!m_havePars) {
      return 1;
   }

   double offset(m_offset_p0 + m_offset_p1*std::log10(energy));
   double slope(m_slope_p0 + m_slope_p1*std::log10(energy));
   double rate(m_rate_p0 + m_rate_p1*livetimefrac);

   return offset + slope*rate;
}

void EfficiencyFactor::
getLivetimeFactors(double energy, double & factor1, double & factor2) const {
   if (!m_havePars) {
      factor1 = 1;
      factor2 = 0;
      return;
   }
   double log10E(std::log10(energy));
   double offset(m_offset_p0 + m_offset_p1*log10E);
   double slope(m_slope_p0 + m_slope_p1*log10E);
   factor1 = offset + m_rate_p0*slope;
   factor2 = m_rate_p1*slope;
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
