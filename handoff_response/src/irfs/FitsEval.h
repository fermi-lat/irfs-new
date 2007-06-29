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

   FitsEval(const std::string & className, const std::string & section,
            const std::string & version="PASS4", 
            const std::string & date="2007-03-12",
            bool forceCaldb=false);

   virtual ~FitsEval();

   static void createMap(const std::string & className, 
                         std::map<std::string, IrfEval *> & evals);

   static void addToMap(const std::string & className, 
                        std::map<std::string, IrfEval *> & evals);

private:

   std::string m_className;
   std::string m_section;
   std::string m_version;
   std::string m_date;
   bool m_forceCaldb;

   void readAeff();
   void readEdisp();
   void readPsf();

   std::string aeffFile() const;
   std::string edispFile() const;
   std::string psfFile() const;

};

} // namespace handoff_response

#endif // handoff_response_FitsEval_h
