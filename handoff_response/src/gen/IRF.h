/** @file IRF.h
  @brief Create a set of histograms to allow analysis of the  response

  $Header$
*/
#ifndef IRF_H
#define IRF_H
#include "MyAnalysis.h"

#include <string>
#include <iostream>
#include <vector>

class TCanvas;
/**
    @class IRF
    @brief Base class with files and cuts for IRF analysis

*/
class IRF : public MyAnalysis{
public:
    IRF(std::string output_file_root,
        std::string summary_root_filename="irf.root");
    ~IRF(){};

    /// divide a canvas
    void divideCanvas(TCanvas & c, int nx, int ny, std::string top_title) ;

    const char* angle_cut(int i){
        static char buffer[256];
        sprintf(buffer, "abs(McZDir-%f)<0.05", -0.95+i*0.1);
        return buffer;
    }

    /**
        @param zdir cos(theta)
        @return angular bin number
    */
    int angle_bin(double zdir){
        return static_cast<int>( (zdir+1.0)/0.1);
    }

    const char* energy_cut(int j){
        static char buffer[256];
        double logecenter=logemin+logedelta*(j+0.5); 
        sprintf(buffer, "abs(McLogEnergy-%f)<%f", logecenter, logedelta/2);
        return buffer;
    }

    /** @param energy energy in MeV
        @return energy bin number
    */
    int energy_bin(double energy){
        double logestart(logemin+0.5*logedelta);
        return static_cast<int>((log10(energy)-logestart+0.5*logedelta)/logedelta);
    }

    static double eCenter(int j){
        double logestart(logemin+0.5*logedelta);
        return pow(10.0, logestart + (j%energy_bins)*logedelta);
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
    size_t ident(int ebin, int abin){ return abin<m_abins? ebin + abin* (m_ebins) : m_ebins*m_abins+ebin; }
    size_t size(){return m_ebins*(m_abins+1);}
#if 0
    bool fileExists(){
        TFile f(summary_filename().c_str());
        return f.IsOpen();
    }
#endif
    std::string output_file_root(){
        std::string ret( m_output_file_root.empty()? "" : m_output_file_root+ "/");
        return ret; //m_output_file_root + "/";
    }    

    std::string summary_filename(){ return m_summary_filename;}

    void set_ymin(double y){ m_ymin=y;}
    void set_ymax(double y){ m_ymax=y;}

    // public stuff
    
    // define energy bins
    static double logemin, logedelta;
    static int energy_bins;

    // define angle bins
    static double deltaCostheta; // delta cos theta
    static int angle_bins;
    std::vector<int> angles;
    std::string name()const{ return m_name;}

    void setName(std::string name){m_name = name;}

private:

    int m_ebins, m_abins;

    std::string  m_summary_filename;
    // folder to put output files
    std::string m_output_file_root;

 
    // limits used in the draw phase
    double m_ymin, m_ymax;

    std::string m_name; 
};
#endif
