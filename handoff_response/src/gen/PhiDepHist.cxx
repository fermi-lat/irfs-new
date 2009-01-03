/**
 * @file PhiDepHist.cxx
 * @brief Class to fit parameterization of phi-dependence of effective area.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <stdexcept>

#include "TF1.h"
#include "TH1F.h"

#include "PhiDepHist.h"

TF1 * PhiDepHist::s_fit_func(new TF1("phi_func", "1 + [0]*x^[1]", 0, 0.9));

PhiDepHist::PhiDepHist() : m_hist(0), m_count(0), m_fitted(false) {
   m_pars.resize(2, 0);
}

PhiDepHist::PhiDepHist(const std::string & histname,
                       const std::string & title, 
                       size_t nbins, double xmin, double xmax) 
   : m_hist(new TH1F(histname.c_str(), title.c_str(), nbins, xmin, xmax)),
     m_nbins(nbins), m_xmin(xmin), m_xmax(xmax),
     m_count(0), m_fitted(false) {
   m_hist->GetXaxis()->SetTitle("abs(abs(atan(McYDir/McXDir))*2./pi-0.5)*2");
   m_pars.resize(2, 0);
}

PhiDepHist::~PhiDepHist() {}

void PhiDepHist::fill(double tangent) {
   if (m_fitted) {
      assert(false);
      throw std::runtime_error("Cannot call PhiDepHist::fill after "
                               "having called PhiDepHist::fit.");
   }
   double arg(std::fabs(std::fabs(std::atan(tangent))*2./M_PI - 0.5)*2);
   m_hist->Fill(arg);
   m_count++;
}

void PhiDepHist::fit() {
// Blech.  This function alters the contents of the underlying
// histogram such that the fill member function cannot be called any
// longer without corrupting the contents. This wouldn't be necessary,
// except that the THF1:;Draw() function works incredibly stupidly,
// violating encapsulation (as usual) in all sorts of ways.  ROOT
// sucks.
   if (m_hist == 0) {
      m_pars.at(0) = 0;
      m_pars.at(1) = 0;
      return;
   }
   m_fitted = true;
   double lowval((m_hist->GetBinContent(1) + m_hist->GetBinContent(2) +
                  m_hist->GetBinContent(3) + m_hist->GetBinContent(4))/4.);
   if (lowval > 0) {
      for (size_t i(1); i < m_nbins + 1; i++) {
         m_hist->SetBinContent(i, m_hist->GetBinContent(i)/lowval);
      }
   }
   s_fit_func->SetParameter(0, 2);
   s_fit_func->SetParameter(1, 1);
   m_hist->Fit(s_fit_func);
   s_fit_func->GetParameters(&m_pars[0]);
}

void PhiDepHist::draw() {
// One should call this after fit().
   if (m_hist) {
      m_hist->SetStats(true);
      m_hist->SetLineColor(kRed);
      m_hist->GetXaxis()->CenterTitle(true);
      m_hist->Draw();
      m_hist->Write();
   }
}

void PhiDepHist::getFitPars(std::vector<double> & pars) const {
   pars = m_pars;
}

double PhiDepHist::entries() const {
   return m_count;
}

const std::vector<double> & PhiDepHist::pars() const {
   return m_pars;
}
