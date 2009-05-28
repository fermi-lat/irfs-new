/**
 * @file IrfLoader.h
 * @brief Definition of IrfLoader class
 *
 * $Header$
 */

#ifndef handoff_response_IrfLoader_h
#define handoff_response_IrfLoader_h

#include <string>

namespace handoff_response {class IrfEval;}
namespace irfInterface { class Irfs;}

namespace handoff_response {
/**
 * @class IrfLoader
 *
 * @brief load irfs from parameter file, create an Irfs object

 This is a wrapper around an IrfEval object, which interprets a file containing the
 tabular information.
 *
 */

class IrfLoader {
public:
    /** @brief ctor 
        @param parameterfile file containing parameter tables
        @param eventclass name of event class to select
    */
    IrfLoader(const std::string & parameterfile, const std::string & eventclass);
    ~IrfLoader();

    /**@brief return pointer to an Irfs object
    @param index index to assign--should be unique, but not enforced
    */
    irfInterface::Irfs * irfs(int index=0);

private:

    handoff_response::IrfEval* m_irfeval;

};

}

#endif // handoff_IrfLoader_h
