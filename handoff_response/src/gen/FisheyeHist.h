/** @file FisheyeHist.h
@brief declaration of class FisheyeHist

$Header$

*/

#ifndef IRF_FisheyeHist_h
#define IRF_FisheyeHist_h


#include "TF1.h"
#include <string>
#include <iostream>
#include <vector>
class TH1F;

class FisheyeHist {

public:

   FisheyeHist();

   FisheyeHist(std::string histname, std::string title);

    ~FisheyeHist();

    /// add a point with a scaled angular difference delta
    void fill(double scaled_delta, double weight=1.0);

    /// add a summary line to a table, with 68%, 95%, and fit parameters.
    void summarize(std::ostream & out);

    /// draw to the current pad
    void draw(double ymin=1e-6, double ymax=1.0, bool ylog=true);

    /// make a fit, using standard PSF function
    void fit(std::string opts = "RQ");

    /// get vector of fit parameters (all zero if not fit)
    void getFitPars(std::vector<double> & pars)const;

    void getFitErrs(std::vector<double> & pars)const;

    /// access to the function itself
    static double function(double* delta, double* par);

    /// the integral of the function
    static double integral(double* delta, double* par);
    double entries()const{return m_count;}

    ///! list of names
    static std::vector<std::string> pnames;

    static const char* parname(int i);
    static int npars();

    static void summary_title(std::ostream & out);

private:

    TH1F& hist(){return *m_hist;}
    TH1F* m_hist;  ///< managed histogram
    TH1F* m_cumhist; ///< a cumulative histogram, generated before fit
    TF1 m_fitfunc; ///< the fit function

    double m_count; ///< number of entries

    double m_mean; 
    double m_median; 
    double m_peak;
    double m_mean_err; ///< Std. error on the mean

    void reorder_parameters();
};

#endif
