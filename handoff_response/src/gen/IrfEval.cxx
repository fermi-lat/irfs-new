/** 
 * @file IrfEval.cxx
 * @brief implement class IrfEval
 *
 * $Header$
 */

#include "handoff_response/IrfEval.h"

using namespace handoff_response;

IrfEval::IrfEval(const std::string & eventclass)
  : m_type(eventclass), m_front(true) {
}
