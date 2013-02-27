#include "CSVParser.hpp"

using namespace std;

CSVParser::CSVParser(string const& filename, char cell_separator)
    : _fileName(filename), _cell_separator(cell_separator)
{
    ifstream file(this->_fileName.c_str(), ios::in);

    if (!file.is_open())
        throw Exception("Enable to open file to fill the data base");

    string line;
    string cell;
    CSVRow row;

    while(getline(file, line))
    {
        stringstream lineStream(line);

        while(getline(lineStream,cell,this->_cell_separator))
        {
            row.push_back(cell);
        }

        this->_csv_content.push_back(row);
        row.clear();
    }

    file.close();
}

CSVRow CSVParser::getHeader(void) const
{
    return this->getRow(0);
}

CSVRow CSVParser::getRow(unsigned int num_row) const
{
    //Signals if the requested position is out of range by throwing an exception
    return this->_csv_content.at(num_row);
}

int CSVParser::getRowCount(void) const
{
    return this->_csv_content.size();
}

void CSVParser::addRow(const CSVRow &row)
{
    this->appendToFile(row);
    this->_csv_content.push_back(row);
}

void CSVParser::appendToFile(CSVRow const& row) const
{
    ofstream file(this->_fileName.c_str(), ios::out | ios::app);

    if (!file.is_open())
        throw Exception("Enable to open csv file to save the row");

    for (unsigned int i = 0; i < row.size(); i++)
        file << row[i] << this->_cell_separator;
    file << endl;

    file.close();
}

void CSVParser::displayDataBase(void) const
{
    CSVRow currentRow;

    for (unsigned int i = 0; i < this->_csv_content.size(); i++)
    {
        currentRow = this->_csv_content[i];

        for (unsigned int j = 0; j < currentRow.size(); j++)
            cout << currentRow[j] << '\t';

        cout << endl;
    }
}

CSVParser::~CSVParser(void)
{

}
