/**
 * @file IrfLoader.h
 * @brief Definition of IrfLoader class
 *
 * $Header$
 */

#ifndef handoff_response_IrfLoader_h
#define handoff_response_IrfLoader_h

#include <map>
#include <string>
#include <vector>

namespace handoff_response {class IrfEval;}
namespace irfInterface { class Irfs;}

namespace handoff_response {

/**
 * @class IrfLoader
 *
 * @brief load irfs from parameter file, create an Irfs object

 This is a wrapper around an IrfEval object, which interprets a file
 containing the tabular information for perhaps several response
 functions
 *
 */

class IrfLoader {
public:

   IrfLoader();

    /** @brief ctor 
        @param parameterfile file containing parameter tables
        
        Depending on the type of the file it invokes an appropriate
        IrfEval subclass.
    */
   IrfLoader(const std::string & parameterfile);

   ~IrfLoader();

   typedef std::map<std::string, handoff_response::IrfEval*>::const_iterator 
   const_iterator;

   /** @brief return pointer to an Irfs object
       @param index index to assign--should be unique, but not enforced
   */
   irfInterface::Irfs * irfs(std::string name, int index=0);

   const_iterator begin()const{return m_evals.begin();}
   const_iterator end()const{return m_evals.end();}

   void getKeys(std::vector<std::string> & keys) const;

   void addIrfEval(const std::string & name, IrfEval * eval);

private:

    std::map<std::string, handoff_response::IrfEval*> m_evals;

};

} // namespace handoff_response

#endif // handoff_IrfLoader_h
