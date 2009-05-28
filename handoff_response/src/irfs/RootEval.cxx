/** @file RootEval.cxx
@brief implement class RootEval

$Header$
*/

#include "RootEval.h"
// get definitions from the generation guys -- @todo move up
#include "../gen/PointSpreadFunction.h"
#include "../gen/Dispersion.h"

#include "Bilinear.h"

#include "TFile.h"
#include "TH2F.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace handoff_response;
#include "TPaletteAxis.h"
namespace {
     TPaletteAxis dummy;
}

class RootEval::Table{
public:
    Table(TH2F* hist)
    : m_hist(hist)
    , m_interpolator(0)
    {
        binArray( 0, 10, hist->GetXaxis(), m_energy_axis);
        binArray(-1.0,1.00, hist->GetYaxis(), m_angle_axis);
#if 0
        std::cout << "energy bins: ";
        std::copy(m_energy_axis.begin(), m_energy_axis.end(), std::ostream_iterator<double>(std::cout, "\t"));
        std::cout << std::endl;

        std::cout << "angle bins: ";
        std::copy(m_angle_axis.begin(), m_angle_axis.end(), std::ostream_iterator<double>(std::cout, "\t"));
        std::cout << std::endl;
#endif
        for(Bilinear::const_iterator iy = m_angle_axis.begin(); iy!=m_angle_axis.end(); ++iy){
            float costh ( *iy );
            if(costh==1.0) costh=0.999; // avoid edge in histgram
            for(Bilinear::const_iterator ix = m_energy_axis.begin(); ix!= m_energy_axis.end(); ++ix){
                float loge ( *ix );
                int bin ( hist->FindBin(loge,costh) );
                double value ( static_cast<float>(hist->GetBinContent(bin)));
                m_data_array.push_back(value);
            }
        }

        m_interpolator = new Bilinear(m_energy_axis, m_angle_axis, m_data_array);

    }

    ~Table(){ delete m_interpolator; }
    double value(double logenergy, double costh);
    
    double maximum() { return m_hist->GetMaximum(); }
private:
    /// Fill vector array with the bin edges in a ROOT TAxis, with extra ones for the overflow bins
    void binArray(double low_limit, double high_limit, TAxis* axis, std::vector<float>& array)
    {
        array.push_back(low_limit);
        int nbins(axis->GetNbins());
        for(int i = 1; i<nbins+1; ++i){
            array.push_back(axis->GetBinCenter(i));
        }
        array.push_back(high_limit);
        
    }
    TH2F* m_hist;
    std::vector<float> m_energy_axis, m_angle_axis, m_data_array;
    Bilinear* m_interpolator;

};
double RootEval::Table::value(double logenergy, double costh)
{
#if 0  // non-interpolating for tests
    int bin= m_hist->FindBin(logenergy, costh);
    return m_hist->GetBinContent(bin);
#else
    return (*m_interpolator)(logenergy, costh);
#endif
}


RootEval::RootEval(TFile* f, std::string eventtype)
: IrfEval(eventtype)
, m_f(f)
{
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

double RootEval::psf_integral(double delta, double energy, double theta, double /*phi*/)
{
    double costh(cos(theta*M_PI/180));
    double * par ( psf_par(energy,costh) );
        
    return PointSpreadFunction::integral(&delta, par)*(2.*M_PI * par[1] * par[1]);           
}

double RootEval::dispersion(double emeas, double energy, double theta, 
                            double /*phi*/)
{
    double costh(cos(theta*M_PI/180)), x(emeas/energy-1);
    if( x<-0.9 ) return 0;
    double ret = Dispersion::function(&x, disp_par(energy,costh));
    return ret/energy;
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
    static double theta_max(90); // how to set this? Too high.
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
    par[1] = m_ltail->value(loge,costh);
    par[2] = m_rwidth->value(loge,costh);
    return par;
}


void RootEval::createMap(std::string filename, std::map<std::string,handoff_response::IrfEval*>& evals)
{
    TFile* file= new TFile(filename.c_str(), "readonly");
    if( !file->IsOpen() ) { throw std::invalid_argument("Could not load the file "+filename);}
    TList * keys = file->GetListOfKeys();
    for( int i = 0; i< keys->GetEntries(); ++i){
        std::string eventclass ( keys->At(i)->GetName() );

        evals[eventclass+"/front"]=new RootEval(file, eventclass+"/front");
        evals[eventclass+"/back"]=new RootEval(file, eventclass+"/back");
    }
}

