# Copyright (C) 2004-2005 Paul Cochrane
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

# $Id: line_plot.py,v 1.1 2005/11/30 04:29:53 paultcochrane Exp $

"""
Class and functions associated with a pyvisi LinePlot objects (gnuplot)
"""

# generic imports
from pyvisi.renderers.gnuplot.common import debugMsg
import copy

# module specific imports
from pyvisi.renderers.gnuplot.plot import Plot

__revision__ = '$Revision: 1.1 $'

class LinePlot(Plot):
    """
    Line plot
    """
    def __init__(self, scene):
        """
        Initialisation of LinePlot class

        @param scene: the scene with which to associate the LinePlot
        @type scene: Scene object
        """
        debugMsg("Called LinePlot.__init__()")
        Plot.__init__(self, scene)

        self.renderer = scene.renderer

        self.title = None
        self.xlabel = None
        self.ylabel = None
        self.zlabel = None

        self.linestyle = 'lines'   # pyvisi-defined linestyle
        self._linestyle = None  # renderer-specific linestyle

        # is the LinePlot data offset (vertically) from each other?
        self.offset = False

        # now add the object to the scene
        scene.add(self)

    def setData(self, *dataList, **options):
        """
        Sets the data to the given plot object.

        @param dataList: list of data objects to plot
        @type dataList: tuple
        """
        debugMsg("Called setData() in LinePlot()")
        
        self.renderer.runString("# LinePlot.setData()")

        # grab the options if any
        if options.has_key('offset'):
            self.offset = options['offset']
        else:
            self.offset = False

        # do some sanity checking on the data
        for i in range(len(dataList)):
            if len(dataList[0]) != len(dataList[i]):
                raise ValueError, "Input vectors must all be the same length"

        # this is a really dodgy way to get the data into the renderer
        # I really have to find a better, more elegant way to do this
        
        # if have more than one array to plot, the first one is the x data
        if len(dataList) > 1:
            xData = dataList[0]
            ## pass around the x data
            self.renderer.renderDict['_x'] = copy.deepcopy(xData)
            # don't need the first element of the dataList, so get rid of it
            dataList = dataList[1:]
        # if only have one array input, then autogenerate xData
        elif len(dataList) == 1:
            xData = range(1, len(dataList[0])+1)
            if len(xData) != len(dataList[0]):
                errorString = "Autogenerated xData array length not "
                errorString += "equal to input array length"
                raise ValueError, errorString
                        
            ## pass around the x data
            self.renderer.renderDict['_x'] = copy.deepcopy(xData)

        # range over the data, printing what the expansion of the array is
        # and regenerate the data within the eval
        for i in range(len(dataList)):
            # check that the data here is a 1-D array
            if len(dataList[i].shape) != 1:
                raise ValueError, "Can only handle 1D arrays at present"

            yDataVar = "_y%d" % i
            self.renderer.renderDict[yDataVar] = copy.deepcopy(dataList[i])

        # if offset is true, then shift the data up accordingly
        if self.offset:
            # concatenate the data
            evalString = "_yAll = concatenate(["
            for i in range(len(dataList)-1):
                evalString += "_y%d," % i
            evalString += "_y%d])" % int(len(dataList)-1)
            self.renderer.runString(evalString)

            # find its min and max
            self.renderer.runString("_yMax = max(_yAll)")
            self.renderer.runString("_yMin = min(_yAll)")

            # keep the data apart a bit with a constant
            self.renderer.runString("_const = 0.1*(_yMax - _yMin)")

            # shift the data up
            self.renderer.runString("_shift = _yMax - _yMin + _const")

            for i in range(len(dataList)):
                evalString = "_y%d = _y%d + %d*_shift" % (i, i, i)
                self.renderer.runString(evalString)

        # give the data to gnuplot
        for i in range(len(dataList)):
            evalString = "_data%d = Gnuplot.Data(_x, " % i
            evalString += "_y%d" % i

            # if there are any linestyle settings etc, add them here (gnuplot
            # reasons)
            if self.linestyle is not None:
                # set the linestyle to renderer-specific version (_linestyle)
                self.setLineStyle(self.linestyle)
                evalString += ", with=\'%s\'" % self._linestyle

            # finish off the evalString
            evalString += ")"

            # and add it to the evalstack
            self.renderer.runString(evalString)

        # return the number of data objects to plot
        self.renderer.numDataObjects = len(dataList)

        return

    def render(self):
        """
        Does LinePlot object specific rendering stuff
        """
        debugMsg("Called LinePlot.render()")

        self.renderer.runString("# LinePlot.render()")

        # if a title is set, put it here
        if self.title is not None:
            evalString = "_gnuplot.title(\'%s\')" % self.title
            self.renderer.runString(evalString)

        # if an xlabel is set, add it
        if self.xlabel is not None:
            evalString = "_gnuplot.xlabel(\'%s\')" % self.xlabel
            self.renderer.runString(evalString)

        # if a ylabel is set, add it
        if self.ylabel is not None:
            evalString = "_gnuplot.ylabel(\'%s\')" % self.ylabel
            self.renderer.runString(evalString)

        # set up the evalString to use for plotting
        evalString = "_gnuplot.plot("
        for i in range(self.renderer.numDataObjects-1):
            evalString += "_data%d, " % i
        evalString += "_data%d)" % (self.renderer.numDataObjects-1,)
        self.renderer.runString(evalString)

        return

    def setLineStyle(self, linestyle):
        """
        Sets the linestyle of the LinePlot

        Linestyles may be either a word in the Gnuplot style, or a symbol 
        shortcut in the Matlab style.  Some of the options do not have a
        Matlab equivalent but do have a Gnuplot equivalent, or vice versa.

        What this method does, is take the linestyles possible as defined by
        PyVisi, and then does some conversion as best it can to get the
        relevant output from (in this case) gnuplot.
        
        Possible linestyles are:
            1. lines ('-')
            2. points ('o')
            3. linespoints ('-o')
            4. dots ('.')
            5. dotted (':')
            6. dashes ('--')
            7. dotdashes ('-.')

        @param linestyle: the style to use for the lines
        @type linestyle: string
        """
        debugMsg("Called LinePlot.setLineStyle()")

        # now implement the gnuplot-specific way to do this
        if linestyle == 'lines' or linestyle == '-':
            self._linestyle = 'lines'
        elif linestyle == 'points' or linestyle == 'o':
            self._linestyle = 'points'
        elif linestyle == 'linespoints' or linestyle == '-o':
            self._linestyle = 'linespoints'
        elif linestyle == 'dots' or linestyle == '.':
            self._linestyle = 'dots'
        elif linestyle == 'dotted' or linestyle == ':':
            print "linestyle = %s" % linestyle
            raise NotImplementedError, \
                    "Sorry, haven't implemented this style yet."
        elif linestyle == 'dashes' or linestyle == '--':
            print "linestyle = %s" % linestyle
            raise NotImplementedError, \
                    "Sorry, haven't implemented this style yet."
        elif linestyle == 'dotdashes' or linestyle == '-.':
            print "linestyle = %s" % linestyle
            raise NotImplementedError, \
                    "Sorry, haven't implemented this style yet."
        else:
            raise ValueError, "Unknown linestyle!  I got \'%s\'" % linestyle

        return

    def getLineStyle(self):
        """
        Gets the current linestyle of the LinePlot

        @return: the linestyle as a string
        """
        debugMsg("Called LinePlot.getLineStyle()")

        return self.linestyle


# vim: expandtab shiftwidth=4:
