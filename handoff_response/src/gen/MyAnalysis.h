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
class TCanvas;

namespace embed_python { class Module;}

#include <string>
#include <iostream>
#include <vector>


class MyAnalysis : public Root_base 
{
public:

    MyAnalysis(embed_python::Module& py);
    ~MyAnalysis();

    void open_input_file();

    TTree& tree(){return *m_tree;}

    /// @brief apply cuts and select the branch names.
    void makeCutTree();

    void current_time(std::ostream& out=std::cout);

    /// divide a canvas
    void divideCanvas(TCanvas & c, int nx, int ny, std::string top_title) ;

    const std::string& summary_filename()const{return m_summary_filename;}


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

};

#endif
