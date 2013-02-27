#include "Exception.hpp"

using namespace std;

Exception::Exception(string const& message)
    : _message(message)
{
}

Exception::Exception(const Exception& exceptionToCopy)
    : _message(exceptionToCopy._message)
{
}

Exception& Exception::operator=(const Exception& exceptionToCopy)
{
    if(this != &exceptionToCopy)
        this->_message = exceptionToCopy._message;

    return *this;
}

Exception::~Exception() throw()
{
}

const char* Exception::what() const throw()
{
    return this->_message.c_str();
}
