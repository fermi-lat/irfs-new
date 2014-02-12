/** 
 * @file Setup.cxx
 * @brief define class Setup
 * $Header$
 */

#include "Setup.h"
#include "embed_python/Module.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>

// support for singleton 
Setup * Setup::instance() {
   if (s_instance==0) {
      throw std::invalid_argument("Setup object not set");
   }
   return s_instance;
}

Setup * Setup::s_instance(0);

Setup::Setup(int argc, char * argv[], bool verbose) 
   : m_root("."), m_verbose(verbose) {
   s_instance = this;
   /// The first argument is the python setup file.
   /// Strip off trailing ".py" if it is present.
   if (argc != 2) {
      throw std::runtime_error("The python setup file should be "
                               "the one and only argument.");
   }
   std::string setup_module(argv[1]);
   // strip off trailing ".py" if present.
   size_t pos(setup_module.find(".py"));
   if (pos == setup_module.length() - 3) {
      setup_module = setup_module.substr(0, pos);
   }
   if (m_verbose) {
      std::cout << "setup module: " << setup_module << std::endl;
   }
   std::string path_to_module;
   m_py = new embed_python::Module(path_to_module="", setup_module);
   try {
      m_py->getValue("root_dir", m_root);
   } catch (std::invalid_argument &) {
      /// Use default value of "."
      std::cout << "Setting root directory for output to '.'" << std::endl;
   }
}
