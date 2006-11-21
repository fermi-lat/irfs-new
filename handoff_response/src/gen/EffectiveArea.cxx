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
#include "TAxis.h"

#include <cmath>
#include <iomanip>

namespace {

       /// Fill vector array with the bin centers from ta TH2F
    void binCenters( TH2F* hist, std::vector<std::pair<float,float> >& array)
    {
        TAxis* xaxis = hist->GetXaxis();
        TAxis* yaxis = hist->GetYaxis();
        int nbinsx(xaxis->GetNbins()), nbinsy(yaxis->GetNbins());
        for(int i = 1; i<nbinsx+1; ++i){
            for( int j=1; j<nbinsy+1; ++j){
                array.push_back(std::make_pair(xaxis->GetBinCenter(i),yaxis->GetBinCenter(j)));
            }
        }
    }
}
EffectiveArea::Bins::Bins()
: m_ebreak(4.25)
, m_ebinfactor(8)
, m_anglebinfactor(4)
, m_ebinhigh(2)
{
    double loge(   IRF::logemin)
        ,  emax(   IRF::logemin+IRF::energy_bins*IRF::logedelta)
        ,  edelta( IRF::logedelta )
        ,  factor( m_ebinfactor) ;
    // x axis, with
    for( ; loge< emax+edelta/2; loge+= edelta/factor){
        if(loge>= m_ebreak) factor=m_ebinhigh;
        m_energy_bin_edges.push_back(loge);
    }
    double angledelta(IRF::deltaCostheta / m_anglebinfactor);
    for(double costh(0.2); costh<1.01; costh+=angledelta) {
        m_angle_bin_edges.push_back(costh);
    }
    
}
double EffectiveArea::Bins::binsize(double loge, double) const{

    return IRF::deltaCostheta * IRF::logedelta 
        / ( loge<m_ebreak? m_ebinfactor :m_ebinhigh)
        / m_anglebinfactor;
}

EffectiveArea::EffectiveArea( IrfAnalysis& irf, std::ostream& log)
: m_irf(irf)
, m_log(&log)
{

    m_hist = new TH2F("aeff"
        , (std::string("Effective area for ")+irf.name()+"; logE; cos(theta); effective area (m^2)").c_str()
        , m_bins.energy_bins().size()-1, &*m_bins.energy_bins().begin()
        , m_bins.angle_bins().size()-1,  &*m_bins.angle_bins().begin()
        ) ;

}

void EffectiveArea::fill(double energy, double costheta, bool front, int /*total*/)
{
    double loge(::log10(energy)), costh(::fabs(costheta));
    m_hist->Fill( loge, ::fabs(costheta));
}

void EffectiveArea::summarize()
{
    // make nice list of the bin centers for iteration below
    std::vector< FloatPair > bins;
    binCenters( m_hist, bins);

    // create histogram to fill with normalization per bin: it depends on the generated 
    // density and the bin size
    TH2F* denomhist = new TH2F(*m_hist); //make copy for defining normalization denominatore
    denomhist->Reset();
    denomhist->SetNameTitle("aeff_denom","aeff denominator");

    // setup loop over normalization entries
    std::vector<MyAnalysis::Normalization>::const_iterator norm_iter= m_irf.normalization().begin();
    for( ; norm_iter != m_irf.normalization().end(); ++norm_iter){
        //unused double count = norm_iter->entries();

        // add to the denominator array
        for( std::vector<FloatPair>::const_iterator it = bins.begin(); it!=bins.end(); ++it) {
            float loge ( it->first )
                , costh( it->second )
                , d( norm_iter->value(loge, costh)*m_bins.binsize(loge,costh) );
            denomhist->Fill(loge,costh, d);
        }
    }
    double factor( IRF::s_generated_area );
    m_hist->Sumw2();
    m_hist->Divide(denomhist);
    m_hist->Scale(factor);
    denomhist->Write(); 
    m_hist->Write();
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
    m_hist->Write(); // update in the output file
}
