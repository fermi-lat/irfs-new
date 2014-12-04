/** @file PsfPlots.h
    @brief declare class PsfPlots

    $Header$
*/
#ifndef IRF_PsfPlots_h
#define IRF_PsfPlots_h


class IrfAnalysis;
class IrfBinner;

#include "PointSpreadFunction.h"
#include "embed_python/Module.h"

#include <vector>

/** @class PSFPlots
 *  @brief manage the PSF plots
*/

class PsfPlots {
public:
    PsfPlots( IrfAnalysis& irf, std::ostream&, embed_python::Module & );
    ~PsfPlots();

    void fill(double diff, double energy, double costheta, bool front);

    void fit();
    void summarize();

    void draw(const std::string &ps_filename) ;


    typedef std::vector<PointSpreadFunction> PSFlist;

    const PSFlist& hists(){return m_hists;} 
    
    // make a set of 2-d histograms with values of the fit parameters
    void fillParameterTables();

    const IrfBinner & binner()const{return m_binner;}

protected:
    IrfAnalysis& m_irf;
    const IrfBinner& m_binner;
    PSFlist m_hists;
    std::ostream * m_log;
    std::ostream& out() {return *m_log;}

};

#endif
