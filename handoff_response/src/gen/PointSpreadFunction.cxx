/** @file PointSpreadFunction.cxx
@brief implementation of class PointSpreadFunction

$Header$

*/

#include "PointSpreadFunction.h"

#include "TH1F.h"
#include "TPad.h"
#include "TPaveStats.h"
#include "TList.h"

#include <cmath>
#include <iomanip>

namespace {
    // histogram parameters
    static double xmin=-1.0, xmax=1.5; 
    static int nbins=75;

    // specify fit function
    static const char* names[] = {"ncore", "sigma", "gcore","gtail"};
    static double pinit[] = {1,    0.5,  2.5, 1.5};
    static double pmin[] =  {0.01, 0.15, 1.0, 1.0};
    static double pmax[] =  {10.,  2.0,  5.0, 5.0};

    static double fitrange[] = {xmin, xmax};
    static double ub = 10.;  // revisit this choice of ub
    static int min_entries=10; // minimum number of entries to fit

    inline double sqr(double x){return x*x;}

    void swap(double & x, double & y) {
       double tmp(x);
       x = y;
       y = tmp;
    }

    double psf_base(double u, double /*sigma*/, double gamma)
    {
        return (1. - 1./gamma)*pow(1. + u/gamma, -gamma);
    }

    double psf_base_integral(double u, double /*sigma*/, double gamma) {
//        return u > 1e3 ? 1. : 1. - pow(1. + u/gamma, 1.-gamma);
       return 1. - pow(1. + u/gamma, 1. - gamma);
    }

    /// @param logx Pointer to log10 of scaled angular deviation.
    double psf_with_tail(double * logx, double * p)
    {
       double ncore(p[0]);
       double sigma(p[1]);
       double gcore(p[2]);
       double gtail(p[3]);
       double ntail = ncore*(psf_base(ub, sigma, gcore)
                             /psf_base(ub, sigma, gtail));
       double r = pow(10., (*logx))/sigma;
       double u = r*r/2.;
       return (ncore*psf_base(u, sigma, gcore) +
               ntail*psf_base(u, sigma, gtail));
    }

