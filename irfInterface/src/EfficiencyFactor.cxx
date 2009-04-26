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

double EfficiencyFactor::s_dt;
std::vector<double> EfficiencyFactor::s_start;
std::vector<double> EfficiencyFactor::s_stop;
std::vector<double> EfficiencyFactor::s_livetimefrac;

EfficiencyFactor::
EfficiencyFactor() : m_havePars(false) {
   char * module_name = ::getenv("EFFICIENCY_PAR_FILE");
   if (module_name != 0) {
      readPars(module_name);
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
   if (!m_havePars) {
      return 1;
   }
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
   if (!m_havePars) {
      return 1;
   }

//    std::cout << m_offset_p0 << "\n"
//              << m_offset_p1 << "\n"
//              << m_slope_p0 << "\n"
//              << m_slope_p1 << "\n"
//              << m_rate_p0 << "\n"
//              << m_rate_p1 << std::endl;

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
