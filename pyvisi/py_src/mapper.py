"""
@author: John NGUI
"""

import vtk

class DataSetMapper:
	"""
	Class that defines a data set mapper.
	"""

	# lookup_table = None is used only by the outline.
	def __init__(self, object, lookup_table = None):
		"""
		Initialise the data set mapper.

		@type object: vtkDataSet (i.e. vtkUnstructuredGrid, vtkPolyData, etc) 
		@param object: Data source map
		@type lookup_table: vtkLookupTable
		@param lookup_table: Maps scalar values to colors
		"""

		self.__object = object
		self.__vtk_data_set_mapper = vtk.vtkDataSetMapper()
		self.__setInput()

		if(lookup_table != None): # True for the outline.
			self.__setLookupTable(lookup_table)

	def __setInput(self):
		"""
		Set the input for the data set mapper.
		"""

		self.__vtk_data_set_mapper.SetInput(self.__object)

	def __setLookupTable(self, lookup_table):
		"""
		Set the lookup table to the data set mapper.
	
		@type lookup_table: vtkLookupTable
		@param lookup_table: Map scalar values to colors
		"""

		self.__vtk_data_set_mapper.SetLookupTable(lookup_table)

	def _setScalarRange(self, range):
		"""
		Set the minimum and maximum scalar range for the data set mapper.
		
		@type range: Two column tuple containing numbers
		@param range: Maximum and minimum data set mapper scalar range
		"""

		self.__vtk_data_set_mapper.SetScalarRange(range) 
	
	def _setScalarVisibilityOn(self):
		"""
		Scalar data is used to color the rendered object.
		"""

		self.__vtk_data_set_mapper.ScalarVisibilityOn()
	
	def _getDataSetMapper(self):
		"""
		Return the data set mapper.

		@rtype: vtkDataSetMapper 	
		@return: Data set mapper
		"""

		return self.__vtk_data_set_mapper

	
