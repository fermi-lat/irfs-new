/** @file PsfPlots.cxx
@brief implement PsfPlots

$Header$
*/

#include "PsfPlots.h"

#include "TreeWrapper.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "IRF.h"
#include "IrfAnalysis.h"

#include <cmath>
#include <iomanip>

namespace {
    double ymin = 1e-6, ymax=1.;
    bool ylog(true);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PsfPlots::PsfPlots( IrfAnalysis& irf, std::ostream& log)
: m_irf(irf)
, m_log(&log)
{

    m_hists.resize(irf.size());
    for (int ebin = 0; ebin < irf.energy_bins; ++ebin) {
        for (int abin = 0; abin <= irf.angle_bins; ++abin) {
            int id = irf.ident(ebin,abin);
            std::ostringstream title;
            title << (int)(irf.eCenter(ebin)+0.5) << " MeV," ;
            if ( abin < irf.angle_bins ) {
                title << irf.angles[abin] << "-"<< irf.angles[abin+1] << " degrees";
            }else {
                title <<  irf.angles[0] << "-"<< irf.angles[irf.angle_bins-2] << " degrees";
            }
            m_hists[id]=PointSpreadFunction(IRF::hist_name(abin, ebin, "psf")
                , title.str(), log);
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PsfPlots::~PsfPlots()
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PsfPlots::fill(double angle_diff, double energy, double costheta, bool front)
{
    int z_bin = m_irf.angle_bin( costheta );     if( z_bin>= m_irf.angle_bins) return;
    int e_bin = m_irf.energy_bin(energy);        if( e_bin<0 || e_bin>= m_irf.energy_bins )return;

    int id =  m_irf.ident(e_bin, z_bin);
    double scaled_delta =angle_diff/PointSpreadFunction::scaleFactor(energy, costheta, front);

    m_hists[id].fill(scaled_delta);

    // set special combined hist, accumulate all but last bins of angles
    if( z_bin< m_irf.angle_bins-2) {
        m_hists[m_irf.ident(e_bin, m_irf.angle_bins)].fill(scaled_delta);
    }

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PsfPlots::summarize()
{
    // summarize psf plot contents, fit 
    out() << 
        "\n                 Title             count      68%       95%       chi2" 
        << "     sigma     gcore      gtail"
        << std::endl;
    //          32 MeV, 0-37 degrees       675     0.643     0.538"
    for( PSFlist::iterator it = m_hists.begin(); it!=m_hists.end(); ++it){
        (*it).summarize();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PsfPlots::fit()
{

    for( PSFlist::iterator it = m_hists.begin(); it!=m_hists.end(); ++it){
        (*it).fit();
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PsfPlots::draw(const std::string &ps_filename ) {

    gStyle->SetOptFit(111);

    TCanvas c;

    for( int abin=0; abin<= m_irf.angle_bins; ++abin){
        int rows=2;
        m_irf.divideCanvas(c,m_irf.energy_bins/rows,rows, 
            std::string("Plots from ") +m_irf.summary_filename());
        for(int ebin=0; ebin<m_irf.energy_bins; ++ebin){
            c.cd(ebin+1);
            gPad->SetRightMargin(0.02);
            gPad->SetTopMargin(0.03);
            m_hists[m_irf.ident(ebin,abin)].draw(ymin, ymax, ylog);
        }
        std::cout << "Printing page #" << (abin+1) << std::endl; 
        if( abin==0) c.Print( (ps_filename+"(").c_str());
        else if (abin<m_irf.angle_bins) c.Print(ps_filename.c_str());
        else c.Print( (ps_filename+")").c_str());
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PsfPlots::fillParameterTables()
{
    // make a set of 2-d histograms with values of the fit parameters
    // binning according to energy and costheta bins 
    // (skip norm, the first one)

    for( int i = 1; i< PointSpreadFunction::npars(); ++i){
        std::string name(PointSpreadFunction::parname(i));
        TH2F* h2 = new TH2F(name.c_str(), (name+";log energy; costheta").c_str() 
        ,IRF::energy_bins, IRF::logemin, IRF::logemin+IRF::energy_bins*IRF::logedelta
        ,IRF::angle_bins,  0.3, 1.0
        );
        std::vector<double> pars;

        int index(0);
        for( PSFlist::iterator it = m_hists.begin(); it!=m_hists.end(); ++it, ++index){
            it->getFitPars(pars);
            double costheta = 0.95 - 0.1*(index/IRF::angle_bins);
            double logenergy = log10(IRF::eCenter(index));

            h2->Fill(logenergy, costheta, pars[i]);
        }
        h2->GetXaxis()->CenterTitle();
        h2->GetYaxis()->CenterTitle();
        h2->Write();

    }

}



