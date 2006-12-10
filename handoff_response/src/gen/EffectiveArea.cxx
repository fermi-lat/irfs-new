/** @file EffectiveArea.h
  @brief create and manage a plot of the effective area

  $Header$
*/

#include "EffectiveArea.h"
#include "IrfAnalysis.h"
#include "Setup.h"
#include "embed_python/Module.h"

#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TAxis.h"

#include <cmath>
#include <iomanip>

namespace {

       /// Fill vector array with the bin centers and areas from the TH2F
    class BinInfo{
    public: BinInfo(double x, double y, double area):m_x(x),m_y(y),m_area(area){}
            double x()const{return m_x;}
            double y()const{return m_y;}
            double area()const{return m_area;}
    private:
        double m_x, m_y, m_area;
    };
    void binCenters( TH2F* hist, std::vector<BinInfo >& info)
    {
        TAxis* xaxis = hist->GetXaxis();
        TAxis* yaxis = hist->GetYaxis();
        int nbinsx(xaxis->GetNbins()), nbinsy(yaxis->GetNbins());
        for(int i = 1; i<nbinsx+1; ++i){
            for( int j=1; j<nbinsy+1; ++j){
                info.push_back(BinInfo(xaxis->GetBinCenter(i),yaxis->GetBinCenter(j), 
                   xaxis->GetBinWidth(i)*yaxis->GetBinWidth(j) ));
            }
        }
    }
}
EffectiveArea::Bins::Bins()
{

    embed_python::Module& py = *(Setup::instance()->py());
    py.getList("EffectiveAreaBins.energy_bin_edges", m_energy_bin_edges);
    py.getList("EffectiveAreaBins.angle_bin_edges",  m_angle_bin_edges);    

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                 EffectiveArea 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

void EffectiveArea::fill(double energy, double costheta, bool /*front*/, int /*total*/)
{
    double loge(::log10(energy));
    m_hist->Fill( loge, ::fabs(costheta));
}

void EffectiveArea::summarize()
{
    // make nice list of the bin centers and areas for iteration below
    std::vector< BinInfo> bins;
    binCenters( m_hist, bins);

    // create histogram to fill with normalization per bin: it depends on the generated 
    // density and the bin size
    TH2F* denomhist = new TH2F(*m_hist); //make copy for defining normalization denominatore
    denomhist->Reset();
    denomhist->SetNameTitle("aeff_denom","aeff denominator");

    // setup loop over normalization entries
    std::vector<IrfAnalysis::Normalization>::const_iterator norm_iter= m_irf.normalization().begin();
    for( ; norm_iter != m_irf.normalization().end(); ++norm_iter){
        //unused double count = norm_iter->entries();

        // add to the denominator array
        for( std::vector<BinInfo>::const_iterator it = bins.begin(); it!=bins.end(); ++it) {
            float loge ( it->x() )
                , costh( it->y() )
                , d( norm_iter->value(loge, costh)* (*it).area() );
            denomhist->Fill(loge,costh, d);
        }
    }
    double factor( m_irf.generate_area() );

    double maxbin ( m_hist->GetBinContent(m_hist->FindBin(3.0, 0.95))), 
        denom( denomhist->GetBinContent(denomhist->FindBin(3.0, 0.95))), 
        norm(factor/denom);
    out() << "\nEffective area histogram has " 
        << m_hist->GetEntries() << " entries."
        << std::endl;
    out() << "\tNormalization is " << std::setprecision(6) << (norm*1e4) 
        << " cm^2/event: 1 GeV normal bin has " << int(maxbin) 
        << " entries, for " << (maxbin*norm) << " m^2"<< std::endl;
    m_hist->Sumw2();
    m_hist->Divide(denomhist);
    m_hist->Scale(factor);
    denomhist->Write(); 
    m_hist->GetXaxis()->SetTitleOffset(1.5);
    m_hist->GetYaxis()->SetTitleOffset(1.5);
    m_hist->Write();

    // make acceptance plot (all angles for now)
    // (projection only works if all bins have same size)
    TH1D* accept = m_hist->ProjectionX("accept");
    accept->Scale(2*M_PI/(m_bins.angle_bins().size()-1));
    accept->Write();
}

void EffectiveArea::draw(const std::string &ps_filename)
{
    TCanvas c;
    m_hist->Draw("surf1");
    c.Print(ps_filename.c_str());
}

void EffectiveArea::writeFitParameters(std::string outputFile, std::string treename)
{
}

void EffectiveArea::fillParameterTables()
{
    m_hist->Write(); // update in the output file
}
