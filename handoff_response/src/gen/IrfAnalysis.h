/** @file IrfAnalysis.h
    @brief Analysis of PSf

$Header$
*/

#include "MyAnalysis.h"

#include <string>

class PsfPlots;
class Dispersion;
class EffectiveArea;

/** @class IrfAnalysis
    @brief Manage fits to binned data, both PSF and dispersion

    */

class IrfAnalysis : public MyAnalysis{
public:
    /// @param folder where to find data file, to put analysis files
    /// @param set 0,1,2 for all, front, back events
    IrfAnalysis(std::string folder, int set=1); 
    
    void project();

    /// do the fits, optionally make plots and write out parameters to a root file
    void fit(bool make_plots=false, std::string output_type="ps");


    /**@brief write out fit parameters to the ROOT file, tabular TTree
    do both psf and dispersion 
    */
    void writeFitParameters(std::string outputFile);

    void makeParameterTuple();

    /// Read back a root file, tablulate by parameter
    void tabulate(std::string filename);


    /// arrays of bin edges
    const std::vector<double>& energy_bin_edges()const{return m_energy_bin_edges;}
    const std::vector<double>& angle_bin_edges()const{return m_angle_bin_edges;}

    size_t energy_bins()const{return m_ebins;}
    size_t angle_bins()const{return m_abins;}

    int angle_bin(double zdir){
        double delta(m_angle_bin_edges[1]-m_angle_bin_edges[0]);
        return static_cast<int>( (zdir+1.0)/delta);
    }
    /** @param energy energy in MeV
        @return energy bin number

        @todo: allow variable bins
    */
    int energy_bin(double energy){
        double logemin(m_energy_bin_edges[0])
            ,  logedelta(m_energy_bin_edges[1] -logemin)
            ,  logestart(logemin+0.5*logedelta);
        return static_cast<int>((log10(energy)-logestart+0.5*logedelta)/logedelta);
    }

    // define angles in degrees for labels
    std::vector<int> angles;

    std::string name()const{ return m_name;}

    void setName(std::string name){m_name = name;}

    double eCenter(int j)const{
        double loge_mean( 0.5*(m_energy_bin_edges[j] + m_energy_bin_edges[j+1]) );
        return pow(10.0, loge_mean);
    }

    static const char *  hist_name(int i, int j, std::string base="h") {
        std::stringstream t; t << base << i <<"_" << j;
        static char buffer[16];
        ::strncpy(buffer,  t.str().c_str(), sizeof(buffer));
        return buffer;
    }

    /**
    these two functions define indexing in the vector arrays of histogram pointers
    */
    size_t ident(int ebin, int abin){ 
        return abin<m_abins? ebin + abin* (m_ebins) : m_ebins*m_abins+ebin; 
    }
    size_t size(){return m_ebins*(m_abins+1);}

    /** @class IrfAnalysis::Normalization
        @brief information allowing normalization for effective area

        */
    class Normalization {
    public:
        /** 
        @param generated number of events initially generated
        @param logemin minimum log10(McEnergy)
        @param logemax maxiumum log10(McEnergy)
        */
        Normalization(int generated, double logemin, double logemax)
            :m_events(generated)
            ,m_low(logemin)
            ,m_high(logemax)
        {}
        bool in_range(double loge, double costh)const{return loge>m_low && loge<=m_high && costh>0 && costh<=1;} 
        int generated()const{return m_events;}
        double logemin()const{return m_low;}
        double logemax()const{return m_high;}
        double value(double loge, double costh)const{ return in_range(loge, costh) ? m_events/(m_high-m_low) : 0;} 
    private:
        int m_events;
        double  m_low, m_high;
    };

    const std::vector<Normalization>& normalization()const{return m_norm;}
    std::vector<Normalization>& normalization() {return m_norm;}


    // the root file where we save all the histograms
    std::string summary_filename()const{return output_file_root()+"/"+m_outputfile;}
    double generate_area()const{return m_generate_area;}

private:

    std::vector<double> m_angle_bin_edges, m_energy_bin_edges;

    std::vector<Normalization> m_norm;///< normalization information 
    double m_generate_area;

    size_t m_ebins, m_abins;

    std::string m_name;
    std::string m_filename_root;
    
    PsfPlots* m_psf;    ///< manage PSF plots
    Dispersion* m_disp; ///< manage energy dispersion plots
    EffectiveArea* m_aeff; ///< manage the effective area plot
    int m_set;  ///< all, front, back
    std::ostream * m_log;
    std::string m_setname; ///< describe the data set
    std::string m_classname; ///<event class, derived from folder name

    int m_nruns; ///< number of runs found
    double m_minlogE, m_maxlogE; ///< minimum, maximum logE found

    std::ostream& out() {return *m_log;}

    std::string m_parameterFile;
    const std::string& parfile()const{return m_parameterFile;}
 
    const std::string& output_file_root()const{return m_filename_root;}
    std::string m_outputfile;
 
 
 
};

