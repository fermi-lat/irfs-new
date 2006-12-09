/** @file MyAnalysis.h"
@brief declare class MyAnalysis

$Header$

*/
#ifndef MYANALYSIS_H
#define MYANALYSIS_H

#include "Root_base.h"

// ROOT
class TChain;
class TFile;
class TTree;

#include <string>
#include <iostream>
#include <vector>


class MyAnalysis : public Root_base 
{
public:

    MyAnalysis();
    ~MyAnalysis();

    void open_input_file();


    TTree& tree(){return *m_tree;}

    /// @brief apply cuts and select the branch names.
    void makeCutTree();

    void current_time(std::ostream& out=std::cout);


    /** @class MyAnalysis::Normalization
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
    const std::vector<std::string> files() const {return m_files;}
    std::vector<std::string> files(){return m_files;}
private:

    std::string  m_summary_filename;
    std::string m_cuts;

    //! the input file with the TTree
    TFile* m_file;

    //! the  tree that will be analyzed
    TTree* m_tree;
    TChain* m_input_tree;
    TFile* m_out;

    std::vector<std::string> m_files; ///< input file description (for TChain)
    std::vector<std::string> m_branchNames; ///< branches to keep in prune
    std::vector<Normalization> m_norm;///< normalization information 
};

#endif
