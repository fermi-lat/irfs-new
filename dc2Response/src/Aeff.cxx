/** 
 * @file Aeff.cxx
 * @brief Implementation for the DC2 effective Area class.
 * @author Johann Cohen-Tanugi
 * 
 * $Header$
 */

#include <cmath>

#include <sstream>
#include <stdexcept>

#include "astro/SkyDir.h"

#include "irfUtil/Util.h"
#include "irfUtil/RootTable.h"

#include "Aeff.h"

namespace dc2Response {

Aeff::Aeff(const Aeff &rhs) : IAeff(rhs), DC2(rhs) {
   m_aeff = rhs.m_aeff;
   if (m_have_FITS_data) {
      m_aeffTable = rhs.m_aeffTable;
   } else {
      m_histName = rhs.m_histName;
      readAeffTable();
   }
}

Aeff::Aeff(const std::string &filename, bool getFront) 
   : DC2(filename, false), m_aeff(0) {
   if (getFront) {
      m_histName = "lectf";
   } else {
      m_histName = "lectb";
   }
   readAeffTable();
}

Aeff::Aeff(const std::string &filename, int hdu) 
   : DC2(filename, hdu), m_aeff(0) {
   read_FITS_table();
}

Aeff::~Aeff() {
   delete m_aeff;
}

void Aeff::read_FITS_table() {
   std::string extName;
   irfUtil::Util::getFitsHduName(m_filename, m_hdu, extName);
   irfUtil::Util::getRecordVector(m_filename, extName, "energy_lo", m_energy);

   std::vector<double> upper_bounds;
   irfUtil::Util::getRecordVector(m_filename, extName, "energy_hi", 
                                  upper_bounds);
   m_energy.push_back( *(upper_bounds.end() - 1) );

   irfUtil::Util::getRecordVector(m_filename, extName, "theta_lo", m_theta);
   irfUtil::Util::getRecordVector(m_filename, extName, "theta_hi", 
                                  upper_bounds);
   m_theta.push_back( *(upper_bounds.end() - 1) );

   m_theta[0] = 0.;  // kludge until aeff_DC2.fits is fixed.
   irfUtil::Util::getRecordVector(m_filename, extName, "effarea", m_aeffTable);
}

void Aeff::readAeffTable() {
   m_aeff = new irfUtil::RootTable(m_filename, m_histName);
}

double Aeff::value(double energy, 
                   const astro::SkyDir &srcDir, 
                   const astro::SkyDir &scZAxis,
                   const astro::SkyDir &) const {
// Inclination wrt spacecraft z-axis in radians.
   double theta = srcDir.difference(scZAxis);
   theta *= 180./M_PI;
   return value(energy, theta, 0.);
}

double Aeff::value(double energy, double theta, double phi) const {
   (void)(phi);

   if (theta < 0) {
      std::ostringstream message;
      message << "dc2Response::Aeff::value(double, double, double):\n"
              << "theta cannot be less than zero. "
              << "Value passed: " << theta;
      throw std::invalid_argument(message.str());
   }

   double my_value;

   if (m_have_FITS_data) {
      int indx = getAeffIndex(energy, theta);
      if (indx >= 0) {
         my_value = m_aeffTable[indx];
      } else {
         my_value = 0;
      }
   } else {
// Convert to cm^2.
      my_value = AeffValueFromTable(energy, theta*M_PI/180.)*1e4;
   }
   return my_value;
}
   
int Aeff::getAeffIndex(double energy, double theta) const {
// Find the location of energy and inclination in grid boundary
// arrays.
   int ien, ith;
   if (energy < *m_energy.begin() || energy >= *(m_energy.end() - 1)
       || theta < *m_theta.begin() || theta >= *(m_theta.end() - 1)) {
// Desired grid point is outside the valid range.
      return -1;
   }
   ien = find_iterator(m_energy, energy) - m_energy.begin();
   ith = find_iterator(m_theta, theta) - m_theta.begin();
   int indx = ien*(m_theta.size()-1) + ith;
   return indx;
}

double Aeff::AeffValueFromTable(double energy, double theta) const {
  return (*m_aeff)(energy, theta);
}

} // namespace dc2Response
