/**
 * @file AeffPhiDep.cxx
 * @brief Class to manage fits of phi-dependence of the effective area.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <sstream>

#include "TCanvas.h"
#include "TH2F.h"

#include "AeffPhiDep.h"
#include "IrfAnalysis.h"
#include "PhiDepHist.h"

AeffPhiDep::AeffPhiDep(IrfAnalysis & irf) 
   : m_irf(irf), m_binner(irf.binner()) {
   m_hists.resize(m_binner.size());
   for (size_t ebin(0); ebin < m_binner.energy_bins(); ebin++) {
      for (size_t abin(0); abin < m_binner.angle_bins(); abin++) {
         size_t id(m_binner.ident(ebin, abin));
         std::ostringstream title;
         title << static_cast<size_t>(m_binner.eCenter(ebin) + 0.5) 
               << " MeV,";
         if (abin < m_binner.angle_bins()) {
            title << m_binner.angle(abin) << "-" 
                  << m_binner.angle(abin+1);
         } else {
            title << m_binner.angle(0) << "-" 
                  << m_binner.angle(m_binner.angle_bins()-2);
         }
         m_hists.at(id) = PhiDepHist(IrfBinner::hist_name(abin, ebin, "phidep"),
                                     title.str());
      }
   }
}

AeffPhiDep::~AeffPhiDep() {}

void AeffPhiDep::fill(double mc_xdir, double mc_ydir,
                      double energy, double costheta) {
   size_t z_bin(m_binner.angle_bin(costheta));
   if (z_bin >= m_binner.angle_bins()) {
      return;
   }
   int e_bin(m_binner.energy_bin(energy));
   if (e_bin < 0 || e_bin >= static_cast<int>(m_binner.energy_bins())) {
      return;
   }

   size_t id(m_binner.ident(e_bin, z_bin));
   double tangent(mc_ydir/mc_xdir);

   m_hists.at(id).fill(tangent);
   if (z_bin < m_binner.angle_bins() - 2) {
      m_hists.at(m_binner.ident(e_bin, m_binner.angle_bins())).fill(tangent);
   }
}

void AeffPhiDep::fit() {
   for (size_t i(0); i < m_hists.size(); i++) {
      m_hists.at(i).fit();
   }
}

void AeffPhiDep::summarize() {
// Do nothing.
}

void AeffPhiDep::draw(const std::string & psfile) {
    gStyle->SetOptFit(111);

    TCanvas canvas("canvas", "aeff phi-dep", 0, 0, 1400, 1000);
    int ps_flag(psfile.find(".ps"));

    for (size_t abin(0); abin <= m_binner.angle_bins(); abin++) {
       size_t rows(3);
       m_irf.divideCanvas(canvas, (m_binner.energy_bins()+1)/rows, rows, 
                          "Plots from " + m_irf.summary_filename());
       for (size_t ebin(0); ebin < m_binner.energy_bins(); ebin++) {
          canvas.cd(ebin+1);
          gPad->SetRightMargin(0.02);
          gPad->SetTopMargin(0.03);
          m_hists.at(m_binner.ident(ebin,abin)).draw();
       }
       std::cout << "Printing page #" << (abin+1) << std::endl; 
       if (ps_flag > 0) {
          // doing a ps file with multiple plots
          if (abin == 0) {
             canvas.Print((psfile + "(").c_str());
          } else if (abin<m_binner.angle_bins()) {
             canvas.Print(psfile.c_str());
          } else {
             canvas.Print((psfile + ")").c_str());
          }
       } else {
          std::stringstream currentfile;
          int dot(psfile.find_last_of("."));
          currentfile << psfile.substr(0,dot) << "_" 
                      << (abin+1) << psfile.substr(dot);
          canvas.Print(currentfile.str().c_str());
       }
    }
}

void AeffPhiDep::fillParameterTables() {
   size_t npars(m_hists.front().pars().size());
   for (size_t i(0); i < npars; i++) {
      std::ostringstream name;
      name << "p" << i << ";log energy; costheta";
      TH2F h2(name.str().c_str(), name.str().c_str(),
              m_binner.energy_bins(), &*m_binner.energy_bin_edges().begin(),
              m_binner.angle_bins(), &*m_binner.angle_bin_edges().begin());
      for (size_t indx(0); indx < m_hists.size(); indx++) {
         double logenergy = 
            std::log10(m_binner.eCenter(indx % m_binner.energy_bins()));
         double costheta(0.95 - 0.1*(indx/m_binner.energy_bins()));
         h2.Fill(logenergy, costheta, m_hists.at(indx).pars().at(i));
      }
      h2.GetXaxis()->CenterTitle();
      h2.GetYaxis()->CenterTitle();
      h2.Write();
   }
}
