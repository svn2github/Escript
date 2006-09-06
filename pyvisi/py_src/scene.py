"""
@author: John Ngui
@author: Lutz Gross
"""	

import vtk

class Scene:
	"""
	Class that defines a scene in which components are displayed.
	"""

	def __init__(self, renderer = "vtk-online", x_size = 800, y_size = 600):
		"""
		Initialize all the instance variables. 


		@type renderer: String
		@param renderer: Type of rendering (I{vtk-online or vtk-offline})		
		@type x_size: Number
		@param x_size: Size of the rendering window on the x-axis
		@type y_size: Number
		@param y_size: Size of the rendering window on the y-axis
		"""	

		self.renderer = renderer
		self.x_size = x_size
		self.y_size = y_size
		self.vtk_renderer = None
		self.vtk_render_window = None

		if(renderer == "vtk-online"):
			self.setRenderingWindow()

	def setRenderingWindow(self):
		"""
		Set up the renderer and rendering window.
		"""

		self.vtk_renderer = vtk.vtkRenderer()
		self.vtk_render_window = vtk.vtkRenderWindow()
		self.vtk_render_window.AddRenderer(self.vtk_renderer)
		self.vtk_render_window.SetSize(self.x_size, self.y_size)
		self.vtk_renderer.SetBackground(1, 1, 1) # Default color is white

	def render(self):
		""" 
		Set up the render window interactor and render the image.
		"""

		vtk_render_window_interactor = vtk.vtkRenderWindowInteractor()
		vtk_render_window_interactor.SetRenderWindow(self.vtk_render_window)
		vtk_render_window_interactor.Initialize()
		self.vtk_render_window.Render()
		vtk_render_window_interactor.Start()

	def getRenderer(self):
		"""
		Return the renderer.

		@rtype: vtkRenderer
		@return: VTK renderer 
		"""

		return self.vtk_renderer
		
		


	



