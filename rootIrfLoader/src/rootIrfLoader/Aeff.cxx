/**
 * @file Aeff.cxx
 * @brief Wrapper class for IAeff for use from ROOT
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfInterface/Irfs.h"

#include "Aeff.h"

namespace rootIrfLoader {

Aeff::Aeff(const std::string & irfs) : IrfBase(irfs), m_aeff(0) {
   setIrfs(m_currentIrfs);
}

Aeff::Aeff(const Aeff & rhs) : IrfBase(rhs) {
   m_aeff = rhs.m_aeff->clone();
}

Aeff & Aeff::operator=(const Aeff & rhs){
   if (*this != rhs) {
      IrfBase::operator=(rhs);
      delete m_aeff;
      m_aeff = rhs.m_aeff->clone();
   }
   return *this;
}

bool Aeff::operator==(const Aeff & rhs) const {
   return this->currentIrfs() == rhs.currentIrfs();
}

Aeff::~Aeff() {
   delete m_aeff;
}

void Aeff::getRef(irfInterface::Irfs * irfs) {
   delete m_aeff;
   m_aeff = irfs->aeff()->clone();
}

double Aeff::operator()(double energy, double theta, double phi) const {
   return m_aeff->value(energy, theta, phi);
}

} // namespace rootIrfLoader
