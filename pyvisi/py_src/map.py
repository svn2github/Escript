"""
@author: John NGUI
"""

import vtk
from mapper import DataSetMapper
from actor import Actor3D
from lookuptable import LookupTable
from outline import Outline
from constant import Viewport, Color, Lut

# NOTE: DataSetMapper and Actor3D were inherited to allow access to their 
# public methods from the driver.
class Map(DataSetMapper, Actor3D):
	"""
	Class that shows a scalar field on a domain surface.
	"""

	# The SOUTH_WEST default viewport is used when there is only one viewport.
	# This saves the user from specifying the viewport when there is only one.
	# If no scalar field is specified, the first encountered in the file will
	# be loaded automatically. If no lut is specified, the color scheme will
	# be used.
	def __init__(self, scene, data_collector, scalar = None, 
			viewport = Viewport.SOUTH_WEST, lut = Lut.COLOR, 
			outline = True):
		"""
		@type scene: L{Scene <scene.Scene>} object
		@param scene: Scene in which objects are to be rendered on
		@type data_collector: L{DataCollector <datacollector.DataCollector>}
				object
		@param data_collector: Deal with source of data for visualisation
		@type scalar: String
		@param scalar: Scalar field to load from the source file
		@type viewport: L{Viewport <constant.Viewport>} constant  
		@param viewport: Viewport in which objects are to be rendered on 
		@type lut : L{Lut <constant.Lut>} constant
		@param lut: Lookup table color scheme
		@type outline: Boolean
		@param outline: Places an outline around the domain surface
		"""

		# NOTE: Actor3D is inherited and there are two instances declared here.
		# As a result, when methods from Actor3D is invoked from the driver,
		# only the methods associated with the latest instance (which in this
		# case is the Actor3D for the  map) can be executed. Actor3D 
		# methods associated with Outline cannot be invoked from the driver. 
		# They can only be called within here, which is why Outline must
		# be place before map as there is unlikely to be any changes
		# made to the Outline's Actor3D.

		# ----- Outline -----

		if(outline == True):
			outline = Outline(data_collector._getOutput())
			DataSetMapper.__init__(self, outline._getOutput())

			Actor3D.__init__(self, DataSetMapper._getDataSetMapper(self))
			# Default outline color is black.
			Actor3D.setColor(self, Color.BLACK)

			# Default line width is 1.
			Actor3D._setLineWidth(self, 1)
			scene._addActor3D(viewport, Actor3D._getActor3D(self))

		# ----- Map -----

		#if(scalar != None): # True only if a scalar field was specified.
		#	data_collector._setActiveScalar(scalar)

		# NOTE: Lookup table color mapping (color or grey scale) MUST be set
		# before DataSetMapper. If it is done after DataSetMapper, no effect
		# will take place.
		if(lut == Lut.COLOR): # Colored lookup table.
			lookup_table = LookupTable() 
			lookup_table._setTableValue()
		elif(lut == Lut.GREY_SCALE): # Grey scaled lookup table.
			lookup_table = LookupTable() 
			lookup_table._setLookupTableToGreyScale()

		DataSetMapper.__init__(self, data_collector._getOutput(), 
				lookup_table._getLookupTable())	
		DataSetMapper._setScalarRange(self, data_collector._getScalarRange())


		Actor3D.__init__(self, DataSetMapper._getDataSetMapper(self))
		scene._addActor3D(viewport, Actor3D._getActor3D(self))


###############################################################################


from transform import Transform
from plane import Plane
from cutter import Cutter

