/** @file FisheyeHist.cxx
@brief implementation of class FisheyeHist

$Header$

*/

#include "FisheyeHist.h"

#include "TH1F.h"
#include "TPad.h"
#include "TPaveStats.h"
#include "TList.h"

#include <cmath>
#include <iomanip>
#include <stdexcept>

namespace {
    // histogram parameters
    static double xmin=-5.0, xmax=5.0; 
    static int nbins=200;

    // specify fit function
  static const char* names[] = {"fisheye_mean","fisheye_median","fisheye_peak"};

    static double fitrange[] = {xmin, xmax};
//    static double ub = 10.;  // revisit this choice of ub
    static int min_entries=10; // minimum number of entries to fit

    inline double sqr(double x){return x*x;}

    void swap(double & x, double & y) {
       double tmp(x);
       x = y;
       y = tmp;
    }

  TH1F* cumulative_hist(TH1F& h)
    {
        // make a cumulative histogram 
        float y=h.GetBinContent(0);
        TH1F* hcum = (TH1F*)h.Clone(); hcum->Clear();
        for(int i=1; i< h.GetNbinsX(); ++i){
            float x = h.GetBinCenter(i), 
                dy=h.GetBinContent(i);
            hcum->Fill(x,y+dy/2.);
            y = y+dy;
        }
        hcum->Scale(1/y);
        return hcum;
    }

}// anon namespace

const char* FisheyeHist::parname(int i){return names[i];}

int FisheyeHist::npars(){return sizeof(names)/sizeof(void*);}

std::vector<std::string> FisheyeHist::pnames(names, names+npars());

FisheyeHist::FisheyeHist(std::string histname, 
			 std::string title)
                                         
   : m_hist(new TH1F(histname.c_str(),  title.c_str(),  nbins, xmin, xmax)),
     m_count(0), m_mean(), m_median(), m_peak(), m_mean_err() 
{ 
  hist().GetXaxis()->SetTitle("scaled deviation");
  m_fitfunc.SetLineWidth(0.5);
}

FisheyeHist::FisheyeHist()
   : m_count(-1), m_mean(), m_median(), m_peak(), m_mean_err()  {
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FisheyeHist::~FisheyeHist()
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FisheyeHist::fill(double scaled_delta, double weight)
{
    hist().Fill( scaled_delta, weight );
    m_count++;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FisheyeHist::summary_title(std::ostream & out)
{
    out << 
        "\n                 Title             count     68%       95%       chi2 " ;
    for( std::vector<std::string>::const_iterator it = pnames.begin(); it!=pnames.end(); ++it){
        out << std::setw(10) << (*it) ;
    }
    out << std::endl;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FisheyeHist::summarize(std::ostream & out)
{

    out << std::setw(30)<<  hist().GetTitle()  
        << std::setw(10) <<  m_count;
    if( m_count>0 ) { 
        static double probSum[2]={0.68, 0.95}; // for defining quantiles
        double quant[2];
        hist().GetQuantiles(2,quant, probSum);
        out <<std::left << "     ";
        out << std::fixed; // for trailing zeros?
        out << std::setw(10) <<  std::setprecision(2) << pow(10.,quant[0])
            << std::setw(10) <<  std::setprecision(2) << pow(10.,quant[1]) ;
        std::vector<double> params;
        getFitPars(params);
        if( params[0]>0) { 
            out << std::setw(10) <<  std::setprecision(1) << m_fitfunc.GetChisquare();

            for( int j=0; j< npars(); ++j){
                // fit was done -- summarize the parameters
                out << std::setw(10) <<  std::setprecision(3) << params[j];
            }

        }else{
            out << std::setw(8) << "--" << std::setw(10) << "--";
        }

        out << std::right;
    }else{
        out << std::setw(9) << "--" << std::setw(10) << "--" 
            << std::setw(10) << "--" << std::setw(10) << "--";
    }
    out << std::endl;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FisheyeHist::fit(std::string opts)
{

  // create the cumulative histgram before changing this one
  //  m_cumhist = cumulative_hist(hist());

  std::cout << "\rProcessing " << hist().GetTitle();
  TH1F & h = hist(); 
  int nbins = h.GetNbinsX();

  // normalize the distribution
  double scale = h.Integral();
  if (scale > 0) { 
    h.Sumw2(); // needed to preserve errors
    h.Scale(1./scale);
  }

  for( int k = 1; k<=nbins; ++k){
    double y = h.GetBinContent(k), 
      dy = h.GetBinError(k),
      dx = h.GetBinWidth(k);
    
    h.SetBinContent(k, y/dx); 
    h.SetBinError(k, dy/dx);
  }

  if(m_count < 100)
    {
      m_mean_err = 10.;
      return;
    }

  static double probSum[1]={0.5}; // for defining quantiles
  double quant[1];

  hist().GetQuantiles(1,quant, probSum);
  m_median = quant[0];
  m_mean = h.GetMean();
  m_mean_err = h.GetRMS()/sqrt(m_count);

  TF1 * f1 = new TF1("f1","gaus");
  f1->SetParameter(0,1.0);
  f1->SetParameter(1,m_median);
  f1->SetParameter(2,1.0);

  // std::cout << "-----------------" << std::endl;

  // std::cout << m_count << std::endl;

  // std::cout << f1->GetParameter(0) << " "
  //  	    << f1->GetParameter(1) << " "
  //  	    << f1->GetParameter(2) << std::endl;

  h.Fit(f1,"QL","",m_median-1.0,m_median+1.0);
  m_peak = f1->GetParameter(1);

  // std::cout << f1->GetParameter(0) << " "
  //  	    << f1->GetParameter(1) << " "
  //  	    << f1->GetParameter(2) << std::endl;
  
  // std::cout << "MEDIAN " << m_median << std::endl;
  // std::cout << "MEAN " << m_mean << std::endl;
  // std::cout << "PEAK " << m_peak << std::endl;
}
 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FisheyeHist::draw(double ymin, double ymax, bool ylog)
{
//
    // set up appearance, and draw to current pad
    if( ylog) gPad->SetLogy();
    TH1F & h = hist(); 
    h.SetMaximum(ymax);
    h.SetMinimum(ymin);
    h.SetStats(true);
    h.SetLineColor(kBlack);
    h.GetXaxis()->CenterTitle(true);

    /// @todo: do I need this? why is it here
    TList * list = h.GetListOfFunctions();
    TPaveStats  *s = (TPaveStats*)list->FindObject("stats");
    if( s!= 0 ){
        s->SetY1NDC(0.6);
    }
    h.Draw();
#if 0
    // overlay the cumulative histogram
    m_cumhist->Draw("same");

    // finally overlay with psf integral
    TF1 psf_int("psf_integral", psf_integral, -1, 1, 3);
    psf_int.SetParameters(m_pars);
#endif
    h.Write();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FisheyeHist::getFitPars(std::vector<double>& params)const
{
  params.resize(3);
  params[0] = m_mean;
  params[1] = m_median;
  params[2] = m_peak;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FisheyeHist::getFitErrs(std::vector<double>& params)const
{
  params.resize(3);
  params[0] = m_mean_err;
  params[1] = m_mean_err;
  params[2] = m_mean_err;
}
