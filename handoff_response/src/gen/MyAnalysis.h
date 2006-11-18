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


class MyAnalysis : public Root_base {
public:

    MyAnalysis(std::string summary_root_filename="", std::string cut_filename="");
    ~MyAnalysis();

    void open_input_file(std::string filename="");

    std::string output_file_root(){
        return std::string(::getenv("output_file_root"))+"/";
    }    

    std::string input_filename();
    TTree& tree(){return *m_tree;}

    /// @brief apply cuts and select the branch names.
    void makeCutTree(const std::string& cuts, const std::vector<std::string>& branchnames);

    void current_time(std::ostream& out=std::cout);

    static std::string s_input_filename;

    /** @class MyAnalysis::Normalization
        @brief information allowing normalization for effective area

        */
    class Normalization {
    public:
        /** 
        @param generated number of events initially generated
        @param logemin minimum log10(McEnergy)
        @param logemax maxiumum log10(McEnergy)
        @param entries number of events found in the file after cuts
        */
        Normalization(int generated, double logemin, double logemax, int entries)
            :m_events(generated)
            ,m_low(logemin)
            ,m_high(logemax)
            ,m_entries(entries)
        {}
        bool in_range(double loge, double costh)const{return loge>m_low && loge<=m_high && costh>0 && costh<=1;} 
        int entries()const{return m_entries;}
        int generated()const{return m_events;}
        double value(double loge, double costh)const{ return in_range(loge, costh) ? m_events/(m_high-m_low) : 0;} 
        int m_events, m_entries;
        double  m_low, m_high;
    };

    const std::vector<Normalization>& normalization()const{return m_norm;}
private:

    std::string  m_summary_filename;

    //! the input file with the TTree
    TFile* m_file;

    //! the  tree that will be analyzed
    TTree* m_tree;
    TChain* m_input_tree;
    TFile* m_out;

    std::vector<Normalization> m_norm;
};

#endif
