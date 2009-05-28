/** @file IrfAnalysis.h
    @brief Analysis of PSf

$Header$
*/

#include "IRF.h"

#include <string>

class PsfPlots;
class Dispersion;
class EffectiveArea;

/** @class IrfAnalysis
    @brief Manage fits to binned data, both PSF and dispersion

    */

class IrfAnalysis : public IRF {
public:
    /// @param folder where to find data file, to put analysis files
    /// @param set 0,1,2 for all, front, back events
    IrfAnalysis(std::string folder, int set=1, std::ostream& log=std::cout); 
    
    void project();

    /// do the fits, optionally make plots and write out parameters to a root file
    void fit(bool make_plots=false, std::string parfile="", std::string output_type="ps");

    /** @return the acceptance per event per energy-angle bin, in m^2 * sr.
    */
    double aeff_per_event();

    /**@brief write out fit parameters to the ROOT file, tabular TTree
    do both psf and dispersion 
    */
    void writeFitParameters(std::string outputFile);

    /// Read back a root file, tablulate by parameter
    void tabulate(std::string filename);

private:

    std::string m_filename_root;
    
    PsfPlots* m_psf;    ///< manage PSF plots
    Dispersion* m_disp; ///< manage energy dispersion plots
    EffectiveArea* m_aeff; ///< manage the effective area plot
    int m_set;  ///< all, front, back
    std::string m_setname; ///< describe the data set

    std::string m_classname; ///<event class, derived from folder name
    std::ostream * m_log;
    std::ostream& out() {return *m_log;}

    int m_nruns; ///< number of runs found
    double m_minlogE, m_maxlogE; ///< minimum, maximum logE found

};

