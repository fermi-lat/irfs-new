// -*- mode: c++ -*-
/**
 * @file pyIrfLoader.in 
 * @brief Interface file for SWIG generated wrappers.  This needs to be
 * processed by setup.py in order to resolve package paths.
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */
%module pyIrfLoader
%{
#include "irfInterface/IAeff.h"
#include "irfInterface/IPsf.h"
#include "irfInterface/IEdisp.h"
#include "irfInterface/IEfficiencyFactor.h"
#include "irfInterface/Irfs.h"
#include "irfInterface/IrfsFactory.h"
#include "irfInterface/AcceptanceCone.h"
#include "irfLoader/Loader.h"
#include "latResponse/Aeff.h"
#include "latResponse/Bilinear.h"
#include "latResponse/Edisp3.h"
#include "latResponse/EdispInterpolator.h"
#include "latResponse/FitsTable.h"
#include "latResponse/ParTables.h"
#include "latResponse/PsfBase.h"
#include "latResponse/Psf3.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Vector/ThreeVector.h"
// EAC, add ProjBase sub-classes
#include "astro/ProjBase.h"
#include "astro/SkyProj.h"
#include "astro/HealpixProj.h"
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
%}
%include stl.i
%exception {
   try {
      $action
   } catch (std::exception & eObj) {
      PyErr_SetString(PyExc_RuntimeError, const_cast<char*>(eObj.what()));
      return NULL;
   }
}
%template(DoublePair) std::pair<double, double>;
%template(DoubleVector) std::vector<double>;
%template(StringVector) std::vector<std::string>;
%template(IrfVector) std::vector<irfInterface::Irfs>;
%template(ConeVector) std::vector<irfInterface::AcceptanceCone *>;
%include CLHEP/Vector/ThreeVector.h
// EAC, add ProjBase sub-classes
%include astro/ProjBase.h
%include astro/SkyProj.h
%include astro/HealpixProj.h
%include astro/SkyDir.h
%include irfInterface/AcceptanceCone.h
%include irfInterface/IAeff.h
%include irfInterface/IPsf.h
%include irfInterface/IEdisp.h
%include irfInterface/IEfficiencyFactor.h
%include irfInterface/Irfs.h
%include irfInterface/IrfsFactory.h
%include irfLoader/Loader.h
%include latResponse/Bilinear.h
%include latResponse/ParTables.h
%include latResponse/Aeff.h
%include latResponse/PsfBase.h
%include latResponse/Psf3.h
%include latResponse/EdispInterpolator.h
%include latResponse/Edisp3.h
%extend astro::SkyDir {
   astro::SkyDir cross(const astro::SkyDir & other) {
      return astro::SkyDir(self->dir().cross(other.dir()));
   }
   double dot(const astro::SkyDir & other) {
      return self->dir().dot(other.dir());
   }
}
%extend irfInterface::IPsf {
   std::pair<double, double> app_dir(double energy, double inclination) {
      astro::SkyDir srcDir(0, 0);
      astro::SkyDir scZAxis(0, inclination);
      astro::SkyDir scXAxis(90., 0);
      astro::SkyDir appDir = self->appDir(energy, srcDir, scZAxis, scXAxis);
      double separation = appDir.difference(srcDir)*180./M_PI;
      double phi = CLHEP::RandFlat::shoot()*360.;
      return std::make_pair<double, double>(separation, phi);
   }

   std::vector<double> value(const std::vector<double>& separation, double energy, double theta, 
			      double phi, double time = 0) const {
      std::vector<double> v;
      for(std::vector<double>::const_iterator itr = separation.begin();
	  itr != separation.end(); ++itr)
	v.push_back(self->value(*itr,energy,theta,phi,time));
 
      return v;
  }
}

%extend irfInterface::IEdisp {

   std::vector<double> value(const std::vector<double>& appEnergy, double energy, 
			     double theta, double phi, double time=0) const {
     std::vector<double> v;
     for(std::vector<double>::const_iterator itr = appEnergy.begin();
	 itr != appEnergy.end(); ++itr)
       v.push_back(self->value(*itr,energy,theta,phi,time));
     
     return v;
   }
}

%extend irfInterface::IrfsFactory {
   std::vector<std::string> irfNames() {
      std::vector<std::string> names;
      self->getIrfsNames(names);
      return names;
   }
}

%extend latResponse::Aeff {
   static latResponse::Aeff * cast(irfInterface::IAeff * iaeff) {
      latResponse::Aeff * aeff = 
         dynamic_cast<latResponse::Aeff *>(iaeff);
      if (aeff == 0) {
         throw std::runtime_error("Cannot cast to a Aeff.");
      }
      return aeff;
   }
}

%extend latResponse::Psf3 {
   static latResponse::Psf3 * cast(irfInterface::IPsf * ipsf) {
      latResponse::Psf3 * psf3 = 
         dynamic_cast<latResponse::Psf3 *>(ipsf);
      if (psf3 == 0) {
         throw std::runtime_error("Cannot cast to a Psf3.");
      }
      return psf3;
   }
}

%extend latResponse::Edisp3 {
   static latResponse::Edisp3 * cast(irfInterface::IEdisp * iedisp) {
      latResponse::Edisp3 * edisp3 = 
         dynamic_cast<latResponse::Edisp3 *>(iedisp);
      if (edisp3 == 0) {
         throw std::runtime_error("Cannot cast to a Edisp3.");
      }
      return edisp3;
   }
}
