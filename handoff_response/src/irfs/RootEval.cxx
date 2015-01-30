/** @file RootEval.cxx
@brief implement class RootEval

$Header$
*/

#include "RootEval.h"
// get definitions from the generation guys -- @todo move up
#include "../gen/PointSpreadFunction.h"
#include "../gen/Dispersion.h"

#include "Bilinear.h"

#include "TFile.h"
#include "TH2F.h"

#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include "st_facilities/GaussianQuadrature.h"

using namespace handoff_response;

#include "Table.h"

namespace {
   class PsfIntegrand {
   public:
      /// @param energy True photon energy (MeV)
      /// @param theta Incident inclination (degrees)
      /// @param phi Incident azimuthal angle (degrees)
      PsfIntegrand(double * pars) : m_pars(pars) {}

      /// @param sep angle between true direction and measured (radians)
      double operator()(double sep) const {
         return PointSpreadFunction::function(&sep, m_pars)*std::sin(sep);
      }
   private:
      double * m_pars;
   };
   class EdispIntegrand {
   public:
      EdispIntegrand(double * pars, double etrue, double costh, bool isFront)
         : m_pars(pars), m_etrue(etrue), m_costh(costh), m_isFront(isFront) {}

      double operator()(double emeas) const {
        double xx((emeas - m_etrue)/m_etrue);
                   //          /Dispersion::scaleFactor(m_etrue, m_costh, m_isFront));
         return Dispersion::function(&xx, m_pars)/m_etrue;
      }

   private:
      double * m_pars;
      double m_etrue;
      double m_costh;
      bool m_isFront;
   };
} // anonymous namespace

RootEval::RootEval(TFile * f, std::string eventtype)
  : IrfEval(eventtype),
    m_f(f), m_loge_last(0), m_costh_last(0), 
    m_loge_last_edisp(0), m_costh_last_edisp(0),
    m_aeff(setupHist("AEFF/aeff")) {
  setupParameterTables("PSF", m_psfTables);
  setupParameterTables("EDISP", m_dispTables);
}

RootEval::~RootEval() {
   delete m_f;
}

void RootEval::
setupParameterTables(const std::string & dirname,
                     std::vector<Table *> & tables) {
  std::string fullname(eventClass()+"/"+dirname);
  bool check=m_f->cd(fullname.c_str());
  if(!check) throw("could not cd to "+fullname);
  TDirectory *curdir = gDirectory;
  TList * keys = curdir->GetListOfKeys();
  for (int i = 0; i< keys->GetEntries(); ++i) {
    std::string histname(keys->At(i)->GetName());
    //the scaling pars histo are dealt with separately
    if(histname.find("scaling_params")==std::string::npos){
      std::cout<<fullname<<" "<<histname<<std::endl;
      tables.push_back(setupHist(dirname+"/"+histname));
    }
   }
}

double RootEval::aeff(double energy, double theta, double /*phi*/) {
   static double factor(1e4); // from m^2 to cm&2
   double costh(cos(theta*M_PI/180));
   if (costh==1.0) {
      costh = 0.9999; // avoid edge of bin
   }
   bool interpolate;
// Do not extrapolate past largest tabulated angle.
   if (costh < m_aeff->minCosTheta()) {
      return 0;
   }
   return factor*m_aeff->value(log10(energy), costh, interpolate=true);
}

double RootEval::aeffmax(){
   return m_aeff->maximum();
}

double RootEval::
psf(double delta, double energy, double theta, double /*phi*/) {
   double costh(cos(theta*M_PI/180));
   return PointSpreadFunction::function(&delta, psf_par(energy, costh));
}

double RootEval::
psf_integral(double delta, double energy, double theta, double /*phi*/) {
   double costh(cos(theta*M_PI/180));
   double * par(psf_par(energy, costh));
   double value = 
      PointSpreadFunction::integral(&delta, par)*(2.*M_PI*par[1]*par[1]);
   return value;
}

