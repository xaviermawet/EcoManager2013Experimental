#include "CSVParser2.hpp"

using namespace std;

CSVParser2::CSVParser2(string const& filename, char cell_separator)
{
    this->open(filename, cell_separator);
}

void CSVParser2::open(string const& filename, char cell_separator)
{
    this->filename = filename;
    this->cellSeparator = cell_separator;
    this->csvContent.clear();

    // Open file
    ifstream file(this->filename.c_str(), ios::in);
    if (!file.is_open())
        throw Exception("Enable to open file to fill the data base");


    // Read file
    string line;
    string cell;
    CSVRow row;

    while(getline(file, line))
    {
        stringstream lineStream(line);

        while(getline(lineStream,cell,this->cellSeparator))
        {
            row.push_back(cell);
        }

        this->csvContent.push_back(row);
        row.clear();
    }

    file.close();
}

void CSVParser2::save(void) const
{
    ofstream file(this->filename.c_str(), ios::out | ios::trunc);

    if (!file.is_open())
        throw Exception("Enable to open csv file to save the row");

    CSVRow row;
    for (unsigned i = 0; i < this->csvContent.size(); i++)
    {
        row = this->csvContent.at(i);
        for (unsigned int j = 0; j < row.size() - 1; j++)
            file << row.at(j) << this->cellSeparator;
        file << row.at(row.size() -1) << endl;
    }

    file.close();
}

size_t CSVParser2::getRowCount(void) const
{
    return this->csvContent.size();
}

CSVRow CSVParser2::getHeader(void) const
{
    return this->csvContent.at(0);
}

CSVRow& CSVParser2::getRow(int num_row)
{
    /* Signals if the requested position is
     * out of range by throwing an exception
     */
    return this->csvContent.at(num_row);
}

void CSVParser2::addRow(const CSVRow &row)
{
    // Add row to file
    ofstream file(this->filename.c_str(), ios::out | ios::app);

    if (!file.is_open())
        throw Exception("Enable to open csv file to save the row");

    for (unsigned int j = 0; j < row.size() - 1; j++)
        file << row.at(j) << this->cellSeparator;
    file << row.at(row.size() - 1) << endl;

    file.close();

    // Add row to the csv content
    this->csvContent.push_back(row);
}

void CSVParser2::eraseRow(int num_row)
{
    this->csvContent.erase(this->csvContent.begin() + num_row);
    this->save();
}

int CSVParser2::exist(const string &key, unsigned int num_column) const
{
    int row_num = -1;

    // browse all the lines
    for (unsigned int i = 0; i < this->csvContent.size(); i++)
    {
        if ((this->csvContent.at(i)).at(num_column) == key)
        {
            row_num = i;
            break;
        }
    }

    return row_num;
}

int CSVParser2::exist(const string &key, const string &columnName) const
{
    return this->exist(key, this->getColumIndice(columnName));
}

string CSVParser2::getColumnName(int indiceColumn) const
{
    CSVRow headers = this->getHeader();

    if (indiceColumn >= (int)headers.size())
        throw Exception("indice out of range");

    return headers.at(indiceColumn);
}

int CSVParser2::getColumIndice(const string &columnName) const
{
    CSVRow headers = this->getHeader();

    for(unsigned int i(0); i < headers.size(); i++)
        if (headers.at(i) == columnName)
            return i;

    throw Exception("column not found");
}
