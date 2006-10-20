/** @file IrfEval.cxx
@brief implement class IrfEval

$Header$
*/

#include "handoff_response/IrfEval.h"
#include <stdexcept>

using namespace handoff_response;

IrfEval::IrfEval(const std::string & eventclass)
: m_type(eventclass)
, m_front(eventclass.find("/front")!= std::string::npos)
{

    if( !m_front && eventclass.find("/back")==std::string::npos){
        throw std::invalid_argument("IrfEval expected name to include /front or /back: it is "+eventclass);

    }
}