# NOTE: DataSetMapper, Actor3D, Transform, Plane and Cutter were inherited 
# to allow access to their public methods from the driver.
class MapOnPlaneCut(DataSetMapper, Actor3D, Transform, Plane, Cutter):
	"""
	Class that show a scalar field on a plane.	
	"""
	
	# The SOUTH_WEST default viewport is used when there is only one viewport.
	# This saves the user from specifying the viewport when there is only one.
	# If no scalar field is specified, the first encountered in the file will
	# be loaded automatically. If no lut is specified, the color scheme will
	# be used.
	def __init__(self, scene, data_collector, scalar = None, 
			viewport = Viewport.SOUTH_WEST, lut = Lut.COLOR, outline = True):

		"""
		@type scene: L{Scene <scene.Scene>} object
		@param scene: Scene in which objects are to be rendered on
		@type data_collector: L{DataCollector <datacollector.DataCollector>}
				object
		@param data_collector: Deal with source of data for visualisation
		@type scalar: String
		@param scalar: Scalar field to load from the source file
		@type viewport: L{Viewport <constant.Viewport>} constant
		@param viewport: Viewport in which objects are to be rendered on
		@type lut : L{Lut <constant.Lut>} constant
		@param lut: Lookup table color scheme 
		@type outline: Boolean
		@param outline: Places an outline around the domain surface
		"""

		# NOTE: Actor3D is inherited and there are two instances declared here.
		# As a result, when methods from Actor3D is invoked from the driver,
		# only the methods associated with the latest instance (which in this
		# case is the Actor3D for the map) can be executed. Actor3D
		# methods associated with Outline cannot be invoked from the driver.
		# They can only be called within here, which is why Outline must
		# be place before the map as there is unlikely to be any changes
		# made to the Outline's Actor3D.

 		# ----- Outline -----

		if(outline == True):
			outline = Outline(data_collector._getOutput())
			DataSetMapper.__init__(self, outline._getOutput())

			Actor3D.__init__(self, DataSetMapper._getDataSetMapper(self))
			# Default outline color is black.
			Actor3D.setColor(self, Color.BLACK)
			# Default line width is 1.

			Actor3D._setLineWidth(self, 1)
			scene._addActor3D(viewport, Actor3D._getActor3D(self))

		# ----- Map on a plane -----

		#if(scalar != None):
		#	data_collector._setActiveScalar(scalar)

		# NOTE: Lookup table color mapping (color or grey scale) MUST be set
		# before DataSetMapper. If it is done after DataSetMapper, no effect
		# will take place.
		if(lut == Lut.COLOR): # Colored lookup table.
			lookup_table = LookupTable() 
			lookup_table._setTableValue()
		elif(lut == Lut.GREY_SCALE): # Grey scaled lookup table.
			lookup_table = LookupTable() 
			lookup_table._setLookupTableToGreyScale()

		Transform.__init__(self)	
		Plane.__init__(self, Transform._getTransform(self))

		Cutter.__init__(self, data_collector._getOutput(), 
				Plane._getPlane(self))

		DataSetMapper.__init__(self, Cutter._getOutput(self), 
				lookup_table._getLookupTable())
		DataSetMapper._setScalarRange(self, data_collector._getScalarRange())	

		Actor3D.__init__(self, DataSetMapper._getDataSetMapper(self))
		scene._addActor3D(viewport, Actor3D._getActor3D(self))


###########################################################################


from clipper import Clipper

# NOTE: DataSetMapper, Actor3D, Transform, Plane and Clipper were inherited 
# to allow access to their public methods from the driver.
class MapOnPlaneClip(DataSetMapper, Actor3D, Transform, Plane, Clipper):
	"""
	Class that show a scalar field on a clipped plane.	
	"""
	
	# The SOUTH_WEST default viewport is used when there is only one viewport.
	# This saves the user from specifying the viewport when there is only one.
	# If no scalar field is specified, the first encountered in the file will
	# be loaded automatically. If no lut is specified, the color scheme will
	# be used.
	def __init__(self, scene, data_collector, scalar = None, 
			viewport = Viewport.SOUTH_WEST, lut = Lut.COLOR, outline = True):

		"""
		@type scene: L{Scene <scene.Scene>} object
		@param scene: Scene in which objects are to be rendered on
		@type data_collector: L{DataCollector <datacollector.DataCollector>}
				object
		@param data_collector: Deal with source of data for visualisation
		@type scalar: String
		@param scalar: Scalar field to load from the source file
		@type viewport: L{Viewport <constant.Viewport>} constant
		@param viewport: Viewport in which objects are to be rendered on
		@type lut : L{Lut <constant.Lut>} constant
		@param lut: Lookup table color scheme
		@type outline: Boolean
		@param outline: Places an outline around the domain surface
		"""

		# NOTE: Actor3D is inherited and there are two instances declared here.
		# As a result, when methods from Actor3D is invoked from the driver,
		# only the methods associated with the latest instance (which in this
		# case is the Actor3D for the map) can be executed. Actor3D
		# methods associated with Outline cannot be invoked from the driver.
		# They can only be called within here, which is why Outline must
		# be place before the map as there is unlikely to be any changes
		# made to the Outline's Actor3D.

 		# ----- Outline -----

		if(outline == True):
			outline = Outline(data_collector._getOutput())
			DataSetMapper.__init__(self, outline._getOutput())

			Actor3D.__init__(self, DataSetMapper._getDataSetMapper(self))
			# Default outline color is black.
			Actor3D.setColor(self, Color.BLACK)

			# Default line width is 1.
			Actor3D._setLineWidth(self, 1)
			scene._addActor3D(viewport, Actor3D._getActor3D(self))

		# ----- Map on a clipped plane -----

		#if(scalar != None):
		#	data_collector._setActiveScalar(scalar)

		# NOTE: Lookup table color mapping (color or grey scale) MUST be set
		# before DataSetMapper. If it is done after DataSetMapper, no effect
		# will take place.
		if(lut == Lut.COLOR): # Colored lookup table.
			lookup_table = LookupTable() 
			lookup_table._setTableValue()
		elif(lut == Lut.GREY_SCALE): # Grey scaled lookup table.
			lookup_table = LookupTable() 
			lookup_table._setLookupTableToGreyScale()

		Transform.__init__(self)	
		Plane.__init__(self, Transform._getTransform(self))

		Clipper.__init__(self, data_collector._getOutput(), 
				Plane._getPlane(self))
		Clipper._setClipFunction(self)

		DataSetMapper.__init__(self, Clipper._getOutput(self), 
				lookup_table._getLookupTable())
		DataSetMapper._setScalarRange(self, data_collector._getScalarRange())	

		Actor3D.__init__(self, DataSetMapper._getDataSetMapper(self))
		scene._addActor3D(viewport, Actor3D._getActor3D(self))


