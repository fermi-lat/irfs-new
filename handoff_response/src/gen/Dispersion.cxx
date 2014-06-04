/** @file Dispersion.cxx
@brief implementation of class Dispersion

$Header$

*/

#include "Dispersion.h"
#include "TH1F.h"
#include "TPad.h"
#include "TPaveStats.h"
#include "TList.h"
#include "TF2.h"

#include <cmath>
#include <iomanip>
#include <stdexcept>

namespace {
    // histogram parameters
    static double xmin=-7.5, xmax=7.5; 
    static int nbins=100;

  static const char* names[]={"norm","ls1", "rs1", "bias", "ls2",  "rs2"};
  static double pinit[] ={0.1,   0.5,   0.5,      0.0,    0.1,       0.1};
  static double pmin[]  ={0.,    0.05,   0.05,   -0.5,    1e-5,   1e-5};
  static double pmax[]  ={1e6,   9,     9,      0.5,   10,      10};
  static double pindex[]={1.6,0.6};
  static double psplit(1.5); //tuned for a better fit
  static double fitrange[]={-7, 7};
  static int min_entries(10);
  static int fit_tries=3; // try a fit this many times before giving up

  double edisp_func(double * x, double * par)
  {
    
    double t=fabs(x[0]-par[3]);
    double s1(par[1]);
    double s2(par[4]);
    
    //left or right sigma's are required?
    if (x[0]>par[3]) {
      //right side
      s1=par[2];
      s2=par[5];
    }
   
    //for core
    double g1=exp(-0.5*pow(t/s1,pindex[0]));
    //for tails
    double g2=exp(-0.5*pow(t/s2,pindex[1]));
    
    //scale norm
    double nscale=exp( 0.5*(pow(psplit/s2,pindex[1])-
			    pow(psplit/s1,pindex[0])));
    if (t>psplit)
      return par[0]*nscale*g2;
    //else
    return par[0]*g1;
  }
  
}// anon namespace

// following numbers determined empirically to roughly 
// make the 68% containment radius be 1.0 independent of energy
double Dispersion::s_coef_thin[6] = {0.0210, 0.058, -0.207,
                                     -0.213, 0.042, 0.564};
double Dispersion::s_coef_thick[6] = {0.0215, 0.0507, -0.22,
                                      -0.243, 0.065, 0.584};

// External versions.


const char* Dispersion::parname(int i){return names[i];}

int Dispersion::npars(){return sizeof(names)/sizeof(void*);}

std::vector<std::string>
      Dispersion::pnames(names, names+npars());

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dispersion::Dispersion(std::string histname, 
                                         std::string title)
                                         
: m_hist( new TH1F(histname.c_str(),  title.c_str(),  nbins, xmin, xmax))
, m_fitfunc(TF1("edisp-fit", edisp_func, fitrange[0], fitrange[1], npars()))
, m_count(0)
{
    hist().GetXaxis()->SetTitle("scaled deviation");

    for (unsigned int i = 0; i < sizeof(pmin)/sizeof(double); i++) {
        m_fitfunc.SetParLimits(i, pmin[i], pmax[i]);
        m_fitfunc.SetParName(i, pnames[i].c_str());
    }
    m_fitfunc.SetLineWidth(1);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dispersion::~Dispersion()
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Dispersion::fill(double scaled_delta, double weight)
{
    hist().Fill( scaled_delta, weight );
    m_count++;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Dispersion::summary_title(std::ostream & out)
{
  out << "\n\t\t Dispersion fit summary\n"
      <<"Nothing here yet! (Dispersion.cxx)" << std::endl;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Dispersion::summarize(std::ostream & out)
{

  out<<"Nothing here yet! (Dispersion.cxx)" << std::endl;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double Dispersion::scaleFactor(double energy,double  zdir, bool thin)
{
  // following numbers determined empirically to roughly 
  // make the 68% containment radius be 1.0 independent of energy
  //use a TFunction so that all this stuff could be read from fits file

   
  // static double coef_thin[] ={0.0210,0.058,-0.207,-0.213,0.042,0.564};
  // static double coef_thick[]={0.0215,0.0507,-0.22,-0.243,0.065,0.584};
  
  //x is McLogEnergy, y is fabs(McZDir)
  static const char funcdef[]="[0]*x*x+[1]*y*y + [2]*x + [3]*y + [4]*x*y + [5]";

  static TF2 edisp_scale_func("edisp_scale_func",funcdef);

  double vars[2]; vars[0]=::log10(energy); vars[1]=::fabs(zdir);

  if( thin ){
    return edisp_scale_func(vars,s_coef_thin); 
  }else{
    return edisp_scale_func(vars,s_coef_thick); 
  }
}

void Dispersion::
setScaleFactorParameters(const std::vector<double> & edisp_front,
                         const std::vector<double> & edisp_back) {
   if (edisp_front.size() != 6 || edisp_back.size() !=6) {
      throw std::runtime_error("Dispersion::setScaleFactorParameters:\n"
                               "each of edisp_[front,back] must have "
                               "exactly 6 values.");
   }
   for (size_t i(0); i < edisp_front.size(); i++) {
      s_coef_thin[i] = edisp_front[i];
      s_coef_thick[i] = edisp_back[i];
   }
}

void Dispersion::
getScaleFactorParameters(std::vector<double> & edisp_front,
                         std::vector<double> & edisp_back) {
   edisp_front.clear();
   edisp_back.clear();
   for (size_t i(0); i < 6; i++) {
      edisp_front.push_back(s_coef_thin[i]);
      edisp_back.push_back(s_coef_thick[i]);
   }
}

double Dispersion::function(double* delta, double* par) {
  return edisp_func(delta,par);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Dispersion::fit(std::string opts)
{
  
    std::cout << "\rProcessing " << hist().GetTitle();
    TH1F & h = hist(); 

    // normalize the distribution
    double scale = h.Integral();
    if (scale > 0 && hist().GetEntries()>50) { 
        h.Sumw2(); // needed to preserve errors
        h.Scale(1./scale);
    }

    m_fitfunc.SetParameters(pinit);
    if( m_count > min_entries ) {
      int fitcount=0;
      int fitres=-1;
      // fit till convergence or for a set number of times
      // Minuit at times gives up for no reason
      while ((fitres!=0)&&(fitcount<fit_tries)) {
	fitcount++;
        fitres=h.Fit(&m_fitfunc,opts.c_str()); // fit only specified range
      }
    }
}

void Dispersion::setFitPars(double * pars, double * pmin,
                                     double * pmax) {
   for (int i = 0; i < npars(); i++) {
      m_fitfunc.SetParLimits(i, pmin[i], pmax[i]);
   }
   m_fitfunc.SetParameters(pars);
}
 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Dispersion::draw(double ymin, double ymax, bool ylog)
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

    h.Write();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Dispersion::getFitPars(std::vector<double>& params)const
{
    params.resize(m_fitfunc.GetNpar());
    // weird, but it seems to work 
    const_cast<TF1*>(&m_fitfunc)->GetParameters(&params[0]);
}
