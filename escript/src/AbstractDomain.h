// $Id$
/* 
 ************************************************************
 *          Copyright 2006 by ACcESS MNRF                   *
 *                                                          *
 *              http://www.access.edu.au                    *
 *       Primary Business: Queensland, Australia            *
 *  Licensed under the Open Software License version 3.0    *
 *     http://www.opensource.org/licenses/osl-3.0.php       *
 *                                                          *
 ************************************************************

*/

#if !defined escript_AbstractDomain_20040609_H
#define escript_AbstractDomain_20040609_H

#include "system_dep.h"

#include <string>
#include <boost/python/dict.hpp>

namespace escript {

//
// forward declarations
class Data;
//class AbstractSystemMatrix;
//class FunctionSpace;

/**
   \brief
   Base class for all escript domains.

   Description:
   Base class for all escript domains.
*/

class AbstractDomain {

 public:

  /**
     \brief
     Default constructor for AbstractDomain.

     Description:
     Default constructor for AbstractDomain. As the name suggests
     this is intended to be an abstract base class but by making it
     constructable we avoid a boost.python wrapper class. A call to 
     almost any of the base class functions will throw an exception
     as they are not intended to be used directly, but are overridden
     by the underlying solver package which escript is linked to.

     By default, this class is overridden by the class NullDomain.

     Preconditions:
     Describe any preconditions.

     Throws:
     Describe any exceptions thrown.
  */
  ESCRIPT_DLL_API
  AbstractDomain();

  /**
     \brief
     Destructor for AbstractDomain.

     Description:
     Destructor for AbstractDomain.
  */
  ESCRIPT_DLL_API
  virtual ~AbstractDomain();

  /**
     \brief
     Returns true if the given integer is a valid function space type
     for this domain.
  */
  ESCRIPT_DLL_API
  virtual bool isValidFunctionSpaceType(int functionSpaceType) const;

  /**
     \brief
     Return a description for this domain.
  */
  ESCRIPT_DLL_API
  virtual std::string getDescription() const;

  /**
     \brief
     Return a description for the given function space type code.
  */
  ESCRIPT_DLL_API
  virtual std::string functionSpaceTypeAsString(int functionSpaceType) const;

  /**
     \brief
      Returns the spatial dimension of the domain.

      This has to be implemented by the actual Domain adapter.
  */
  ESCRIPT_DLL_API
  virtual int getDim() const;

  /**
     \brief
     Return true if given domains are equal.
  */
  ESCRIPT_DLL_API
  virtual bool operator==(const AbstractDomain& other) const;
  ESCRIPT_DLL_API
  virtual bool operator!=(const AbstractDomain& other) const;

  /**
     \brief
     Writes the domain to an external file filename.

     This has to be implemented by the actual Domain adapter.
  */
  ESCRIPT_DLL_API
  virtual void write(const std::string& filename) const;

  /**
     \brief
     Return the number of data points per sample, and the number of samples as a pair.

     This has to be implemented by the actual Domain adapter.

     \param functionSpaceCode Input - Code for the function space type.
     \return pair, first - number of data points per sample, second - number of samples
  */
  ESCRIPT_DLL_API
  virtual std::pair<int,int> getDataShape(int functionSpaceCode) const;

  /**
     \brief
     Return the tag key for the given sample number.
     \param functionSpaceType Input - The function space type.
     \param sampleNo Input - The sample number.
  */
  ESCRIPT_DLL_API
  virtual int getTagFromSampleNo(int functionSpaceType, int sampleNo) const;

  /**
     \brief
     Return the reference number of the given sample number.
     \param functionSpaceType Input - The function space type.
     \param sampleNo Input - The sample number.
  */
  ESCRIPT_DLL_API
  virtual int getReferenceNoFromSampleNo(int functionSpaceType, int sampleNo) const;

  /**
     \brief
     Assigns new location to the domain.

     This has to be implemented by the actual Domain adapter.
  */
  ESCRIPT_DLL_API
  virtual void setNewX(const escript::Data& arg);

