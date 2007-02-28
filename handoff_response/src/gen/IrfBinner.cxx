/** @file IrfBinner.h
@brief implement class IrfBinner

$Header$

*/


#include "IrfBinner.h"

#include "embed_python/Module.h"

#include <cmath>
#include <sstream>
#include <stdexcept>

IrfBinner::IrfBinner(embed_python::Module& py) 
   : m_edispEnergyOverLap(0), m_edispAngleOverLap(0),
     m_psfEnergyOverLap(0), m_psfAngleOverLap(0)
{
    // get the angle and energy bin edges
    py.getList("Bins.angle_bin_edges", m_angle_bin_edges);
    py.getList("Bins.energy_bin_edges", m_energy_bin_edges);
    m_ebins = m_energy_bin_edges.size()-1;
    m_abins = m_angle_bin_edges.size()-1;

    for( std::vector<double>::reverse_iterator i =m_angle_bin_edges.rbegin(); i!=m_angle_bin_edges.rend(); ++i){
        m_angles.push_back( int( acos(*i) * 180/M_PI+0.5)); 
    }

    try {
       py.getValue("Bins.edisp_energy_overlap", m_edispEnergyOverLap);
       py.getValue("Bins.edisp_angle_overlap", m_edispAngleOverLap);
    } catch (std::invalid_argument &) {
       // use defaults
    }

    try {
       py.getValue("Bins.psf_energy_overlap", m_psfEnergyOverLap);
       py.getValue("Bins.psf_angle_overlap", m_psfAngleOverLap);
    } catch (std::invalid_argument &) {
       // use defaults
    }
}


int IrfBinner::angle_bin(double zdir)const{
    double delta(m_angle_bin_edges[1]-m_angle_bin_edges[0]);
    return static_cast<int>( (zdir+1.0)/delta);
}


int IrfBinner::energy_bin(double energy)const
{
    double logemin(m_energy_bin_edges[0])
        ,  logedelta(m_energy_bin_edges[1] -logemin)
        ,  logestart(logemin+0.5*logedelta);
    return static_cast<int>((log10(energy)-logestart+0.5*logedelta)/logedelta);
}

size_t IrfBinner::ident(int ebin, int abin)const 
{ 
    return abin<m_abins? ebin + abin* (m_ebins) : m_ebins*m_abins+ebin; 
}

int IrfBinner::hist_id(int ebin, int abin) const {
   if (ebin < 0 || ebin >= static_cast<int>(energy_bins()) || 
       abin < 0 || abin >= static_cast<int>(angle_bins())) {
      return -1;
   }
   return ident(ebin, abin);
}

double IrfBinner::eCenter(int j)const{
    double loge_mean( 0.5*(m_energy_bin_edges[j] + m_energy_bin_edges[j+1]) );
    return pow(10.0, loge_mean);
}


const char *  IrfBinner::hist_name(int i, int j, std::string base)
{
    std::stringstream t; t << base << i <<"_" << j;
    static char buffer[16];
    ::strncpy(buffer,  t.str().c_str(), sizeof(buffer));
    return buffer;
}

