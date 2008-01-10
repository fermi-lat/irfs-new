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
 *
 */

class IrfLoader {
public:
    IrfLoader(const std::string & parameterfile, const std::string & eventclass);
    ~IrfLoader();

    irfInterface::Irfs * irfs(int index=0);

private:

    handoff_response::IrfEval* m_irfeval;

};

}

#endif // handoff_IrfLoader_h
