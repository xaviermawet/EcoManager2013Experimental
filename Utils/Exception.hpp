#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__

#include <exception>
#include <string>

class Exception : public std::exception
{
    public:

        Exception(std::string const& message);
        Exception(const Exception& exceptionToCopy);
        virtual Exception& operator=(const Exception& exceptionToCopy);
        virtual ~Exception() throw();

        virtual const char* what() const throw();

    protected:

        std::string _message;
};

#endif /* __EXCEPTION_HPP__ */
