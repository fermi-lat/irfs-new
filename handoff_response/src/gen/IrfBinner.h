/** @file IrfBinner.h
    @brief declare class IrfBinner

    $Header$

    */

#include <vector>
#include <string>
namespace embed_python { class Module; }

class IrfBinner {
public:
    IrfBinner(embed_python::Module& py);

    /// arrays of bin edges
    const std::vector<double>& energy_bin_edges()const{return m_energy_bin_edges;}
    const std::vector<double>& angle_bin_edges()const{return m_angle_bin_edges;}

    /// nummber of bins
    size_t energy_bins()const{return m_ebins;}
    size_t angle_bins()const{return m_abins;}

    /**
        @param zdir cos(theta)
        @return angle bin
    */
    int angle_bin(double zdir)const;

    /** @param energy energy in MeV
        @return energy bin number

        @todo: allow variable bins
    */
    int energy_bin(double energy)const;

    /// energy (MeV) 
    double eCenter(int j)const; 

     /**
    these two functions define indexing in the vector arrays of histogram pointers
    */
    size_t ident(int ebin, int abin)const ;
    size_t size()const {return m_ebins*(m_abins+1);}

    /** 
        the bin edge angles in degress, for forming labels
    */
    int angle( int i)const{ return m_angles[i];}

    /** create histogram id based on the ident
    */
    static const char *  hist_name(int i, int j, std::string base="h"); 
 

private:
     std::vector<double> m_angle_bin_edges, m_energy_bin_edges;
   // define angles in degrees for labels
    std::vector<int> m_angles;


    size_t m_ebins, m_abins;

    std::string m_name;
};
