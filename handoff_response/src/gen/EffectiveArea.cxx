
/** @file EffectiveArea.h
  @brief create and manage a plot of the effective area

  $Header$
*/

#include "EffectiveArea.h"
#include "IRF.h"
#include "IrfAnalysis.h"

#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"

#include <cmath>
#include <iomanip>

namespace {

    int ebinfactor(8), anglebinfactor(2);
}

EffectiveArea::EffectiveArea( IrfAnalysis& irf, std::ostream& log)
: m_irf(irf)
, m_log(&log)
{
    m_hist = new TH2F("aeff"
        , (std::string("Effective area for ")+irf.name()+"; logE; cos(theta); effective area (m^2)").c_str()
        ,ebinfactor*IRF::energy_bins, IRF::logemin, IRF::logemin+IRF::energy_bins*IRF::logedelta
        , anglebinfactor*IRF::angle_bins, 0.2, 1.0);
}


void EffectiveArea::fill(double energy, double costheta, bool front)
{
    m_hist->Fill( ::log10(energy), ::fabs(costheta));

}

void EffectiveArea::summarize()
{
    m_norm= m_irf.aeff_per_event()*ebinfactor*anglebinfactor;
    double maxbin = m_hist->GetMaximum();
    out() << "\nEffective area histogram has " 
        << m_hist->GetEntries() << " entries."
        << std::endl;
    out() << "\tNormalizion is " << std::setprecision(6) << (m_norm*1e4) 
        << " cm^2/event: Maximum bin has " << maxbin 
        << " entries, for " << (maxbin*m_norm) << " m^2"<< std::endl;
    m_hist->GetXaxis()->CenterTitle();
    m_hist->GetYaxis()->CenterTitle();
    m_hist->GetZaxis()->CenterTitle();
#if 0 // this screws it up?
    m_hist->GetXaxis()->SetLabelOffset(2.0);
    m_hist->GetYaxis()->SetLabelOffset(2.0);
#endif
    m_hist->Scale(m_norm);
    m_hist->Write(); // update in the output file

}

void EffectiveArea::draw(const std::string &ps_filename)
{
    TCanvas c;
    m_hist->Draw("colz");
    c.Print(ps_filename.c_str());
}

void EffectiveArea::writeFitParameters(std::string outputFile, std::string treename)
{

    // Create or update the new Root file.
    TFile* file = new TFile(outputFile.c_str(), "UPDATE");

    // Create the TTree.
    TTree* tree = new TTree((treename+"-aeff").c_str(), (treename+" aeff").c_str());


}
void EffectiveArea::fillParameterTables()
{
#if 0
    m_hist->Scale(m_norm);
    m_hist->Write(); // update in the output file
#endif
}