#############################################################################


# NOTE: DataSetMapper, Actor3D and Clipper were inherited 
# to allow access to their public methods from the driver.
class MapOnScalarClip(DataSetMapper, Actor3D, Clipper):
	"""
	Class that show a scalar field clipped using a scalar value.	
	"""
	
	# The SOUTH_WEST default viewport is used when there is only one viewport.
	# This saves the user from specifying the viewport when there is only one.
	# If no scalar field is specified, the first encountered in the file will
	# be loaded automatically. If no lut is specified, the color scheme will
	# be used.

	def __init__(self, scene, data_collector, scalar = None, 
			viewport = Viewport.SOUTH_WEST, lut = Lut.COLOR, outline = True):

		"""
		@type scene: L{Scene <scene.Scene>} object
		@param scene: Scene in which objects are to be rendered on
		@type data_collector: L{DataCollector <datacollector.DataCollector>}
				object
		@param data_collector: Deal with source of data for visualisation
		@type scalar: String
		@param scalar: Scalar field to load from the source file
		@type viewport: L{Viewport <constant.Viewport>} constant
		@param viewport: Viewport in which objects are to be rendered on
		@type lut : L{Lut <constant.Lut>} constant
		@param lut: Lookup table color scheme
		@type outline: Boolean
		@param outline: Places an outline around the domain surface
		"""

		# NOTE: Actor3D is inherited and there are two instances declared here.
		# As a result, when methods from Actor3D is invoked from the driver,
		# only the methods associated with the latest instance (which in this
		# case is the Actor3D for the map) can be executed. Actor3D
		# methods associated with Outline cannot be invoked from the driver.
		# They can only be called within here, which is why Outline must
		# be place before the map as there is unlikely to be any changes
		# made to the Outline's Actor3D.

 		# ----- Outline -----

		if(outline == True):
			outline = Outline(data_collector._getOutput())
			DataSetMapper.__init__(self, outline._getOutput())

			Actor3D.__init__(self, DataSetMapper._getDataSetMapper(self))
			# Default outline color is black.
			Actor3D.setColor(self, Color.BLACK)

			# Default line width is 1.
			Actor3D._setLineWidth(self, 1)
			scene._addActor3D(viewport, Actor3D._getActor3D(self))

		# ----- Map clipped using a scalar value -----

		#if(scalar != None):
		#	data_collector._setActiveScalar(scalar)

		# NOTE: Lookup table color mapping (color or grey scale) MUST be set
		# before DataSetMapper. If it is done after DataSetMapper, no effect
		# will take place.
		if(lut == Lut.COLOR): # Colored lookup table.
			lookup_table = LookupTable() 
			lookup_table._setTableValue()
		elif(lut == Lut.GREY_SCALE): # Grey scaled lookup table.
			lookup_table = LookupTable() 
			lookup_table._setLookupTableToGreyScale()

		# None is used because a plane is not required when a scalar value is 
		# used to perform the clipping.
		Clipper.__init__(self, data_collector._getOutput(), None)

		DataSetMapper.__init__(self, Clipper._getOutput(self), 
				lookup_table._getLookupTable())
		DataSetMapper._setScalarRange(self, data_collector._getScalarRange())	

		Actor3D.__init__(self, DataSetMapper._getDataSetMapper(self))
		scene._addActor3D(viewport, Actor3D._getActor3D(self))


