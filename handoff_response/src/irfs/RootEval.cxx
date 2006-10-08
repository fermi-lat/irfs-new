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

#include <stdexcept>
#include <string>
#include <cmath>
#include <iomanip>
#include <iostream>

using namespace handoff_response;

RootEval::RootEval(std::string filename, std::string eventtype)
: IrfEval(eventtype)
, m_f(new TFile(filename.c_str(), "READONLY"))
{
    if( m_f==0) throw std::invalid_argument("RootEval: could not open ROOT file "+filename);

    m_aeff=setupHist( "aeff");
    //       std::cout << "Aeff: " << m_aeff->GetEntries() << " found entries" << std::endl;
    m_sigma = setupHist("sigma");
    m_gcore = setupHist("gcore");
    m_gtail = setupHist("gtail");
    m_dnorm = setupHist("dnorm");
    m_rwidth= setupHist("rwidth");
    m_ltail = setupHist("ltail");
}
RootEval::~RootEval(){ delete m_f;}


double RootEval::aeff(double energy, double theta, double phi)
{
    static double factor(1e4); // from m^2 to cm&2
    double costh(cos(theta*M_PI/180));
    if( costh==1.0) costh = 0.9999; // avoid edge of bin
    int bin= m_aeff->FindBin(log10(energy), costh);
    return factor*m_aeff->GetBinContent(bin);
}

double RootEval::aeffmax()
{
    return m_aeff->GetMaximum();
}

double RootEval::psf(double delta, double energy, double theta, double phi)
{
    double costh(cos(theta*M_PI/180));
    return PointSpreadFunction::function(&delta, psf_par(energy, costh));           
}

double RootEval::dispersion(double emeas, double energy, double theta, double phi)
{
    double costh(cos(theta*M_PI/180));
    return Dispersion::function(&emeas, disp_par(energy,costh));
}

TH2F* RootEval::setupHist( std::string name)
{
    std::string fullname(eventClass()+"/"+name);
    TH2F* h2 = (TH2F*)m_f->GetObjectChecked((fullname).c_str(), "TH2F");
    if( h2==0) throw std::invalid_argument("RootEvalr: could not find plot "+fullname);
    return h2;
}
double * RootEval::psf_par(double energy, double costh)
{
    static double par[4];
    static double zdir(1.0); // not used
    double loge(::log10(energy));
    if( costh==1.0) costh = 0.9999;
    int bin = m_sigma->FindBin(loge, costh);
    par[1] = m_sigma->GetBinContent(bin) * PointSpreadFunction::scaleFactor(energy, zdir, isFront());
    par[2] = m_gcore->GetBinContent(bin);
    par[3] = m_gtail->GetBinContent(bin);
    par[0] = 1.0/(2.*M_PI * par[1] * par[1]); // solid angle normalization (not using fit)
    if( par[3]==0) par[2]=par[1];
    return par;
}

double * RootEval::disp_par(double energy, double costh)
{
    static double par[3];
    double loge(::log10(energy));
    if( costh==1.0) costh = 0.9999;
    ///@todo: check limits, flag invalid if beyond.
    int bin = m_rwidth->FindBin(loge, costh);
    par[0] = m_dnorm->GetBinContent(bin);
    par[1] = m_rwidth->GetBinContent(bin);
    par[2] = m_ltail->GetBinContent(bin);
    return par;
}
