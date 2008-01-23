/**
 * @file Psf.cxx
 * @brief Implementation for Psf class.
 * @author J. Chiang
 *
 * $Header$
 */

#include <stdexcept>

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "Psf.h"

namespace {
   double sqr(double x) {
      return x*x;
   }
}

//namespace latResponse {

Psf::Psf(const std::string & fitsfile, const std::string & extname,
         bool isFront)
   : m_loge_last(0), m_costh_last(0) {
   readPars(fitsfile, extname);
   readScaling(fitsfile);
}

Psf::Psf(const Psf & rhs) : irfInterface::IPsf(rhs), m_pars(rhs.m_pars), 
                            m_thin0(rhs.m_thin0), m_thin1(rhs.m_thin1),
                            m_thick0(rhs.m_thick0), m_thick1(rhs.m_thick1),
                            m_index(rhs.m_index), m_loge_last(0), 
                            m_costh_last(0)  {}
   
Psf::~Psf() {}

double Psf::value(const astro::SkyDir & appDir, 
                  double energy, 
                  const astro::SkyDir & srcDir, 
                  const astro::SkyDir & scZAxis,
                  const astro::SkyDir & scXAxis, 
                  double time) const {
   (void)(scXAxis);
   double sep(appDir.difference(srcDir)*180./M_PI);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   double phi(0);
   return value(sep, energy, theta, phi, time);
}

double Psf::value(double separation, double energy, double theta,
                  double phi, double time) const {
   (void)(phi);
   (void)(time);
   double * my_pars(pars(energy, std::cos(theta*M_PI/180.)));
   return old_function(separation*M_PI/180., my_pars);
}

double Psf::angularIntegral(double energy, double theta, 
                            double phi, double radius, double time) const {
//    double time(0);
//    double integral = IPsf::angularIntegral(energy, theta, phi, radius, time);
   double * my_pars(pars(energy, std::cos(theta*M_PI/180.)));
   return old_integral(radius*M_PI/180., my_pars)*(2.*M_PI*::sqr(my_pars[1]));
}

double angularIntegral(double energy,
                       const astro::SkyDir & srcDir,
                       const astro::SkyDir & scZAxis,
                       const astro::SkyDir & scXAxis,
                       const std::vector<irfInterface::AcceptanceCone *> 
                       & acceptanceCones,
                       double time) {
   (void)(scXAxis);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   double phi(0);
   return angularIntegral(energy, srcDir, theta, phi, acceptanceCones, time);
}

double Psf::old_integral(double sep, double * pars) {
   double ncore(pars[0]);
   double sigma(pars[1]);
   double gcore(pars[2]);
   double gtail(pars[3]);
   double ntail = ncore*(old_base_function(ub, sigma, gcore)
                         /old_base_function(ub, sigma, gtail));
   double r = sep/sigma;
   double u = r*r/2.;
   return (ncore*old_base_integral(u, sigma, gcore) + 
           ntail*old_base_integral(u, sigma, gtail));
}

double Psf::old_base_integral(double u, double sigma, double gamma) {
   (void)(sigma);
   return 1. - std::pow(1. + u/gamma, 1. - gamma);
}

double Psf::old_function(double sep, double * pars) const {
   double ncore(pars[0]);
   double sigma(pars[1]);
   double gcore(pars[2]);
   double gtail(pars[3]);
   double ntail = ncore*(psf_base(ub, sigma, gcore)
                         /psf_base(ub, sigma, gtail));
   double r = sep/sigma;
   double u = r*r/2.;
   return (ncore*old_base_function(u, sigma, gcore) +
           ntail*old_base_function(u, sigma, gtail));
}

double Psf::old_base_function(double u, double sigma, double gamma) const {
   (void)(sigma);
   return (1. - 1./gamma)*pow(1. + u/gamma, -gamma);
}

