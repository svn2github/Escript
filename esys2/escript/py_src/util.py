# $Id$

## @file util.py

"""
@brief Utility functions for escript
"""

import numarray
import escript
#
#   escript constants (have to be consistent witj utilC.h 
#
UNKNOWN=-1
EPSILON=1.e-15
Pi=numarray.pi
# some solver options:
NO_REORDERING=0
MINIMUM_FILL_IN=1
NESTED_DISSECTION=2
# solver methods
DEFAULT_METHOD=0
DIRECT=1
CHOLEVSKY=2
PCG=3
CR=4
CGS=5
BICGSTAB=6
SSOR=7
ILU0=8
ILUT=9
JACOBI=10
GMRES=11
PRES20=12

METHOD_KEY="method"
SYMMETRY_KEY="symmetric"
TOLERANCE_KEY="tolerance"

# supported file formats:
VRML=1
PNG=2
JPEG=3
JPG=3
PS=4
OOGL=5
BMP=6
TIFF=7
OPENINVENTOR=8
RENDERMAN=9
PNM=10
#
# wrapper for various functions: if the argument has attribute the function name
# as an argument it calls the correspong methods. Otherwise the coresponsing numarray
# function is called.
#
# functions involving the underlying Domain:
#
def grad(arg,where=None):
    """
    @brief returns the spatial gradient of the Data object arg

    @param arg: Data object representing the function which gradient to be calculated.
    @param where: FunctionSpace in which the gradient will be. If None Function(dom) where dom is the
                  domain of the Data object arg.
    """
    if where==None:
       return arg.grad()
    else:
       return arg.grad(where)

def integrate(arg):
    """
    @brief return the integral if the function represented by Data object arg over its domain.

    @param arg
    """
    return arg.integrate()

def interpolate(arg,where):
    """
    @brief interpolates the function represented by Data object arg into the FunctionSpace where.

    @param arg
    @param where
    """
    return arg.interpolate(where)

# functions returning Data objects:

def transpose(arg,axis=None):
    """
    @brief returns the transpose of the Data object arg. 

    @param arg
    """
    if isinstance(arg,escript.Data):
       if axis==None: axis=arg.getRank()/2
       return arg.transpose(axis)
    else:
       if axis==None: axis=arg.rank/2
       return numarray.transpose(arg,axis=axis)

def trace(arg):
    """
    @brief

    @param arg
    """
    if isinstance(arg,escript.Data):
       return arg.trace()
    else:
       return numarray.trace(arg)

def exp(arg):
    """
    @brief

    @param arg
    """
    if isinstance(arg,escript.Data):
       return arg.exp()
    else:
       return numarray.exp(arg)

def sqrt(arg):
    """
    @brief

    @param arg
    """
    if isinstance(arg,escript.Data):
       return arg.sqrt()
    else:
       return numarray.sqrt(arg)

def sin(arg):
    """
    @brief

    @param arg
    """
    if isinstance(arg,escript.Data):
       return arg.sin()
    else:
       return numarray.sin(arg)

def tan(arg):
    """
    @brief

    @param arg
    """
    if isinstance(arg,escript.Data):
       return arg.tan()
    else:
       return numarray.tan(arg)

def cos(arg):
    """
    @brief

    @param arg
    """
    if isinstance(arg,escript.Data):
       return arg.cos()
    else:
       return numarray.cos(arg)

def maxval(arg):
    """
    @brief

    @param arg
    """
    if isinstance(arg,escript.Data):
       return arg.maxval()
    else:
       return arg.max()

def minval(arg):
    """
    @brief

    @param arg
    """
    if isinstance(arg,escript.Data):
       return arg.minval()
    else:
       return arg.max()

def length(arg):
    """
    @brief

    @param arg
    """
    if isinstance(arg,escript.Data):
       return arg.length()
    else:
       return sqrt((arg**2).sum())

def sign(arg):
    """
    @brief

    @param arg
    """
    if isinstance(arg,escript.Data):
       return arg.sign()
    else:
       return numarray.greater(arg,numarray.zeros(arg.shape))-numarray.less(arg,numarray.zeros(arg.shape))

# reduction operations:

def sum(arg):
    """
    @brief

    @param arg
    """
    return arg.sum()

def sup(arg):
    """
    @brief

    @param arg
    """
    if isinstance(arg,escript.Data):
       return arg.sup()
    else:
       return arg.max()

def inf(arg):
    """
    @brief

    @param arg
    """
    if isinstance(arg,escript.Data):
       return arg.inf()
    else:
       return arg.min()

def L2(arg):
    """
    @brief returns the L2-norm of the 

    @param arg
    """
    return arg.L2()

def Lsup(arg):
    """
    @brief

    @param arg
    """
    if isinstance(arg,escript.Data):
       return arg.Lsup()
    else:
       return arg.max(numarray.abs(arg))

def dot(arg1,arg2):
    """
    @brief

    @param arg
    """
    if isinstance(arg1,escript.Data):
       return arg1.dot(arg2)
    elif isinstance(arg1,escript.Data):
       return arg2.dot(arg1)
    else:
       return numarray.dot(arg1,arg2)
