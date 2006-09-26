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

    MyAnalysis(std::string summary_root_filename="");
    ~MyAnalysis();

    void open_input_file(std::string filename="");

    std::string output_file_root(){
        return std::string(::getenv("output_file_root"))+"/";
    }    

    std::string input_filename();
    TTree& tree(){return *m_tree;}

    /// @brief apply cuts and select the branch names.
    void makeCutTree(const std::string& cuts, const std::vector<std::string>& branchnames, std::string filename);

    void current_time(std::ostream& out=std::cout);

    static std::string s_input_filename;

private:

    std::string  m_summary_filename;

    //! the input file with the TTree
    TFile* m_file;

    //! the  tree that will be analyzed
    TTree* m_tree;
    TChain* m_input_tree;
};

#endif
