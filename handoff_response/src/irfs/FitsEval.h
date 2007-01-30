/**
 * @file FitsEval.h
 * @brief Interface class to access and evalute FITS versions of IRF tables.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef handoff_response_FitsEval_h
#define handoff_response_FitsEval_h

#include "RootEval.h"

namespace handoff_response {

/**
 * @class FitsEval
 * @brief Interface class to access and evalute FITS versions of IRF tables.
 *
 */

class FitsEval : public RootEval {

public:

   FitsEval(const std::string & className, const std::string & section);

   static void createMap(const std::string & className, 
                         std::map<std::string, IrfEval *> & evals);

   static void addToMap(const std::string & className, 
                        std::map<std::string, IrfEval *> & evals);

private:

   std::string m_className;
   std::string m_section;

   void readAeff();
   void readEdisp();
   void readPsf();

   std::string aeffFile() const;
   std::string edispFile() const;
   std::string psfFile() const;

};

} // namespace handoff_response

#endif // handoff_response_FitsEval_h
