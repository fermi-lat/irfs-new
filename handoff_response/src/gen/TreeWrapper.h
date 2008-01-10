/** @file TreeWrapper.h
    @brief declaration of TreeWrapper class

    $Header$

*/

#ifndef TreeWrapper_h
#define TreeWrapper_h

#include <string>

// forward declarations: interface does not need any ROOT declarations
class TTree;
class TFile;
class TLeaf;

/** @class TreeWrapper 
    @brief simple class to open a ROOT file or tree, and support container-like iteration, 
    using nested class Iterator for forward interation
    
    Note the nested class Leaf allowing simple access to the value of any numeric column

    Example: 
    @verbatim

    TreeWrapper tw("file.root"); // do not need to specify tree name
    TrewWrapper::Leaf x("x"), y("y"); // objects like doubles corresponding to leaf names
    for( TreeWrapper::Iterator it = tw.begin(); it!=tw.end(); ++it){
        double r = sqrt(x*x+y*y); // will use current values of x, y
        }
    @endverbatim
    Note that dereferencing of an iterator is not supported: not clear what it should evaluate to

**/

class TreeWrapper {
public:
    /**@brief open the file, get specified tree, or first one found

    @param filename name of the root file
    @param treename optional name of a TTree
    @param filter   expression that will filter the output
    */
    TreeWrapper(std::string filename,  std::string treename="", std::string filter="");

    ///@brief wrap an existing TTree. 
    TreeWrapper(TTree* tree);

    ~TreeWrapper();

    ///@brief access to the wrapped TTree
    operator TTree*(){return m_tree;}

    ///@class TreeWapper::Iterator
    ///@brief For iteration over the entries of the wrapped tree
    class Iterator {
    public:
        ///@param tree tree to control
        ///@param rec starting entry number: if -1, set to end.
        Iterator(TTree* tree, int rec);
        Iterator operator++(); 
        Iterator operator++(int);
        bool operator!=(const Iterator& other){return m_rec!= other.m_rec;}
        size_t index()const{ return m_rec;} ///< @return entry number
    private:
        size_t m_rec;
        TTree* m_tree;
    };

    size_t size()const; ///< @return number of entries in the tree

    Iterator begin(int i=0); ///< @return a begin iterator, setting the TTree to the first, or indicated entry
    Iterator end();   ///< @return an end iterator, allowing termination of the loop

    ///@class TreeWrapper::Leaf
    ///@brief double-like object evaluates to the current value of a numeric TLeaf
    class Leaf{
    public:
        Leaf(TLeaf * leaf): m_leaf(leaf){}
        Leaf(std::string name); ///< ctor will use current TreeWrapper instance
        operator double()const;
    private:
        TLeaf* m_leaf;
    };

    Leaf leaf(std::string name); ///< @return a double-like object corresponding to the leaf name

    /// Note that this class is not a singleton: this is just the most recent instance
    static TreeWrapper* instance(){return s_instance;}

private:
    TTree* findTree(std::string treename);

    TFile* m_file;  ///< the file: zero if not owned (i.e., opened)
    TTree* m_tree;  ///< the tree

    static TreeWrapper* s_instance; // most recent instance
};

#endif
