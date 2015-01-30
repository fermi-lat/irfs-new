/** @file PsfPlots.cxx
@brief implement PsfPlots

$Header$
*/

#include "FisheyePlots.h"
#include "PointSpreadFunction.h"
#include "Setup.h"

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
    double ymin = 1e-3, ymax=10.0;
    bool ylog(true);
#else // linear to emphasize integral
    double ymin = 0, ymax=0.5;
    bool ylog(false);

#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FisheyePlots::FisheyePlots( IrfAnalysis& irf, std::ostream& log, 
			    embed_python::Module & py)
  : m_irf(irf),
    m_binner(py,"FisheyeBins"),
    m_log(&log)
{
   try {
      py.getList("PSF.scaling_pars", m_psf_scaling_pars);
      std::cout << "FisheyePlots : using PSF scaling parameters:" << std::endl;
      for (size_t i(0); i < m_psf_scaling_pars.size(); i++) {
        std::cout << m_psf_scaling_pars[i] << "  ";
      }
      std::cout << std::endl;
   } catch(std::invalid_argument &) {
     throw std::runtime_error("FisheyePlots::FisheyePlots: PSF.scaling_pars not found in python setup script!");
   }

  m_hists.resize(m_binner.energy_bins()*m_binner.angle_bins());
  for (int ebin = 0; ebin < m_binner.energy_bins(); ++ebin) {
    for (int abin = 0; abin < m_binner.angle_bins(); ++abin) {
      int id = m_binner.ident(ebin,abin);
      std::ostringstream title;
      title << (int)(m_binner.eCenter(ebin)+0.5) << " MeV," ;
      
      title << m_binner.angle(abin) << "-"<< m_binner.angle(abin+1) 
	    << " degrees";

      m_hists[id]=FisheyeHist(IrfBinner::hist_name(abin, ebin, "fisheye")
			      , title.str());
    }
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FisheyePlots::~FisheyePlots()
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FisheyePlots::fill(double angle_diff, double energy, 
			double costheta)
{
    int z_bin = binner().angle_bin( costheta );     
    if( z_bin>= binner().angle_bins()) return;
    int e_bin = binner().energy_bin(energy);        
    if( e_bin<0 || e_bin>= binner().energy_bins() )return;

    int id =  binner().ident(e_bin, z_bin);
    double scaled_delta = 
      angle_diff/PointSpreadFunction::scaleFactor(energy, costheta, m_psf_scaling_pars);
    m_hists[id].fill(scaled_delta);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FisheyePlots::summarize()
{
  FisheyeHist::summary_title(out());
  for( PSFlist::iterator it = m_hists.begin(); it!=m_hists.end(); ++it){
    (*it).summarize(out());
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FisheyePlots::fit()
{
  for( PSFlist::iterator it = m_hists.begin(); it!=m_hists.end(); ++it){
    (*it).fit();
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FisheyePlots::draw(const std::string &ps_filename ) {

    gStyle->SetOptFit(111);

    TCanvas c("c","fisheye plots", 0, 0, 1400, 1000);
    int ps_flag(ps_filename.find(".ps"));

    for( int abin=0; abin< binner().angle_bins(); ++abin){
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
void FisheyePlots::fillParameterTables()
{
    // make a set of 2-d histograms with values of the fit parameters
    // binning according to energy and costheta bins 

    for( int i = 0; i< FisheyeHist::npars(); ++i){
        std::string name(FisheyeHist::parname(i));
        TH2F* h2 = new TH2F(name.c_str(), (name+";log energy; costheta").c_str() 
            ,binner().energy_bins(), &*binner().energy_bin_edges().begin()
            ,binner().angle_bins(),  &*binner().angle_bin_edges().begin()
        );

        std::vector<double> pars;
        std::vector<double> errs;

        int index(0);
        for( PSFlist::iterator it = m_hists.begin(); it!=m_hists.end(); ++it, ++index){
            it->getFitPars(pars);
            it->getFitErrs(errs);

	    int abin = index/binner().energy_bins();
	    int ebin = index%binner().energy_bins();

	    double theta =  binner().angle(abin);
	    double costheta = binner().cthCenter(abin);
            double logenergy = log10(binner().eCenter(index % binner().energy_bins()));
	    //            h2->Fill(logenergy, costheta, pars[i]);
	    h2->SetBinContent(ebin+1,binner().angle_bins()-abin,-pars[i]);
	    h2->SetBinError(ebin+1,binner().angle_bins()-abin,errs[i]);
        }
        h2->GetXaxis()->CenterTitle();
        h2->GetYaxis()->CenterTitle();
        h2->Write();

    }
}



