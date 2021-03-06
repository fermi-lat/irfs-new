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
            bool newFile=true,
            size_t numRows=1);

   ~FitsFile() throw();

   void close();

   const std::vector<std::string> & fieldNames() const {
      return m_fieldNames;
   }

   void setVectorData(const std::string & fieldname,
                      const std::vector<double> & data,
                      size_t row=1);

   void setTableData(const std::string & fieldname,
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

   void setCbdValue(const std::string & cbdKey,
                    const std::string & cbdValue);

   const std::map<std::string, std::string> & cbdValues() const {
      return m_cbdValues;
   }

private:

   fitsfile * m_fptr;

   size_t m_numRows;

   std::string m_outfile;

   std::string m_extname;

   std::vector<std::string> m_fieldNames;

   std::string m_tdim;

   std::map<std::string, std::string> m_cbdValues;

   void prepareFile(const std::string & outfile, 
                    const std::string & extname,
                    const std::string & templateFile,
                    bool newFile);

   int fieldNum(const std::string & fieldName) const;

   void readBoundaryKeywords(const tip::Table * table);

   void fitsReportError(int status, const std::string & routine) const;

   void setDateKeyword();

};

} // namespace handoff_response

#endif // handoff_response_FitsFile_h
