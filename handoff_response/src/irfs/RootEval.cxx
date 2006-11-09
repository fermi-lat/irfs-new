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
    /** @brief lookup a value from the table
        @param logenergy log10(energy)
        @param costh    cos(theta)
        @param interpolate [true] if true, make linear interpolation. Otherwise take value for given cell


    */
    double value(double logenergy, double costh, bool interpolate=true);
    
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
double RootEval::Table::value(double logenergy, double costh, bool interpolate)
{
    if( interpolate) return (*m_interpolator)(logenergy, costh);

    // non-interpolating: look up value for the bin 

    double maxloge( *(m_energy_axis.end()-2)); // if go beyond this, use last bin
    if( logenergy>= maxloge ) {
        logenergy = maxloge;
    }
    if (logenergy <= m_energy_axis.at(1)) {    // use first bin if necessary
       logenergy = m_energy_axis.at(1);        // why isn't m_energy_axis.at(0)
    }                                          // the first bin?
    int bin= m_hist->FindBin(logenergy, costh);
    return m_hist->GetBinContent(bin);

}


RootEval::RootEval(TFile* f, std::string eventtype)
: IrfEval(eventtype)
, m_f(f)
{
    m_aeff  = setupHist("aeff");
    setupParameterTables(PointSpreadFunction::pnames, m_psfTables);

    double psftest = psf(1000, 1000, 0.);

    setupParameterTables(Dispersion::Hist::pnames, m_dispTables);
#if 0
    std::cout << "Test dispersion at 1000 MeV" << std::endl;
    for( double e(500); e<1500; e*=1.05) {
        std::cout << e << "\t" <<  dispersion(e, 1000, 0.) << std::endl;
    }
#endif
}
RootEval::~RootEval(){ delete m_f;}

void RootEval::setupParameterTables(const std::vector<std::string>& names, std::vector<Table*>&tables)
{
    for( std::vector<std::string>::const_iterator it (names.begin()); it!=names.end(); ++it){
        const std::string& name(*it);
        tables.push_back(setupHist(name));
    }
}

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

    for( int i = 1; i< m_psfTables.size(); ++i){
        par[i] = m_psfTables[i]->value(loge,costh);
    }

    // rescale the sigma value after interpolation
    par[1]*=PointSpreadFunction::scaleFactor(energy, zdir, isFront());

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

    // manage normalization by replacing normalization parameter for current set of parameters
    par[0]=1;
    static double theta_max(90); // how to set this? Too high.
    double norm = PointSpreadFunction::integral(&theta_max,par);
    par[0] = 1./norm/(2.*M_PI * par[1] * par[1]); // solid angle normalization 
    return par;
}

double * RootEval::disp_par(double energy, double costh)
{
    static bool interpolate(false);//<----- wire in non-interpolation
    static double par[10];
    double loge(::log10(energy));

    if( costh==1.0) costh = 0.9999;

    for( int i = 0; i< m_dispTables.size(); ++i){
        par[i] = m_dispTables[i]->value(loge,costh, interpolate);
    }
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

