/** @file RootEval.cxx
@brief implement class RootEval

$Header$
*/

#include "RootEval.h"
// get definitions from the generation guys -- @todo move up
#include "../gen/PointSpreadFunction.h"
#include "../gen/Dispersion.h"

#include "TFile.h"
#include "TH2F.h"
#include "TGraph2D.h" 

#include <sstream>
#include <stdexcept>
#include <string>
#include <cmath>
#include <iomanip>
#include <iostream>

using namespace handoff_response;
#include "TPaletteAxis.h"
namespace {
     TPaletteAxis dummy;
}

class RootEval::Table{
public:
    Table(TH2F* hist)
    : m_hist(hist)
    , m_graph(new TGraph2D(hist))
    {
      //  std::cout << "Loading " << hist->GetTitle() << std::endl;
        double check = value(3.0, 0.85);
        double check2 = value(3.0, 0.85);// second fails?
        if (check != check2) {
           throw std::runtime_error("RootEval::Table: value method error");
        }
    }

    double value(double logenergy, double costh);
    
    double maximum() { return m_hist->GetMaximum(); }
private:
    TH2F* m_hist;
    TGraph2D* m_graph;
};
double RootEval::Table::value(double logenergy, double costh)
{
#if 1
    int bin= m_hist->FindBin(logenergy, costh);
    return m_hist->GetBinContent(bin);
#else
    if( costh> 0.95) costh=0.95;
    double ret = m_graph->Interpolate(logenergy,costh);
    //std::cout << "interpolate: " << logenergy<<", "<< costh << "-->"<< ret << std::endl;
    return ret;
#endif
}


RootEval::RootEval(std::string filename, std::string eventtype)
: IrfEval(eventtype)
, m_f(new TFile(filename.c_str(), "READONLY"))
{
    if( m_f==0) throw std::invalid_argument("RootEval: could not open ROOT file "+filename);

    m_aeff  = setupHist("aeff");
    m_sigma = setupHist("sigma");
    m_gcore = setupHist("gcore");
    m_gtail = setupHist("gtail");
    m_dnorm = setupHist("dnorm");
    m_rwidth= setupHist("rwidth");
    m_ltail = setupHist("ltail");
}
RootEval::~RootEval(){ delete m_f;}


double RootEval::aeff(double energy, double theta, double /*phi*/)
{
    static double factor(1e4); // from m^2 to cm&2
    double costh(cos(theta*M_PI/180));
    if( costh==1.0) costh = 0.9999; // avoid edge of bin
    return factor*m_aeff->value(log10(energy), costh);
}

double RootEval::aeffmax()
{
    return m_aeff->maximum();
}

double RootEval::psf(double delta, double energy, double theta, double /*phi*/)
{
    double costh(cos(theta*M_PI/180));
    return PointSpreadFunction::function(&delta, psf_par(energy, costh));           
}

double RootEval::dispersion(double emeas, double energy, double theta, 
                            double /*phi*/)
{
    double costh(cos(theta*M_PI/180));
    return Dispersion::function(&emeas, disp_par(energy,costh));
}

RootEval::Table* RootEval::setupHist( std::string name)
{
    std::string fullname(eventClass()+"/"+name);
    TH2F* h2 = (TH2F*)m_f->GetObjectChecked((fullname).c_str(), "TH2F");
    if( h2==0) throw std::invalid_argument("RootEvalr: could not find plot "+fullname);
    return new Table(h2);
}
double * RootEval::psf_par(double energy, double costh)
{
    static double par[4];
    static double zdir(1.0); // not used
    double loge(::log10(energy));
    if( costh==1.0) costh = 0.9999;
    par[1] = m_sigma->value(loge,costh) * PointSpreadFunction::scaleFactor(energy, zdir, isFront());
    par[2] = m_gcore->value(loge,costh);
    par[3] = m_gtail->value(loge,costh);
    if (par[1] == 0 || par[2] == 0) {
       std::ostringstream message;
       message << "handoff_response::RootEval: psf parameters are zero in " 
               << "when computing solid angle normalization:\n"
               << "\tenergy = " << energy << "\n"
               << "\tcosth  = " << zdir   << "\n"
               << "\tpar[1] = " << par[1] << "\n"
               << "\tpar[2] = " << par[2] << std::endl;
       std::cerr << message.str() << std::endl;
       throw std::runtime_error(message.str());
    }
    if( par[3]==0) par[3]=par[2];
    // manage normalization by replacing normalization parameter for current set of parameters
    par[0]=1;
    static double theta_max(50); // how to set this? Too high.
    double norm = PointSpreadFunction::integral(&theta_max,par);
    par[0] = 1./norm/(2.*M_PI * par[1] * par[1]); // solid angle normalization 
    return par;
}

double * RootEval::disp_par(double energy, double costh)
{
    static double par[3];
    double loge(::log10(energy));
    if( costh==1.0) costh = 0.9999;
    ///@todo: check limits, flag invalid if beyond.
    par[0] = m_dnorm->value(loge,costh);
    par[1] = m_rwidth->value(loge,costh);
    par[2] = m_ltail->value(loge,costh);
    return par;
}


