/** @file PointSpreadFunction.cxx

$Header$

*/

#include "PointSpreadFunction.h"

#include "TH1F.h"
#include "TPad.h"
#include "TPaveStats.h"

#include <cmath>
#include <iomanip>


namespace {
    // histogram prameters
    static double xmin=-1.0, xmax=1.0; 
    static int nbins=50;

    // specify fit function
#if 1
    static const char* pnames[]={"pnorm", "sigma", "gcore","gtail"};
#else
    static const char* pnames[]={"pnorm", "sigma", "gamma"};
#endif
    static double pinit[]={1,     0.3,  2.5,  2.0};
    static double pmin[]= {0.01,  0.15, 1.0,  1.5};
    static double pmax[]= {50.,   2.0,  5.0,  5.0};

    static double fitrange[] = {-1.0, 2.0};
    static double u_break (10.0); 

    inline double sqr(double x){return x*x;}

    double psf_base(double u, double sigma, double gamma)
    {
        return (1-1./gamma) * pow(1+u/gamma, -gamma);
    }
    double psf_base_integral(double u,double sigma, double gamma){
        return 1- pow(1+u/gamma, 1-gamma);
    }
    double psf_function( double * x, double * p)
    {
        double sigma = p[1], gamma = p[2];
        double qsq = ::pow(10., 2* (*x))/2/sqr(sigma);
        return p[0] * (1-1/gamma) * pow( 1.+qsq/gamma ,-gamma);
    }
    double psf_integral(double* x, double* par)
    {
        double sigma = par[1], gamma = par[2];
        double r = pow(10., (*x))/sigma,  u = r*r/2.;
        if( u< u_break) {
            return par[0]*psf_base_integral(u,sigma,gamma);
        }
        double gtail = par[3];
        return par[0]*
            (
            psf_base_integral(u_break, sigma, gamma)
            + psf_base(u_break,sigma, gamma)/psf_base(u_break,sigma, gtail)
                    *(psf_base_integral(u,      sigma, gtail)
                     -psf_base_integral(u_break,sigma, gtail))
            );
    }
    static int min_entries=10; // minimum number of entries to fit
    double psf_with_tail(double* x, double* p)
    {
        double  sigma = p[1], gamma = p[2];
        double u = ::pow(10., 2* (*x))/2/sqr(sigma);
        //double r = ::pow(10., 2* (*x))/2/sqr(sigma), u=r*r/2.;
#if 0
        return p[0]*psf_base(u, sigma, gamma);
#else
        double  gamma_tail = p[3]; // reduce the exponent for tail
        return u<u_break
            ? p[0] * psf_base(u, sigma, gamma)
            : p[0]  * psf_base(u_break,sigma, gamma) 
                    * psf_base(u,      sigma, gamma_tail) 
                    / psf_base(u_break,sigma, gamma_tail);
#endif
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



// external version
double PointSpreadFunction::function(double* x, double *p)
{
    double  sigma = p[1], gamma = p[2];
    double u = 0.5* sqr((*x)/sigma);
    double  gamma_tail = p[3]; // reduce the exponent for tail
    return u<u_break
        ? p[0] * psf_base(u, sigma, gamma)
        : p[0]  * psf_base(u_break,sigma, gamma) 
        * psf_base(u,      sigma, gamma_tail) 
        / psf_base(u_break,sigma, gamma_tail);
}

const char* PointSpreadFunction::parname(int i){return pnames[i];}
int PointSpreadFunction::npars(){return sizeof(pnames)/sizeof(void*);}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PointSpreadFunction::PointSpreadFunction(std::string histname, 
                                         std::string title,
                                         std::ostream& log)
                                         
: m_hist( new TH1F(histname.c_str(),  title.c_str(),  nbins, xmin, xmax))
//, m_fitfunc(TF1("psf-fit", psf_function, -1.5, 1.5, npars()))
#if 0 // old function
, m_fitfunc(TF1("psf-fit", psf_function, fitrange[0], fitrange[1], npars()))
#else
, m_fitfunc(TF1("psf-fit", psf_with_tail, fitrange[0], fitrange[1], npars()))
#endif
, m_count(0)
, m_log(& log)
{
    hist().GetXaxis()->SetTitle("log10(scaled deviation)");

    for (unsigned int i = 0; i < sizeof(pmin)/sizeof(double); i++) {
        m_fitfunc.SetParLimits(i, pmin[i], pmax[i]);
        m_fitfunc.SetParName(i, pnames[i]);
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
void PointSpreadFunction::summarize()
{

    out() << std::setw(30)<<  hist().GetTitle()  
        << std::setw(10) <<  m_count;
    if( m_count>0 ) { 
        static double probSum[2]={0.68, 0.95}; // for defining quantiles
        double quant[2];
        hist().GetQuantiles(2,quant, probSum);
        out() <<std::left << "     ";
        out() << std::setw(10) <<  std::setprecision(3) << pow(10.,m_quant[0])
            << std::setw(10) <<  std::setprecision(3) << pow(10.,m_quant[1]) ;
        std::vector<double> params;
        getFitPars(params);
        if( params[0]>0) { 
            out() << std::setw(10) <<  std::setprecision(4) << m_fitfunc.GetChisquare();

            for( int j=1; j< npars(); ++j){
                // fit was done -- summarize the parameters
                out() << std::setw(10) <<  std::setprecision(3) << params[j];
            }
#if 0 // not needed with tail managed now?
            // examine the prediction of the fit beyond the fit interval, and compare with actual
            double predtail = 1-psf_integral(&fitrange[1], &params[1]);
            out() << std::setw(10) <<  std::setprecision(3) << predtail*100;
            out() << std::setw(10) <<  std::setprecision(3) << m_tail*100;
#endif

        }else{
            out() << std::setw(8) << "--" << std::setw(10) << "--";
        }

        out() << std::right;
    }else{
        out() << std::setw(9) << "--" << std::setw(10) << "--" 
            << std::setw(10) << "--" << std::setw(10) << "--";
    }
    out() << std::endl;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double PointSpreadFunction::scaleFactor(double energy,double zdir, bool thin)
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
            jacobian=pow(10.,2*x) ;// for log10 binning.
        if( dy==0)continue; //???
        h.SetBinContent(k, y/  jacobian); 
        h.SetBinError(k, dy/ jacobian);
    }
    if( m_count > min_entries ) {
        m_fitfunc.SetParameters(pinit);
        h.Fit(&m_fitfunc,opts.c_str()); // fit only specified range
    }

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
    // wierd, but it seems to work 
    const_cast<TF1*>(&m_fitfunc)->GetParameters(&params[0]);
}
