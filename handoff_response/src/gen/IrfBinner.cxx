/** @file IrfBinner.h
@brief implement class IrfBinner

$Header$

*/


#include "IrfBinner.h"

#include "embed_python/Module.h"

#include <cmath>
#include <sstream>

IrfBinner::IrfBinner(embed_python::Module& py)
{
    // get the angle and energy bin edges
    py.getList("Bins.angle_bin_edges", m_angle_bin_edges);
    py.getList("Bins.energy_bin_edges", m_energy_bin_edges);
    m_ebins = m_energy_bin_edges.size()-1;
    m_abins = m_angle_bin_edges.size()-1;

    for( std::vector<double>::reverse_iterator i =m_angle_bin_edges.rbegin(); i!=m_angle_bin_edges.rend(); ++i){
        m_angles.push_back( int( acos(*i) * 180/M_PI+0.5)); 
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

