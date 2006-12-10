/** @file Setup.h
    @brief declare class Setup

    $Header$

*/
#ifndef Setup_h
#define Setup_h

#include <vector>
#include <string>
#include <iostream>
namespace embed_python { class Module; }

/** @class Setup
    @brief read in and parse a setup file


*/
class Setup  {
public:

    Setup(int argc, char* argv[], bool verbose=false);
    /// @return the path to the folder with the setup file
    const std::string& root()const{return m_root;}


    static Setup* instance();
    embed_python::Module* py(){return m_py;}

private:
  
    std::string m_root;
    bool m_verbose;
    embed_python::Module* m_py;

    static Setup* s_instance;
};

#endif
