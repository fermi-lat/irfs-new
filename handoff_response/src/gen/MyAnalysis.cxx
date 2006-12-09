/** @file MyAnalysis.cxx
*/
#include "MyAnalysis.h"
#include "TreeWrapper.h"

#include "TChain.h"
#include "TFile.h"

#include "Setup.h"

#include <cmath>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "embed_python/Module.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MyAnalysis::MyAnalysis()
:m_out(0)
{
    // get file information from input description 
    // first, file list

    embed_python::Module& py = *(Setup::instance()->py());

    py.getList("Data.files", m_files);
    std::cout << "Reading from " << m_files.size() << " filelists" << std::endl;

    // then set of info
    std::vector<double> generated, logemins, logemaxes;
    py.getList("Data.generated", generated);
    py.getList("Data.logemin", logemins);
    py.getList("Data.logemax", logemaxes);
    for( int i=0; i<generated.size(); ++i){
        normalization().push_back(Normalization(generated[i], logemins[i], logemaxes[i]));
    }
    py.getValue("Prune.cuts", m_cuts);
    py.getValue("Prune.fileName", m_summary_filename);
    py.getList("Prune.branchNames", m_branchNames);

    current_time();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MyAnalysis::~MyAnalysis()
{
    current_time();
    delete m_out;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MyAnalysis::open_input_file()
{
    static std::string tree_name("MeritTuple");
    std::cout << "Creating TChain(\"MeritTuple\") and adding files matching "
        << m_summary_filename << std::endl;

    m_input_tree = new TChain("MeritTuple");
    m_input_tree->SetMaxTreeSize(4000000000L); // 4 gigs?

    // reprocessing the original file 
    m_input_tree->Add(m_summary_filename.c_str());
#if 0
    TChain norms("norm");
    norms.Add(s_input_filename.c_str());
    if( norms.GetEntries()==0) throw std::invalid_argument("MyAnalysis::open_input_file -- did not find the normaliztion data");
    TreeWrapper tnorm(&norms);
    TreeWrapper::Leaf generated("generated"), logemin("logemin"), logemax("logemax");
    for( TreeWrapper::Iterator it = tnorm.begin(); it!=tnorm.end(); ++it){
        m_norm.push_back(Normalization(generated, logemin, logemax));
    }
#endif

    if( m_input_tree==0 || m_input_tree->GetEntries()==0) {
        std::cerr << "Did not find tree \"" << tree_name << "\" in the input file" << std::endl;
        throw "did not find the TTree";
    }
    std::cout << "Tree " << m_input_tree->GetTitle() 
        << " has " << m_input_tree->GetEntries() << " entries." << std::endl;

    m_tree= m_input_tree;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MyAnalysis::makeCutTree()
{
    static std::string tree_name("MeritTuple");
    std::cout << "Creating TChain(\"MeritTuple\") from files" << std::endl;

    m_input_tree = new TChain(tree_name.c_str());
    m_input_tree->SetMaxTreeSize(4000000000L); // 4 gigs?

    for( std::vector<std::string>::const_iterator sit = m_files.begin(); sit!=m_files.end(); ++sit){
        std::cout << "\t" << *sit << std::endl;
        m_input_tree->Add((*sit).c_str());
    }
    std::cout << "Copying cut tree, using cuts "<< m_cuts << std::endl;
    if( !m_summary_filename.empty() ){
        m_out = new TFile(m_summary_filename.c_str(), "recreate");
    }

    m_input_tree->SetBranchStatus("*", 0);
    for(std::vector<std::string>::const_iterator i=m_branchNames.begin(); i!=m_branchNames.end(); ++i){
        m_input_tree->SetBranchStatus((*i).c_str(), 1);
    }
    m_tree = m_input_tree->CopyTree(m_cuts.c_str());
    m_tree->Write(); // save it
    std::cout << "Wrote " << m_tree->GetEntries() << " events to file " << m_summary_filename << std::endl;

    // now create and save the normalization tuple

    if( m_norm.size()>0) {
        TTree*  norm = new TTree("norm", "normalization");
        int generated; 
        double logemin,logemax;

        norm->Branch("generated",&generated, "generated/I");
        norm->Branch("logemin", &logemin, "logemin/D");
        norm->Branch("logemax", &logemax, "logemax/D");
        for( std::vector<Normalization>::const_iterator it = m_norm.begin(); it!=m_norm.end(); ++it){
            generated = it->generated();
            logemin   = it->logemin();
            logemax   = it->logemax();
            norm->Fill();
        }
        norm->Write();

        std::cout << "Wrote " << m_norm.size() << " entries to the normalization tuple" << std::endl;
    }else{
        std::cout << "No normalization info: tuple not created"<< std::endl;
    }
}

#include <time.h>

void MyAnalysis::current_time(std::ostream& out)
{   
    static bool first=true;
    static time_t start;
    if(first){ first=false; ::time(&start);}
    time_t aclock;
    ::time( &aclock );   
    char tbuf[25]; ::strncpy(tbuf, asctime( localtime( &aclock ) ),24);
    tbuf[24]=0;
    double deltat ( ::difftime( aclock, start) );
    out<<  "Current time: " << tbuf
        << " ( "<< deltat <<" s elapsed)" << std::endl;
}
