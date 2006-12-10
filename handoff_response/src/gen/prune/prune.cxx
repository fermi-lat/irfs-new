/** @file prune.cxx
@brief create pruned output file

$Header$
*/

#include "../MyAnalysis.h"
#include "../Setup.h"

#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <iterator>


//_____________________________________________________________________________

int main(int argc, char* argv[]){
    int ret=0;
    try {

        Setup s(argc, argv);

        MyAnalysis copier(*s.py()); // makes a copy of the good gammas

        copier.makeCutTree();

    }catch( const std::exception& e){
        std::cerr << "Caught exception "<< e.what() << std::endl;
        ret=1;
    }
    return ret;
}

/** @page prune The prune application

Use the Setup class to process arguments, which are assumed to be just a path to a folder containing a 
file "setup.txt". 

Expect this to define three parameters:

- the full path to the original ROOT file
- a list of cuts to apply
- a list of tuple names to enable

The created file is "goodEvent.root" in the same folder.

*/
