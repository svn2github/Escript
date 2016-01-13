
/*****************************************************************************
*
* Copyright (c) 2003-2016 by The University of Queensland
* http://www.uq.edu.au
*
* Primary Business: Queensland, Australia
* Licensed under the Open Software License version 3.0
* http://www.opensource.org/licenses/osl-3.0.php
*
* Development until 2012 by Earth Systems Science Computational Center (ESSCC)
* Development 2012-2013 by School of Earth Sciences
* Development from 2014 by Centre for Geoscience Computing (GeoComp)
*
*****************************************************************************/

#ifndef __Speckley_EXCEPTION_H__
#define __Speckley_EXCEPTION_H__

#include <speckley/system_dep.h>
#include <esysUtils/EsysException.h>

namespace speckley {

/**
   \brief
   SpeckleyException exception class.
*/
class Speckley_DLL_API SpeckleyException : public esysUtils::EsysException
{
protected:
    typedef EsysException Parent;

public:
    /**
       \brief
       Default constructor for the exception.
    */
    SpeckleyException() : Parent() { updateMessage(); }

    /**
       \brief
       Constructor with message.
    */
    SpeckleyException(const char *cstr) : Parent(cstr) { updateMessage(); }

    /**
       \brief
       Constructor with message.
    */
    SpeckleyException(const std::string &str) : Parent(str) { updateMessage(); }

    /**
       \brief
       Copy Constructor.
    */
    SpeckleyException(const SpeckleyException &other) : Parent(other)
    {
        updateMessage();
    }

    /// Destructor
    virtual ~SpeckleyException() THROW(NO_ARG) {}

    /**
       \brief
       Assignment operator.
    */
    inline SpeckleyException& operator=(const SpeckleyException &other ) THROW(NO_ARG)
    {
        Parent::operator=(other);
        updateMessage();
        return *this;
    }

    /**
       \brief
       Returns the name of the exception.
    */
    virtual const std::string& exceptionName() const;

private:
    //
    // the exception name is immutable and class-wide.
    static const std::string exceptionNameValue;
};

} // end of namespace speckley

#endif // __Speckley_EXCEPTION_H__

