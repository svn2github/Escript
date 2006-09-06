"""
@author: John Ngui
@author: Lutz Gross
"""

import vtk
from common import * 

class Map(Common):
	"""
	Class that shows a scalar field by a surface map. 
	"""

	def __init__(self, scene, data_collector, lut = None):
		"""
		@type scene: L{OpenScene <scene.Scene>} object
		@param scene: Scene in which components are to be added to
		@type data_collector: L{DataCollector <datacollector.DataCollector>}
			object
		@param data_collector: Source of data for visualization	
		@type lut: L{BlueToRed <colormap.BlueToRed>} object or 
			L{RedToBlue <colormap.RedToBlue>} object
		@param lut: Color lookup tabl to be used by the mapper
		"""

		Common.__init__(self, scene, data_collector)
		Common.setMapper(self, "self.data_collector.getReader().GetOutput()",
			lut)
		Common.setActor(self)
		Common.addActor(self)

"""
class MapOnPlane():
shows scalar data by color on a given plane
"""
pass
