
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
#if 1
    int ebinfactor(8), anglebinfactor(4);
#else
    int ebinfactor(1), anglebinfactor(1);
#endif
    std::vector<MyAnalysis::Normalization>::const_iterator s_iter;

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

EffectiveArea::EffectiveArea( IrfAnalysis& irf, std::ostream& log)
: m_irf(irf)
, m_log(&log)
{
    m_hist = new TH2F("aeff"
        , (std::string("Effective area for ")+irf.name()+"; logE; cos(theta); effective area (m^2)").c_str()
        ,ebinfactor*IRF::energy_bins, IRF::logemin, IRF::logemin+IRF::energy_bins*IRF::logedelta
        , anglebinfactor*IRF::angle_bins, 0.2, 1.0);

    binCenters( m_hist, m_bins);
    // setup normalization entry.
    s_iter = irf.normalization().begin();
    m_count = s_iter->entries();
    m_numerator =  new TH2F("aeff_numerator"
        , (std::string("Effective area for ")+irf.name()+"; logE; cos(theta); effective area (m^2)").c_str()
        ,ebinfactor*IRF::energy_bins, IRF::logemin, IRF::logemin+IRF::energy_bins*IRF::logedelta
        , anglebinfactor*IRF::angle_bins, 0.2, 1.0);

    m_denominator = new TH2F("aeff_denominator"
        , (std::string("Effective area for ")+irf.name()+"; logE; cos(theta); effective area (m^2)").c_str()
        ,ebinfactor*IRF::energy_bins, IRF::logemin, IRF::logemin+IRF::energy_bins*IRF::logedelta
        , anglebinfactor*IRF::angle_bins, 0.2, 1.0);

}


void EffectiveArea::fill(double energy, double costheta, bool front, int total)
{
    if( total> m_count) updateNorm();
        
    double loge(::log10(energy)), costh(::fabs(costheta));
    m_hist->Fill( loge, ::fabs(costheta));

}

void EffectiveArea::updateNorm(bool done)
{
    // add to the numerator and denominator arrays
    for( std::vector<Fpair>::const_iterator it = m_bins.begin(); it!=m_bins.end(); ++it) {
        float loge ( it->first )
            , costh( it->second )
            , d( s_iter->value(loge, costh) )
            , count( m_hist->GetBinContent(m_hist->FindBin(loge, costh)) );
        if( count>0 && d>0) {
            m_numerator->Fill( loge, costh, d);
            m_denominator->Fill(loge,costh, d*d/count);
        }
    }
    m_hist->Reset();
    ++s_iter; 
    if( done ){

        m_count=0;//throw std::runtime_error("invalid normalization data");
        double factor( IRF::s_generated_area/IRF::logedelta/IRF::deltaCostheta*ebinfactor*anglebinfactor);
        for( std::vector<Fpair>::const_iterator it = m_bins.begin(); it!=m_bins.end(); ++it) {
            float loge ( it->first )
                , costh( it->second );
            int bin( m_hist->FindBin(loge, costh));
            float numerator(  m_numerator->GetBinContent(bin))
                , denominator(m_denominator->GetBinContent(bin));

            if( denominator==0) continue;
            m_hist->SetBinContent( bin, factor* numerator/denominator);
            m_hist->SetBinError(bin, factor/sqrt(denominator));

        }
        m_numerator->Write(); // to look at
        m_denominator->Write(); // needs to be inverted, sqrt taken for errors
        m_hist->Write();

    }
    else{
        m_count = s_iter->entries();
    }
}
void EffectiveArea::summarize()
{
    //make the quotient now
    updateNorm(true);
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
#if 1
   // m_hist->Scale(m_norm);
    m_hist->Write(); // update in the output file
#endif
}
