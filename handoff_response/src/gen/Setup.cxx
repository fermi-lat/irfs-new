/** @file Setup.cxx
@brief define class Setup

$Header$

*/

#include "Setup.h"
#include "embed_python/Module.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#ifdef WIN32
 #include <direct.h> // for chdir
#else
# include <unistd.h>
namespace {
    int _chdir( const char * d){return ::chdir(d);}
    char * _getcwd(  char * buffer,   size_t maxlen ){return ::getcwd(buffer, maxlen);}
}
#endif

namespace{
    std::string 
        envvar("output_file_root") //here to look if no command-ine arg
        , setupfile("setup"); // file name to read and parse
}
// support for singleton
Setup* Setup::instance(){if( s_instance==0) throw std::invalid_argument("Setup object not set");return s_instance;}

Setup* Setup::s_instance(0);

Setup::Setup(int argc, char* argv[], bool verbose)
: m_verbose(verbose)
{
    s_instance = this;
   char * envvarvalue = ::getenv(envvar.c_str());
   if (argc ==1 && envvarvalue == 0) {
//        throw std::runtime_error(std::string(argv[0])+": no command line argument and the environment variable " + envvar 
//                                + " is not set");

// Use current working directory if output_file_root env var is not
// set and no command line argument is given.
      char value[128];
      _getcwd(value, sizeof(value));
      envvarvalue = value;
   }
   m_root = std::string(argc>1? argv[1] : envvarvalue);

    char oldcwd[128], newcwd[128];
    _getcwd(oldcwd, sizeof(oldcwd));

    std::cout << "Current working directory: " << oldcwd << std::endl;

    if( _chdir(m_root.c_str()) !=0 ){
        throw std::runtime_error("Setup: could not find folder " +m_root);
    }
    // save current working directory.
    _getcwd(newcwd, sizeof(newcwd));
    std::cout << "switched to " << newcwd << std::endl;
    m_root = newcwd; 

    // python implementation: expect to find file setup.py in the current path, 
    // defines three string attributes: files, cuts, and names
    m_py = new embed_python::Module("", setupfile);    
}
