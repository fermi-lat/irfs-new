/**
 * @file Edisp.cxx
 * @brief Wrapper class for IEdisp for use from ROOT
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfInterface/Irfs.h"

#include "Edisp.h"

namespace rootIrfLoader {

Edisp::Edisp(const std::string & irfs) : IrfBase(irfs), m_edisp(0) {
   setIrfs(m_currentIrfs);
}

Edisp::Edisp(const Edisp & rhs) : IrfBase(rhs) {
   m_edisp = rhs.m_edisp->clone();
}

Edisp & Edisp::operator=(const Edisp & rhs) {
   if (*this != rhs) {
      IrfBase::operator=(rhs);
      delete m_edisp;
      m_edisp = rhs.m_edisp->clone();
   }
   return *this;
}

Edisp::~Edisp() {
   delete m_edisp;
}

bool Edisp::operator==(const Edisp & rhs) const {
   return this->currentIrfs() == rhs.currentIrfs();
}

void Edisp::getRef(irfInterface::Irfs * irfs) {
   delete m_edisp;
   m_edisp = irfs->edisp()->clone();
}

double Edisp::operator()(double appEnergy, double energy, double theta,
                         double phi) const {
   return m_edisp->value(appEnergy, energy, theta, phi);
}

double Edisp::integral(double emin, double emax, double energy, 
                              double theta, double phi) const {
   return m_edisp->integral(emin, emax, energy, theta, phi);
}

} // namespace rootIrfLoader
