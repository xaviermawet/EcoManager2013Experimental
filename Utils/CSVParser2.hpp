#ifndef CSVPARSER2_HPP
#define CSVPARSER2_HPP

#include "Exception.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

typedef std::vector<std::string> CSVRow;
typedef std::vector<CSVRow> CSVDataBase;

class CSVParser2
{
    public:

        CSVParser2(std::string const& filename, char cell_separator);

        void open(std::string const& filename, char cell_separator);
        void save(void) const;

        size_t  getRowCount(void) const;
        CSVRow  getHeader(void) const;
        CSVRow& getRow(int num_row);
        void    addRow(CSVRow const& row);
        void    eraseRow(int num_row);
        int     exist(std::string const& key, unsigned int num_column) const;
        int     exist(std::string const& key,
                      std::string const& columnName) const;

        std::string getColumnName(int indiceColumn) const;
        int getColumIndice(std::string const& columnName) const;

    protected:

        char cellSeparator;
        std::string filename;
        CSVDataBase csvContent;
};

#endif // CSVPARSER2_HPP
