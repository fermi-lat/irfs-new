/** @file PsfPlots.h
    @brief declare class PsfPlots

    $Header$
*/
#ifndef IRF_PsfPlots_h
#define IRF_PsfPlots_h


class IrfAnalysis;


#include "PointSpreadFunction.h"
#include <vector>

/** @class PSFPlots
 *  @brief manage the PSF plots
*/

class PsfPlots {
public:
    PsfPlots( IrfAnalysis& irf, std::ostream& log=std::cout);
    ~PsfPlots();

    void fill(double diff, double energy, double costheta, bool front);

    void fit();
    void summarize();

    void draw(const std::string &ps_filename) ;


    typedef std::vector<PointSpreadFunction> PSFlist;

    const PSFlist& hists(){return m_hists;} 
    
    // make a set of 2-d histograms with values of the fit parameters
    void fillParameterTables();

protected:
    IrfAnalysis& m_irf;
    PSFlist m_hists;
    std::ostream * m_log;
    std::ostream& out() {return *m_log;}

};

#endif