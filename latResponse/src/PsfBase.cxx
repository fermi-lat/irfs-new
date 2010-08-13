/**
 * @file PsfBase.cxx
 * @brief Base class for latResponse Psfs
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>
#include <algorithm>

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "latResponse/FitsTable.h"

#include "PsfBase.h"

namespace {
   double sqr(double x) { 
      return x*x;
   }
}

namespace latResponse {

PsfBase::PsfBase(const std::string & fitsfile, bool isFront,
                 const std::string & extname) {
   readScaling(fitsfile, isFront);
}

PsfBase::PsfBase(const PsfBase & rhs) : irfInterface::IPsf(rhs),
                                        m_par0(rhs.m_par0),
                                        m_par1(rhs.m_par1),
                                        m_index(rhs.m_index) {}

double PsfBase::scaleFactor(double energy) const {
   double tt(std::pow(energy/100., m_index));
   return std::sqrt(::sqr(m_par0*tt) + ::sqr(m_par1));
}

double PsfBase::scaleFactor(double energy, bool isFront) const {
   double par0, par1;
   if (isFront) {
      par0 = m_psf_pars.at(0);
      par1 = m_psf_pars.at(1);
   } else {
      par0 = m_psf_pars.at(2);
      par1 = m_psf_pars.at(3);
   }      
   double tt(std::pow(energy/100., m_index));
   return std::sqrt(::sqr(par0*tt) + ::sqr(par1));
}

void PsfBase::readScaling(const std::string & fitsfile, bool isFront, 
                          const std::string & extname) {
   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());
   const tip::Table * table(fileSvc.readTable(fitsfile, extname));

   std::vector<double> values;

   FitsTable::getVectorData(table, "PSFSCALE", values);
   
   if (isFront) {
      m_par0 = values.at(0);
      m_par1 = values.at(1);
   } else {
      m_par0 = values.at(2);
      m_par1 = values.at(3);
   }
   m_index = values.at(4);

   m_psf_pars.resize(values.size());
   std::copy(values.begin(), values.end(), m_psf_pars.begin());

   delete table;
}

} // namespace latResponse
