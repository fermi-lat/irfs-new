/** @file Dispersion.h
*/
#ifndef IRF_Dispersion_h
#define IRF_Dispersion_h


#include <vector>
#include <iostream>
#include <string>
class IrfAnalysis;
class TH1F;
class TH2F;

/** @class Dispersion
 *  @brief manage the PSF plots
*/

class Dispersion {
public:
    Dispersion( IrfAnalysis& irf, std::ostream& log=std::cout);
    ~Dispersion();

    void fill(double diff, double energy, double costheta, bool front);

    void fit(std::string opts="RQ");
    void summarize();

    void draw(const std::string &ps_filename) ;

    /// make a set of 2-d histograms with values of the fit parameters
    /// binning according to energy and costheta bins
    void fillParameterTables();

    /// @brief access to the function used
    static double function(double* x, double* par); 

    /** @class Dispersion::Hist
        @brief nested class manages a ROOT histogram
        */

    class Hist{
    public:
        Hist(std::string id, std::string title);
        Hist(){};
        operator TH1F*(){return m_h;}
        void fill(double diff);
        void fit(std::string opts="RQ");
        void summarize(std::ostream& out= std::cout);
        void draw()const;
        double ltail()const;
        double rwidth()const;
        double chisq()const;
        void getFitPars(std::vector<double>& pars)const;
        double entries()const;

        double parameter(int n)const;
       // static const char* pnames[];
        static std::vector<std::string> pnames;
        static double      pinit[];
        static double      pmin[];
        static double      pmax[];
        static double      fitrange[2];

        static int npars();

        static bool s_logy; ///< determine if log y
        static int  s_minEntries; ///< minimum number of entries to fit
        static int  s_bins; ///< number of bins in fit histogram
        static double s_histrange[2]; ///< range for the fit histogram

        static void summaryTitle(std::ostream& out=std::cout);
    private:
        TH1F* m_h;
        double m_lowTail, m_highTail;
    };
    typedef std::vector<Hist> HistList;
    const HistList& hists()const{return m_hists;}

private:

    IrfAnalysis& m_irf;
    typedef std::vector<Hist> HistList;
    HistList m_hists; // simple histogram lookukp
    std::ostream * m_log;
    std::ostream& out() {return *m_log;}

};

#endif