double * Psf::pars(double energy, double costh) const {
   static double par[5];
   double loge(std::log10(energy));
   if (costh == 1.0) {  // Why is this necessary?
      costh = 0.9999;
   }
   
   if (loge == m_loge_last && costh == m_costh_last) {
      return par;
   }
   
   m_loge_last = loge;
   m_costh_last = costh;
   
   for (size_t i(0); i < m_parNames.size(); i++) {
      par[i] = parTable(m_parNames.at(i)).value(loge,costh);
   }
   
   // Rescale the sigma value after interpolation
   static double zdir(1.0);
   par[1] *= scaleFactor(energy, m_isFront);
   
   if (par[1] == 0 || par[2] == 0 || par[3] == 0) {
      std::ostringstream message;
      message << "latResponse::Psf::pars: psf parameters are zero "
              << "when computing solid angle normalization:\n"
              << "\tenergy = " << energy << "\n"
              << "\tcosth  = " << zdir   << "\n"
              << "\tpar[1] = " << par[1] << "\n"
              << "\tpar[2] = " << par[2] << "\n"
              << "\tpar[3] = " << par[3] << std::endl;
      std::cerr << message.str() << std::endl;
      throw std::runtime_error(message.str());
   }
   
// Ensure that the Psf integrates to unity.
   double norm;
   static double theta_max(M_PI/2.);
//   if (energy < 70.) { // Use the *correct* integral of Psf over solid angle.
   if (energy < 120.) { // Use the *correct* integral of Psf over solid angle.
      ::PsfIntegrand foo(par);
      double err(1e-5);
      int ierr;
      norm = st_facilities::GaussianQuadrature::dgaus8(foo, 0, theta_max,
                                                       err, ierr);
      par[0] /= norm*2.*M_PI;
   } else { // Use small angle approximation.
      norm = PointSpreadFunction::integral(&theta_max, par);
      par[0] /= norm*2.*M_PI*par[1]*par[1];
   }

   return par;
}

const FitsTable & Psf::parTable(const std::string & name) const {
   std::map<std::string, FitsTable>::const_iterator table =
      m_pars.find(name);
   if (table == m_pars.end()) {
      throw std::runtime_error("latResponse::Psf::parTable: "
                               "table name not found.");
   }
   return *table;
}

double Psf::scaleFactor(double energy, bool thin) const {
   double tt(std::pow(energy/100., m_index));
   if (thin) {
      return std::sqrt(::sqr(m_thin0*tt) + sqr(m_thin1));
   }
   return std::sqrt(::sqr(m_thick0*tt) + sqr(m_thick1));
}

void Psf::readPars(const std::string & fitsfile, 
                   const std::string & extname) {
   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());
   const tip::Table * table(fileSvc.readTable(fitsfile, extname));
   const std::vector<std::string> & validFields(table->getValidFields());

   // The first four columns *must* be "ENERG_LO", "ENERG_HI", "CTHETA_LO",
   // "CTHETA_HI", in that order.
   char * boundsName[] = {"energ_lo", "energ_hi", "ctheta_lo", "ctheta_hi"};
   for (size_t i(0); i < 4; i++) {
      if (validFields.at(i) != boundsName[i]) {
         std::ostringstream message;
         message << "latResponse::Psf::readPars: "
                 << "invalid header in " << fitsfile << "  "
                 << validFields.at(i) << "  " << i;
         throw std::runtime_error(message.str());
      }
   }

   // Read in the table values for the remaining rows.
   for (size_t i(4); i < validFields.size(); i++) {
      const std::string & tablename(validFields.at(i));
      m_parNames.push_back(tablename);
      m_pars.insert(std::make_pair(tablename, 
                                   FitsTable(fitsfile, extname, tablename)));
   }

   delete table;
}

void Psf::readScaling(const std::string & fitsfile, 
                      const std::string & extname) {
   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());
   const tip::Table * table(fileSvc.readTable(fitsfile, extname));

   std::vector<float> values;

   FitsTable::getVectorData(table, "PSFSCALE", values);
   
   m_thin0 = values.at(0);
   m_thin1 = values.at(1);
   m_thick0 = values.at(2);
   m_thick1 = values.at(3);
   m_index = values.at(4);

   delete table;
}

} // namespace latResponse