double RootEval::
dispersion(double emeas, double energy, double theta, double /*phi*/) {
   double costh(cos(theta*M_PI/180)), x(emeas/energy-1);

   // prescale x
   //x = x/Dispersion::scaleFactor(energy, costh, isFront());

   // get dispersion for prescaled var
   double ret = Dispersion::function(&x, disp_par(energy, costh));

   return ret/energy;
}

void RootEval::getPsfPars(double energy, double inclination, 
                          std::map<std::string, double> & params) {
   params.clear();
   double mu(cos(inclination*M_PI/180.));
   double * pars(psf_par(energy, mu));
   const std::vector<std::string> & parnames(PointSpreadFunction::pnames);
   for (size_t i(0); i < parnames.size(); i++) {
      params[parnames.at(i)] = pars[i];
   }
}

Table * RootEval::setupHist(std::string name) {
  std::string fullname(eventClass()+"/"+name);
   TH2F* h2 = (TH2F*)m_f->GetObjectChecked((fullname).c_str(), "TH2F");
   if (h2==0) {
      throw std::invalid_argument("RootEval: could not find plot "+fullname);
   }
   return new Table(h2);
}

double * RootEval::psf_par(double energy, double costh) {
   static double par[5];
   double loge(::log10(energy));
   if (costh == 1.0) {  // Why is this necessary?
      costh = 0.9999;
   }
   
   if (loge == m_loge_last && costh == m_costh_last) {
      return par;
   }
   
   m_loge_last = loge;
   m_costh_last = costh;
   
   for (unsigned int i = 0; i < m_psfTables.size(); ++i) {
      par[i] = m_psfTables[i]->value(loge,costh);
   }
   
   // rescale the sigma value after interpolation
   static double zdir(1.0);
   //par[1] *= PointSpreadFunction::scaleFactor(energy, zdir, isFront());
   
   if (par[1] == 0 || par[2] == 0 || par[3] == 0) {
      std::ostringstream message;
      message << "handoff_response::RootEval: psf parameters are zero "
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

double * RootEval::disp_par(double energy, double costh) {
    bool interpolate(false);

    char * foo;
    if ( (foo = ::getenv("INTERPOLATE_EDISP")) ) {
       if (std::string(foo) == "true") {
          interpolate = true;
       } else {
          interpolate = false;
       }
    }

    static double par[10];
    double loge(::log10(energy));

    if (costh == 1.0) {
       costh = 0.9999;
    }

    /// save some time, if querying for the same energy and theta
    if (loge == m_loge_last_edisp && costh == m_costh_last_edisp) {
       return par;
    }

    m_loge_last_edisp = loge;
    m_costh_last_edisp = costh;

    for (unsigned int i=0; i < m_dispTables.size(); ++i) {
        par[i] = m_dispTables[i]->value(loge, costh, interpolate);
    }

    ::EdispIntegrand bar(par, energy, costh, isFront());
    double err(1e-5);
    int ierr;
    double norm = 
       st_facilities::GaussianQuadrature::dgaus8(bar, energy/10.,
                                                 energy*3., err, ierr);
    
    par[0] /= norm;

    return par;
}

void RootEval::
createMap(std::string filename,
          std::map<std::string, handoff_response::IrfEval *> & evals) {
   TFile * file = new TFile(filename.c_str(), "readonly");
   if (!file->IsOpen()) { 
      throw std::invalid_argument("Could not load the file " + filename);
   }
   TList * keys = file->GetListOfKeys();
   for (int i = 0; i< keys->GetEntries(); ++i) {
      std::string eventclass(keys->At(i)->GetName());
      evals[eventclass] = new RootEval(file, eventclass);
      // evals[eventclass+"/front"] = new RootEval(file, eventclass + "/front");
      // evals[eventclass+"/back"] = new RootEval(file, eventclass + "/back");
   }
}
