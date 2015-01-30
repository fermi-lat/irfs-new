/** @file DispPlots.cxx
@brief implement DispPlots


*/

#include "DispPlots.h"

#include "TreeWrapper.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "IrfAnalysis.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {
#if 1 // log-scale plots to see tails
    double ymin = 1e-6, ymax=1.;
    bool ylog(true);
#else // linear to emphasize integral
    double ymin = 0, ymax=0.5;
    bool ylog(false);
#endif
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DispPlots::DispPlots( IrfAnalysis& irf, 
		      std::ostream& log, 
		      embed_python::Module & py)
: m_irf(irf)
, m_binner(irf.binner())
, m_log(&log)
{
   // Get energy dispersion scale parameters from input file.
   //std::vector<double> scaling_pars;
   try {
      py.getList("Edisp.scaling_pars", m_edisp_scaling_pars);
      std::cout << "Using energy dispersion scaling parameters:" << std::endl;
      for (size_t i(0); i < m_edisp_scaling_pars.size(); i++) {
        std::cout << m_edisp_scaling_pars[i] << "  ";
      }
      std::cout << std::endl;
   } catch(std::invalid_argument &) {
     throw std::runtime_error("DispPlots//DispPlots: Edisp.scaling_pars not found in python setup script!");
   }

    m_hists.resize(binner().size());
    for (int ebin = 0; ebin < binner().energy_bins(); ++ebin) {
        for (int abin = 0; abin <= binner().angle_bins(); ++abin) {
            int id = binner().ident(ebin,abin);
            std::ostringstream title;
            title << (int)(binner().eCenter(ebin)+0.5) << " MeV," ;
            if ( abin < binner().angle_bins() ) {
                title << binner().angle(abin) << "-"<< binner().angle(abin+1) << " scaled";
            }else {
                title <<  binner().angle(0) << "-"<< binner().angle(binner().angle_bins()-2) << " scaled";
            }
            m_hists[id]=Dispersion(IrfBinner::hist_name(abin, ebin, "disp")
				   , title.str(), py);
            m_hists[id].setScaleFactorParameters(m_edisp_scaling_pars);
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DispPlots::~DispPlots()
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DispPlots::fill(double deviat, double energy, double costheta)
{
    int z_bin = binner().angle_bin( costheta );     if( z_bin>= binner().angle_bins()) return;
    int e_bin = binner().energy_bin(energy);        if( e_bin<0 || e_bin>= binner().energy_bins() )return;

//     int id =  binner().ident(e_bin, z_bin);
    double scaled_delta =deviat/Dispersion::scaleFactor(energy, costheta,m_edisp_scaling_pars);

//     m_hists[id].fill(scaled_delta);

// use over-lapping bins if da, de are non-zero:
    int da(binner().edispEnergyOverLap());
    int de(binner().edispAngleOverLap());
    for (int eoffset(-de); eoffset < de + 1; eoffset++) {
      for (int aoffset(-da); aoffset < da + 1; aoffset++) {
         int indx(binner().hist_id(e_bin + eoffset, z_bin + aoffset));
         if (indx >= 0) {
	     m_hists[indx].fill(scaled_delta);

	 }
      }
    }
    // set special combined hist, accumulate all but last bins of angles
    if( z_bin< binner().angle_bins()-2) {
        m_hists[binner().ident(e_bin, binner().angle_bins())].fill(scaled_delta);
    }

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DispPlots::summarize()
{
    // summarize edisp plot contents, fit 
    //          32 MeV, 0-37 degrees       675     0.643     0.538"
    Dispersion::summary_title(out());
    for( Displist::iterator it = m_hists.begin(); it!=m_hists.end(); ++it){
        (*it).summarize(out());
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DispPlots::fit()
{

    for( Displist::iterator it = m_hists.begin(); it!=m_hists.end(); ++it){
        (*it).fit();
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DispPlots::draw(const std::string &ps_filename ) {

    gStyle->SetOptFit(111);

    TCanvas c("c","disp plots", 0, 0, 1400, 1000);
    int ps_flag(ps_filename.find(".ps"));

    for( int abin=0; abin<= binner().angle_bins(); ++abin){
        int rows=3;
        m_irf.divideCanvas(c,(binner().energy_bins()+1)/rows,rows, 
            std::string("Plots from ") +m_irf.summary_filename());
        for(int ebin=0; ebin<binner().energy_bins(); ++ebin){
            c.cd(ebin+1);
            gPad->SetRightMargin(0.02);
            gPad->SetTopMargin(0.03);
            m_hists[binner().ident(ebin,abin)].draw(ymin, ymax, ylog);
        }
        std::cout << "Printing page #" << (abin+1) << std::endl; 
        if( ps_flag >0 ) {
            // doing a ps file with multiple plots
            if( abin==0) c.Print( (ps_filename+"(").c_str());
            else if (abin<binner().angle_bins()) c.Print(ps_filename.c_str());
            else c.Print( (ps_filename+")").c_str());
        }else{
            std::stringstream currentfile;
            int dot(ps_filename.find_last_of("."));
            currentfile << ps_filename.substr(0,dot) << "_"<<(abin+1) << ps_filename.substr(dot);
            c.Print(currentfile.str().c_str());
        }
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DispPlots::fillParameterTables()
{
    // make a set of 2-d histograms with values of the fit parameters
    // binning according to energy and costheta bins 
    // (skip norm, the first one)

    //use first Dispersion object to get the global info on fit parameter number and names :
    std::vector<std::string>  names=m_hists[0].getFitParNames();
    int npars = names.size();
    for( int i = 0; i<npars; ++i){
        std::string name(names[i]);
        TH2F* h2 = new TH2F(name.c_str(), (name+";log energy; costheta").c_str() 
            ,binner().energy_bins(), &*binner().energy_bin_edges().begin()
            ,binner().angle_bins(),  &*binner().angle_bin_edges().begin()
        );

        std::vector<double> pars;
        int index(0);
        for( Displist::iterator it = m_hists.begin(); it!=m_hists.end(); ++it, ++index){
            it->getFitPars(pars);
            double costheta = 0.95 - 0.1*(index/binner().energy_bins());
            double logenergy = log10(binner().eCenter(index % binner().energy_bins()));

            h2->Fill(logenergy, costheta, pars[i]);
        }
        h2->GetXaxis()->CenterTitle();
        h2->GetYaxis()->CenterTitle();
        h2->Write();

    }

    // Write scaleFactor parameter values
    size_t nbins(m_edisp_scaling_pars.size());
    std::string histname("edisp_scaling_params");
    TH1F * h1 = new TH1F(histname.c_str(), 
                         (histname + ";index").c_str(),
                         nbins, 0,nbins-1);
    for (size_t i(0); i < m_edisp_scaling_pars.size(); i++) {
       h1->SetBinContent(i, m_edisp_scaling_pars[i]);
    }
    h1->GetXaxis()->CenterTitle();
    h1->Write();
}
