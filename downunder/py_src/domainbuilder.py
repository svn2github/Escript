
##############################################################################
#
# Copyright (c) 2003-2013 by University of Queensland
# http://www.uq.edu.au
#
# Primary Business: Queensland, Australia
# Licensed under the Open Software License version 3.0
# http://www.opensource.org/licenses/osl-3.0.php
#
# Development until 2012 by Earth Systems Science Computational Center (ESSCC)
# Development since 2012 by School of Earth Sciences
#
##############################################################################

"""Domain construction from survey data for inversions"""

__copyright__="""Copyright (c) 2003-2013 by University of Queensland
http://www.uq.edu.au
Primary Business: Queensland, Australia"""
__license__="""Licensed under the Open Software License version 3.0
http://www.opensource.org/licenses/osl-3.0.php"""
__url__="https://launchpad.net/escript-finley"

__all__ = ['DomainBuilder']

import logging
import numpy as np
from esys.escript.util import *
from esys.escript import unitsSI as U
from esys.ripley import Brick, Rectangle
from .datasources import DataSource

class DomainBuilder(object):
    """
    This class is responsible for constructing an escript Domain object with
    suitable extents and resolution for survey data (`DataSource` objects)
    that is added to it.
    """

    def __init__(self, dim=3):
        """
        Constructor.

        :param dim: Dimensionality (2 or 3) of the target domain.
                    This has implications for the survey data than can be
                    added. By default a 3D domain is created.
        :type dim: ``int``
        """
        self.logger = logging.getLogger('inv.%s'%self.__class__.__name__)
        if dim not in (2,3):
            raise ValueError("Number of dimensions must be 2 or 3")
        self.__domain=None
        self.__dim=dim
        self.__sources=[]
        self.__background_magnetic_field=None
        self.setPadding()
        self.setVerticalExtents()
        self.fixDensityBelow()
        self.fixSusceptibilityBelow()

    def addSource(self, source):
        """
        Adds a survey data provider to the domain builder.

        :param source: The data source to be added
        :type source: `DataSource`
        """
        if self.__domain is not None:
            raise RuntimeError("Invalid call to addSource(). Domain is already built.")
        if not isinstance(source, DataSource):
            raise TypeError("source is not a DataSource")
        self.__sources.append(source)

    def setFractionalPadding(self, pad_x=None, pad_y=None):
        """
        Sets the amount of padding around the dataset as a fraction of the
        dataset side lengths.

        For example, calling ``setFractionalPadding(0.2, 0.1)`` with a data
        source of size 10x20 will result in the padded data set size
        14x24 (10*(1+2*0.2), 20*(1+2*0.1))

        :param pad_x: Padding per side in x direction (default: no padding)
        :type pad_x: ``float``
        :param pad_y: Padding per side in y direction (default: no padding)
        :type pad_y: ``float``
        :note: `pad_y` is ignored for 2-dimensional domains.
        """
        if self.__domain is not None:
            raise RuntimeError("Invalid call to setFractionalPadding(). Domain is already built.")
        if pad_x is not None:
            if pad_x < 0:
                raise ValueError("setFractionalPadding: Arguments must be non-negative")
            if pad_x > 10:
                raise ValueError("setFractionalPadding: Argument too large")
        if pad_y is not None:
            if pad_y < 0:
                raise ValueError("setFractionalPadding: Arguments must be non-negative")
            if pad_y > 10:
                raise ValueError("setFractionalPadding: Argument too large")
        self._padding = [pad_x,pad_y], 'f'
        
    def setPadding(self, pad_x=None, pad_y=None):
        """
        Sets the amount of padding around the dataset in absolute length units.

        The final domain size will be the length in x (in y) of the dataset
        plus twice the value of `pad_x` (`pad_y`). The arguments must be
        non-negative.

        :param pad_x: Padding per side in x direction (default: no padding)
        :type pad_x: ``float``
        :param pad_y: Padding per side in y direction (default: no padding)
        :type pad_y: ``float``
        :note: `pad_y` is ignored for 2-dimensional domains.
        """
        if self.__domain is not None:
            raise RuntimeError("Invalid call to setPadding(). Domain is already built.")
        if pad_x is not None:
            if pad_x < 0:
                raise ValueError("setPadding: Arguments must be non-negative")
        if pad_y is not None:
            if pad_y < 0:
                raise ValueError("setPadding: Arguments must be non-negative")
        self._padding = [pad_x,pad_y], 'l'

    def setElementPadding(self, pad_x=None, pad_y=None):
        """
        Sets the amount of padding around the dataset in number of elements
        (cells).

        When the domain is constructed `pad_x` (`pad_y`) elements are added
        on each side of the x- (y-) dimension. The arguments must be
        non-negative.

        :param pad_x: Padding per side in x direction (default: no padding)
        :type pad_x: ``int``
        :param pad_y: Padding per side in y direction (default: no padding)
        :type pad_y: ``int``
        :note: `pad_y` is ignored for 2-dimensional datasets.
        """
        if self.__domain is not None:
            raise RuntimeError("Invalid call to setElementPadding(). Domain is already built.")
        if pad_x is not None:
            if type(pad_x) is not int:
                raise TypeError("setElementPadding expects integer arguments")
            if pad_x < 0:
                raise ValueError("setElementPadding: Arguments must be non-negative")
        if pad_y is not None:
            if type(pad_y) is not int:
                raise TypeError("setElementPadding expects integer arguments")
            if pad_y < 0:
                raise ValueError("setElementPadding: Arguments must be non-negative")
        self._padding = [pad_x,pad_y], 'e'
        
    def getGravitySurveys(self):
        """
        Returns a list of gravity surveys, see `getSurveys` for details.
        """
        return self.getSurveys(DataSource.GRAVITY)
        
    def getMagneticSurveys(self):
        """
        Returns a list of magnetic surveys, see `getSurveys` for details.
        """
        return self.getSurveys(DataSource.MAGNETIC)
        
    def fixDensityBelow(self, depth=None):
        """
        Defines the depth below which the density anomaly is set to zero.
        """
        self.__fix_density_below=depth
        
    def fixSusceptibilityBelow(self, depth=None):
        """
        Defines the depth below which the susceptibility anomaly is set to
        zero.
        """
        self.__fix_susceptibility_below=depth

    def getSurveys(self, datatype):
        """
        Returns a list of `Data` objects for all surveys of type `datatype`
        available to this domain builder.

        :return: List of surveys which are tuples (anomaly,error).
        :rtype: ``list``
        """
        surveys=[]
        for src in self.__sources:
            if src.getDataType()==datatype:
                surveys.append(src.getSurveyData(self.getDomain(), self._dom_origin, self._dom_NE, self._spacing))
        return surveys

    def setBackgroundMagneticFluxDensity(self, B):
        """
        Sets the background magnetic flux density B=(B_r, B_theta, B_phi)
        """
        self.__background_magnetic_field=B

    def getBackgroundMagneticFluxDensity(self):
        """
        Returns the background magnetic flux density.
        """
        B = self.__background_magnetic_field
        # this is for Cartesian (FIXME ?)
        if self.__dim<3:
            return np.array([-B[2], -B[0]])
        else:
            return np.array([-B[1], -B[2], -B[0]])

    def getSetDensityMask(self):
        """
        Returns the density mask data object which is non-zero for cells
        whose density value is fixed, zero otherwise.
        """
        z=self.getDomain().getX()[self.__dim-1]
        m = whereNonNegative(z)
        if self.__fix_density_below:
            m += whereNonPositive(z+self.__fix_density_below)
        return m

    def getSetSusceptibilityMask(self):
        """
        Returns the susceptibility mask data object which is non-zero for
        cells whose susceptibility value is fixed, zero otherwise.
        """
        z=self.getDomain().getX()[self.__dim-1]
        m = whereNonNegative(z)
        if self.__fix_susceptibility_below:
            m += whereNonPositive(z+self.__fix_susceptibility_below)
        return m

    def getDomain(self):
        """
        Returns a domain that spans the data area plus padding.

        The domain is created the first time this method is called,
        subsequent calls return the same domain so anything that affects
        the domain (such as padding) needs to be set beforehand.

        :return: The escript domain for this data source
        :rtype: `esys.escript.Domain`
        """
        if self.__domain is None:
            self.__domain=self.__createDomain()
        return self.__domain

    def setVerticalExtents(self, depth=40000., air_layer=10000., num_cells=25):
        """
        This method sets the target domain parameters for the vertical
        dimension.

        :param depth: Depth of the domain (in meters)
        :type depth: ``float``
        :param air_layer: Depth of the layer above sea level (in meters)
        :type air_layer: ``float``
        :param num_cells: Number of domain elements for the entire vertical
                          dimension
        :type num_cells: ``int``
        """
        if self.__domain is not None:
            raise RuntimeError("Invalid call to setVerticalExtents(). Domain is already built.")
        self._v_depth=depth
        self._v_air_layer=air_layer
        self._v_num_cells=num_cells

    def __getTotalExtentsWithPadding(self):
        """
        Helper method that computes origin and number of elements
        after adding padding to the bounding box of all available survey data.
        """
        X0, NX, DX = self.__getTotalExtents()
        DIM=len(X0)
        frac=[]
        # padding is applied to each side so multiply by 2 to get the total
        # amount of padding per dimension
        pad, pt = self._padding
        for i in range(DIM):
            if pad[i] is None:
                frac.append(0.)
                continue
            if pt == 'f': # fraction of side length
                frac.append(2.*pad[i])
            elif pt == 'e': # number of elements
                frac.append(2.*pad[i]/float(NX[i]))
            else: # absolute length
                f=pad[i]/DX[i]
                frac.append(2.*f/float(NX[i]))

        # calculate new number of elements
        NX_padded=[int(round(NX[i]*(1+frac[i]))) for i in range(DIM)]
        NXdiff=[NX_padded[i]-NX[i] for i in range(DIM)]
        X0_padded=[X0[i]-NXdiff[i]/2.*DX[i] for i in range(DIM)]
        return X0_padded, NX_padded, DX

    def __getTotalExtents(self):
        """
        Helper method that computes the origin, number of elements and
        minimal element spacing taking into account all available survey data.
        """
        if len(self.__sources)==0:
            raise ValueError("No data")
        X0, NE, DX = self.__sources[0].getDataExtents()
        XN=[X0[i]+NE[i]*DX[i] for i in range(len(NE))]

        for src in self.__sources[1:]:
            d_x0, d_ne, d_dx = src.getDataExtents()
            for i in range(len(d_x0)):
                X0[i]=min(X0[i], d_x0[i])
            for i in range(len(d_dx)):
                DX[i]=min(DX[i], d_dx[i])
            for i in range(len(d_ne)):
                XN[i]=max(XN[i], d_x0[i]+d_ne[i]*d_dx[i])
        NE=[int((XN[i]-X0[i])/DX[i]) for i in range(len(XN))]
        return X0, NE, DX

    def __createDomain(self):
        """
        Creates and returns an escript domain that spans the entire area of
        available data plus a padding zone. This method is called only once
        the first time `getDomain()` is invoked.

        :return: The escript domain
        :rtype: `esys.escript.Domain`
        """
        X0, NX, DX = self.__getTotalExtentsWithPadding()

        # number of domain elements
        NE = NX + [self._v_num_cells]

        # origin of domain
        origin = X0 + [-self._v_depth]
        self._dom_origin = [np.floor(oi) for oi in origin]

        # cell size / point spacing
        spacing = DX + [(self._v_depth+self._v_air_layer)/self._v_num_cells]
        self._spacing = [float(np.floor(si)) for si in spacing]

        lo=[(self._dom_origin[i], self._dom_origin[i]+NE[i]*self._spacing[i]) for i in range(self.__dim)]
        if self.__dim==3:
            dom=Brick(*NE, l0=lo[0], l1=lo[1], l2=lo[2])
        else:
            dom=Rectangle(*NE, l0=lo[0], l1=lo[1])

        # ripley may internally adjust NE and length, so recompute
        self._dom_len=[sup(dom.getX()[i])-inf(dom.getX()[i]) for i in range(self.__dim)]
        self._dom_NE=[int(self._dom_len[i]/self._spacing[i]) for i in range(self.__dim)]

        self.logger.debug("Domain size: "+str(self._dom_NE))
        self.logger.debug("     length: "+str(self._dom_len))
        self.logger.debug("     origin: "+str(self._dom_origin))
        return dom

