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
#include "irfInterface/Irfs.h"
#include "irfInterface/IrfsFactory.h"
#include "irfInterface/AcceptanceCone.h"
#include "irfLoader/Loader.h"
#include "CLHEP/Random/RandFlat.h"
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
%include astro/SkyProj.h
%include astro/SkyDir.h
%include irfInterface/AcceptanceCone.h
%include irfInterface/IAeff.h
%include irfInterface/IPsf.h
%include irfInterface/IEdisp.h
%include irfInterface/Irfs.h
%include irfInterface/IrfsFactory.h
%include irfLoader/Loader.h
%extend irfInterface::IPsf {
   std::pair<double, double> app_dir(double energy, double inclination) {
      astro::SkyDir srcDir(0, 0);
      astro::SkyDir scZAxis(0, inclination);
      astro::SkyDir scXAxis(90., 0);
      astro::SkyDir appDir = self->appDir(energy, srcDir, scZAxis, scXAxis);
      double separation = appDir.difference(srcDir)*180./M_PI;
      double phi = RandFlat::shoot()*360.;
      return std::make_pair<double, double>(separation, phi);
   }
}
%extend irfInterface::IrfsFactory {
   std::vector<std::string> irfNames() {
      std::vector<std::string> names;
      self->getIrfsNames(names);
      return names;
   }
}
