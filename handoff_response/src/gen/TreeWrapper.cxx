/** @file TreeWrapper.cxx
    @brief implementation of TreeWrapper class

    $Header$
*/

#include "TreeWrapper.h"

#include "TFile.h"
#include "TTree.h"
#include "TKey.h"
#include "TString.h"
#include "TLeaf.h"
#include <stdexcept>

TreeWrapper* TreeWrapper::s_instance=0;
#ifdef WIN32
TTree dummy; // for linking bug?
#endif

namespace {
    bool flag (false) ;
    class Watcher : public TObject {
    public:
        bool Notify(){flag = true; 
        return true;
        }
    }s_watcher;
}// namespace


TreeWrapper::TreeWrapper(std::string filename, std::string treename, std::string filter)
: m_file( new TFile(filename.c_str(),"readonly"))
{
    if( ! m_file->IsOpen() ) throw std::invalid_argument("TreeWrapper: could not open "+filename);
    m_tree = findTree(treename);
    if(m_tree==0) throw std::invalid_argument("TreeWrapper: could not find TTree "+treename + " in file "+filename);

    if( ! filter.empty() ){ // apply filter expression by creating new tree
        int initialsize( static_cast<int>(m_tree->GetEntries()) );
        TFile * dummy =new TFile("dummy.root", "recreate");// for copy tree.
        TTree * tnew = m_tree->CopyTree(filter.c_str() );
        int size ( (int)tnew->GetEntries() );
        if( size == 0) {
            delete dummy;
            throw std::runtime_error(std::string("TreeWapper: Filter expression \"")+filter+"\" yielded no events");
        }
        m_tree = tnew;
        std::cout << "\t " << size << "/" << initialsize << " events" << std::endl;
        delete dummy; // to silence warnings.
    }
    // turn off all branches: enable them as requested by leaf calls for the event loop
    m_tree->SetBranchStatus("*", 0);
    s_instance = this;

}

TreeWrapper::TreeWrapper(TTree* tree)
: m_file(0)
, m_tree(tree)
{
    s_instance = this;
    // if this is really a TChain, we want to be notified if there is a new TTree
    tree->SetNotify(&s_watcher);
}

TreeWrapper::~TreeWrapper()
{
    delete m_file; // if we own it, delete it
    s_instance=0;
}



TTree* TreeWrapper::findTree(std::string treename) 
{
    // if specified, assume it is a tree and grab it
    if( !treename.empty()) return (TTree*)m_file->Get(treename.c_str());

    // otherwise find first toplevel object that is a TTree

    // Create an iterator on the list of keys 
    TIter nextTopLevelKey(m_file->GetListOfKeys());
    TKey *keyTopLevel;

    // loop on keys, and search for a TTree 
    while( (keyTopLevel=(TKey*)nextTopLevelKey())!=0 ) {
        TString name(keyTopLevel->GetName());
        TString className(keyTopLevel->GetClassName());

        if( className.CompareTo("TTree") ==0 )  {
            // Found one
            return (TTree*)m_file->Get(keyTopLevel->GetName());
        }
    }
    return 0; // failed
}

TreeWrapper::Leaf TreeWrapper::leaf(std::string name)
{
    TLeaf* leaf = m_tree->GetLeaf(name.c_str());
    if(leaf==0) throw std::invalid_argument(
        std::string("TreeWrapper: Leaf name ")+name+" not found");
    m_tree->SetBranchStatus(name.c_str(), 1);
    
    return TreeWrapper::Leaf(leaf);
}

TreeWrapper::Iterator::Iterator(TTree* tree, int rec)
: m_tree(tree)
, m_rec( rec>=0? rec : tree->GetEntries())
{
    if(rec>=0) m_tree->GetEntry(rec); // prime the pump
}

TreeWrapper::Iterator TreeWrapper::Iterator::operator++(){ 
    int read =m_tree->GetEntry(++m_rec);  
    return *this;
}
// post-iterator
TreeWrapper::Iterator TreeWrapper::Iterator::operator++(int){ 
    Iterator temp = *this;
    m_tree->GetEntry(++m_rec);  
    return temp;
}

size_t TreeWrapper::size()const{return static_cast<size_t>(m_tree->GetEntries());}

TreeWrapper::Iterator TreeWrapper::begin(int i){return Iterator(m_tree, i);}
TreeWrapper::Iterator TreeWrapper::end(){return Iterator(m_tree, -1);}

TreeWrapper::Leaf::operator double()const
{
    return m_leaf->GetValue();
}

TreeWrapper::Leaf::Leaf(std::string name)
{
    if( TreeWrapper::instance() == 0 ) throw std::invalid_argument("TreeWrapper::Leaf: no TreeWrapper instance available");

    // copy that leaf to this.
    m_leaf = TreeWrapper::instance()->leaf(name).m_leaf;
}