  /**
     \brief
     Interpolates data given on source onto target where source and target have to be given on the same domain.

     This has to be implemented by the actual Domain adapter.
  */
  ESCRIPT_DLL_API
  virtual void interpolateOnDomain(escript::Data& target,const escript::Data& source) const;
  ESCRIPT_DLL_API
  virtual bool probeInterpolationOnDomain(int functionSpaceType_source,int functionSpaceType_target) const;

  /**
     \brief
     Interpolates data given on source onto target where source and target are given on different domains.

     This has to be implemented by the actual Domain adapter.
  */
  ESCRIPT_DLL_API
  virtual void interpolateACross(escript::Data& target, const escript::Data& source) const;
  ESCRIPT_DLL_API
  virtual bool probeInterpolationACross(int functionSpaceType_source,const AbstractDomain& targetDomain, int functionSpaceType_target) const;

  /**
     \brief
     Returns locations in the domain. The function space is chosen appropriately.
  */
  ESCRIPT_DLL_API
  virtual escript::Data getX() const;

  /**
     \brief
     Return boundary normals. The function space is chosen appropriately.
  */
  ESCRIPT_DLL_API
  virtual escript::Data getNormal() const;

  /**
     \brief
     Returns the local size of samples. The function space is chosen appropriately.
  */
  ESCRIPT_DLL_API
  virtual escript::Data getSize() const;
  
  /**
     \brief
     Copies the location of data points on the domain into out.
     The actual function space to be considered
     is defined by out. out has to be defined on this.

     This has to be implemented by the actual Domain adapter.
  */
  ESCRIPT_DLL_API
  virtual void setToX(escript::Data& out) const;

  /**
     \brief
     Copies the surface normals at data points into out.
     The actual function space to be considered
     is defined by out. out has to be defined on this.

     This has to be implemented by the actual Domain adapter.
  */
  ESCRIPT_DLL_API
  virtual void setToNormal(escript::Data& out) const;

  /**
     \brief
     Copies the size of samples into out. The actual
     function space to be considered
     is defined by out. out has to be defined on this.

     This has to be implemented by the actual Domain adapter.
  */
  ESCRIPT_DLL_API
  virtual void setToSize(escript::Data& out) const;

  /**
     \brief
     Copies the gradient of arg into grad. The actual function space to be considered
     for the gradient is defined by grad. arg and grad have to be defined on this.

     This has to be implemented by the actual Domain adapter.
  */
  ESCRIPT_DLL_API
  virtual void setToGradient(escript::Data& grad, const escript::Data& arg) const;
  /**
     \brief
     Saves a dictonary of Data objects to an OpenDX input file. The keywords are used as identifier

     This has to be implemented by the actual Domain adapter.
  */
  ESCRIPT_DLL_API
  virtual void saveDX(const std::string& filename,const boost::python::dict& arg) const;

  /**
     \brief
     Saves a dictonary of Data objects to an VTK XML input file. The keywords are used as identifier

     This has to be implemented by the actual Domain adapter.
  */
  ESCRIPT_DLL_API
  virtual void saveVTK(const std::string& filename,const boost::python::dict& arg) const;

  /**
     \brief
     returns the function space representation of the type functionSpaceCode on this domain
     as a vtkObject.

     This has to be implemented by the actual Domain adapter.
  */
  //virtual vtkObject createVtkObject(int functionSpaceCode) const;

  /**
     \brief assigns new tag newTag to all samples of functionspace with a positive
     value of mask for any its sample point.

  */
  ESCRIPT_DLL_API
  virtual void setTags(const int functionSpaceType, const int newTag, const escript::Data& mask) const;

  /**
     \brief
     returns true if data on this domain and a function space of type functionSpaceCode has to
     considered as cell centered data.

     This has to be implemented by the actual Domain adapter.
  */
  ESCRIPT_DLL_API
  virtual bool isCellOriented(int functionSpaceCode) const;

  /**
     \brief
     Throw a standard exception. This function is called if any attempt 
     is made to use a base class function.
  */
  ESCRIPT_DLL_API
  void throwStandardException(const std::string& functionName) const;

 protected:

 private:

};

} // end of namespace

#endif
