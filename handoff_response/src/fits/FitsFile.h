/**
 * @file FitsFile.h
 * @brief Write out binary table data for handoff_response FITS files.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef handoff_response_FitsFile_h
#define handoff_response_FitsFile_h

#include <string>
#include <vector>

#include "fitsio.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

namespace handoff_response {

class IrfTable;

/**
 * @class FitsFile
 * @brief Write out binary table data for handoff_response FITS files.
 * @author J. Chiang
 *
 */ 

class FitsFile {

public: 

   FitsFile(const std::string & outfile, 
            const std::string & extname, 
            const std::string & templateFile,
            size_t numRows=1);

   ~FitsFile() throw();

   const std::vector<std::string> & fieldNames() const {
      return m_fieldNames;
   }

   void setVectorData(const std::string & fieldname,
                      const std::vector<double> & data,
                      size_t row=1);

   void setGrid(const IrfTable & table);

   void setGrid(const std::vector<double> & logEs,
                const std::vector<double> & mus);

   template<class Type>
   void setKeyword(const std::string & keyword,
                   const Type & value) const {
      tip::Table * hdu = 
         tip::IFileSvc::instance().editTable(m_outfile, m_extname);
      tip::Header & header = hdu->getHeader();
      header[keyword].set(value);
      delete hdu;
   }

private:

   fitsfile * m_fptr;

   size_t m_numRows;

   std::string m_outfile;

   std::string m_extname;

   std::vector<std::string> m_fieldNames;

   void createFile(const std::string & outfile, 
                   const std::string & extname,
                   const std::string & templateFile);

   int fieldNum(const std::string & fieldName) const;

   void fitsReportError(int status, const std::string & routine) const;

};

} // namespace handoff_response

#endif // handoff_response_FitsFile_h
