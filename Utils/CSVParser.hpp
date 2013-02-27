#ifndef CSVPARSER_HPP
#define CSVPARSER_HPP

#include "Exception.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

typedef std::vector<std::string> CSVRow;
typedef std::vector<CSVRow> CSVDataBase;

class CSVParser
{
    public:

        CSVParser(std::string const& filename, char cell_separator);

        CSVRow getHeader(void) const;
        CSVRow getRow(unsigned int num_line) const;
        int    getRowCount(void) const;

        void   addRow(CSVRow const& row);
        void   displayDataBase(void) const;

        virtual ~CSVParser();

    protected:

        void   appendToFile(CSVRow const& row) const;

        std::string _fileName;
        char        _cell_separator;
        CSVDataBase _csv_content;
};

#endif // CSVPARSER_HPP
