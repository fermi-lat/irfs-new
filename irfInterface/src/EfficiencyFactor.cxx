/**
 * @file EfficiencyFactor.cxx
 * @brief Static function that returns the IRF-dependent efficiency
 * corrections as a function of livetime fraction based on fits to
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

#include "embed_python/Module.h"

#include "irfInterface/EfficiencyFactor.h"

namespace irfInterface {

double EfficiencyFactor::s_dt;
std::vector<double> EfficiencyFactor::s_start;
std::vector<double> EfficiencyFactor::s_stop;
std::vector<double> EfficiencyFactor::s_livetimefrac;

EfficiencyFactor::
EfficiencyFactor(std::string python_dir, std::string parfile) {
   facilities::Util::expandEnvVar(&python_dir);
   embed_python::Module pars(python_dir, parfile);

   pars.getValue("offset_p0", m_offset_p0);
   pars.getValue("offset_p1", m_offset_p1);
   pars.getValue("slope_p0", m_slope_p0);
   pars.getValue("slope_p1", m_slope_p1);
   pars.getValue("rate_p0", m_rate_p0);
   pars.getValue("rate_p1", m_rate_p1);
}

double EfficiencyFactor::operator()(double energy, double met) const {
   double ltfrac;

   size_t indx = static_cast<size_t>((met - s_start.front())/s_dt);
// Intervals may not be uniform, so must do a search.  The offsets
// from the computed index should be constant and small over large
// ranges, so a linear search from the computed point is reasonable.
   indx = std::min(indx, s_start.size()-1);
   if (s_start.at(indx) > met) {
      while (s_start.at(indx) > met && indx > 0) {
         indx--;
      }
      if (s_start.at(indx) <= met && met <= s_stop.at(indx)) {
         ltfrac = s_livetimefrac.at(indx);
      } else { // This may occur if there are gaps in the FT2 file.
         std::ostringstream message;
         message << "Input MET value " << met 
                 << " lies outside any interval in the spacecraft data.";
         throw std::runtime_error(message.str());
      }
   }
// Ensure we have not fallen short of the desired interval.
   while (s_start.at(indx) < met && indx < s_start.size()) {
      ++indx;
   }
   --indx;
   if (s_start.at(indx) <= met && met <= s_stop.at(indx)) {
      ltfrac = s_livetimefrac.at(indx);
   }

   return value(energy, ltfrac);
}

double EfficiencyFactor::value(double energy, double livetimefrac) const {

   double offset(m_offset_p0 + m_offset_p1*std::log10(energy));
   double slope(m_slope_p0 + m_slope_p1*std::log10(energy));
   double rate(m_rate_p0 + m_rate_p1*livetimefrac);

   return 1./(offset + slope*rate);
}

void EfficiencyFactor::readFt2File(const std::string & ft2file) {
   const tip::Table * scData = 
      tip::IFileSvc::instance().readTable(ft2file, "SC_DATA");
   tip::Table::ConstIterator it(scData->begin());
   tip::ConstTableRecord & row(*it);
   for ( ; it != scData->end(); ++it) {
      s_start.push_back(row["START"].get());
      s_stop.push_back(row["STOP"].get());
      double duration(s_stop.back() - s_start.back());
      s_livetimefrac.push_back(row["livetime"].get()/duration);
   }
   s_dt = (s_stop.back() - s_start.front())/s_start.size();
   delete scData;
}

void EfficiencyFactor::clearFt2Data() {
   s_start.clear();
   s_stop.clear();
   s_livetimefrac.clear();
}

} // namespace irfInterface