    double psf_integral(double * logx, double * p)
    {
       double ncore(p[0]);
       double sigma(p[1]);
       double gcore(p[2]);
       double gtail(p[3]);
       double ntail = ncore*(psf_base(ub, sigma, gcore)
                             /psf_base(ub, sigma, gtail));
       double r = pow(10., (*logx))/sigma;
       double u = r*r/2.;
       return (ncore*psf_base_integral(u, sigma, gcore) + 
               ntail*psf_base_integral(u, sigma, gtail));
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

// External versions.

/// @param *x Pointer to the angular deviation in radians.
/// @param *p Pointer to the PSF fit parameters.
double PointSpreadFunction::function(double * x, double * p)
{
   double ncore(p[0]);
   double sigma(p[1]);
   double gcore(p[2]);
   double gtail(p[3]);
   double ntail = ncore*(psf_base(ub, sigma, gcore)
                         /psf_base(ub, sigma, gtail));
   double r = *x/sigma;
   double u = r*r/2.;
   return (ncore*psf_base(u, sigma, gcore) +
           ntail*psf_base(u, sigma, gtail));
}

double PointSpreadFunction::integral(double * x, double * p)
{
   double ncore(p[0]);
   double sigma(p[1]);
   double gcore(p[2]);
   double gtail(p[3]);
   double ntail = ncore*(psf_base(ub, sigma, gcore)
                         /psf_base(ub, sigma, gtail));
   double r = *x/sigma;
   double u = r*r/2.;
   return (ncore*psf_base_integral(u, sigma, gcore) + 
           ntail*psf_base_integral(u, sigma, gtail));
}

const char* PointSpreadFunction::parname(int i){return names[i];}

int PointSpreadFunction::npars(){return sizeof(names)/sizeof(void*);}

std::vector<std::string>
      PointSpreadFunction::pnames(names, names+npars());

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PointSpreadFunction::PointSpreadFunction(std::string histname, 
                                         std::string title)
                                         
: m_hist( new TH1F(histname.c_str(),  title.c_str(),  nbins, xmin, xmax))
, m_fitfunc(TF1("psf-fit", psf_with_tail, fitrange[0], fitrange[1], npars()))
, m_count(0)
{
    hist().GetXaxis()->SetTitle("log10(scaled deviation)");

    for (unsigned int i = 0; i < sizeof(pmin)/sizeof(double); i++) {
        m_fitfunc.SetParLimits(i, pmin[i], pmax[i]);
        m_fitfunc.SetParName(i, pnames[i].c_str());
    }

    m_fitfunc.SetLineWidth(1);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PointSpreadFunction::~PointSpreadFunction()
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PointSpreadFunction::fill(double scaled_delta, double weight)
{
    hist().Fill( log10(scaled_delta), weight );
    m_count++;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PointSpreadFunction::summary_title(std::ostream & out)
{
    out << 
        "\n                 Title             count     68%       95%       chi2 " ;
    for( std::vector<std::string>::const_iterator it = pnames.begin(); it!=pnames.end(); ++it){
        out << std::setw(10) << (*it) ;
    }
    out << std::endl;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PointSpreadFunction::summarize(std::ostream & out)
{

    out << std::setw(30)<<  hist().GetTitle()  
        << std::setw(10) <<  m_count;
    if( m_count>0 ) { 
        static double probSum[2]={0.68, 0.95}; // for defining quantiles
        double quant[2];
        hist().GetQuantiles(2,quant, probSum);
        out <<std::left << "     ";
        out << std::fixed; // for trailing zeros?
        out << std::setw(10) <<  std::setprecision(2) << pow(10.,m_quant[0])
            << std::setw(10) <<  std::setprecision(2) << pow(10.,m_quant[1]) ;
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
double PointSpreadFunction::scaleFactor(double energy,double /* zdir*/, bool thin)
{
    // following numbers determined empirically to roughly 
    // make the 68% containment radius be 1.0 independent of energy
    static double 
        coef_thin[] ={58e-3,  377e-6},
        coef_thick[]={96e-3, 1300e-6},
        power = -0.80;

    double t = pow( energy/100., power);
    if( thin ){
        return sqrt( sqr(coef_thin[0]*t) + sqr( coef_thin[1]) ); 
    }else{
        return sqrt( sqr(coef_thick[0]*t) + sqr( coef_thick[1]) ); 
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PointSpreadFunction::fit(std::string opts)
{
    // create the cumulative histgram before changing this one
    m_cumhist = cumulative_hist(hist());

    std::cout << "\rProcessing " << hist().GetTitle();
    TH1F & h = hist(); 

    // now add overflow to last bin
    int nbins = h.GetNbinsX();
    h.SetBinContent(nbins, h.GetBinContent(nbins) +h.GetBinContent(nbins+1));

    // determine positions of 68, 95%
    static double probSum[2]={0.68, 0.95}; // for defining quantiles
    hist().GetQuantiles(2,m_quant, probSum);

    // record fraction in tail beyond the fit range.
    int bin1=hist().FindBin(fitrange[1]);
    m_tail = h.Integral(bin1, nbins); 
    m_tail /= m_count;
    // normalize the distribution
    double scale = h.Integral();
    if (scale > 0 && hist().GetEntries()>50) { 
        h.Sumw2(); // needed to preserve errors
        h.Scale(1./scale);
    }


    // adjust values by bin center, to get density
    for( int k = 1; k<=nbins; ++k){
        double 
            y = h.GetBinContent(k), 
            dy = h.GetBinError(k),
            x = h.GetBinCenter(k),
            jacobian = pow(10.,2*x) ;// for log10 binning.
        if (dy == 0) continue; //???
        h.SetBinContent(k, y/jacobian); 
        h.SetBinError(k, dy/jacobian);
    }
    m_fitfunc.SetParameters(pinit);
    if( m_count > min_entries ) {
        h.Fit(&m_fitfunc,opts.c_str()); // fit only specified range
    }
}

void PointSpreadFunction::setFitPars(double * pars, double * pmin,
                                     double * pmax) {
   for (int i = 0; i < npars(); i++) {
      m_fitfunc.SetParLimits(i, pmin[i], pmax[i]);
   }
   m_fitfunc.SetParameters(pars);
}
 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PointSpreadFunction::draw(double ymin, double ymax, bool ylog)
{
//
    // set up appearance, and draw to current pad
    if( ylog) gPad->SetLogy();
    TH1F & h = hist(); 
    h.SetMaximum(ymax);
    h.SetMinimum(ymin);
    h.SetStats(true);
    h.SetLineColor(kRed);
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
void PointSpreadFunction::getFitPars(std::vector<double>& params)const
{
    params.resize(m_fitfunc.GetNpar());
    // weird, but it seems to work 
    const_cast<TF1*>(&m_fitfunc)->GetParameters(&params[0]);
}
