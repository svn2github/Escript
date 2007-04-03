# Import the necessary modules
from esys.pyvisi import Scene, DataCollector, VelocityOnPlaneCut, Camera
from esys.pyvisi.constant import *

PYVISI_EXAMPLE_MESHES_PATH = "data_meshes/"
PYVISI_EXAMPLE_IMAGES_PATH = "data_sample_images/"
X_SIZE = 400
Y_SIZE = 400

VECTOR_FIELD_CELL_DATA = "velocity"
FILE_3D = "interior_3D.xml"
IMAGE_NAME = "velocity.jpg"
JPG_RENDERER = Renderer.ONLINE_JPG


# Create a Scene with four viewports
s = Scene(renderer = JPG_RENDERER, num_viewport = 1, x_size = X_SIZE, 
        y_size = Y_SIZE)

# Create a DataCollector reading from a XML file.
dc1 = DataCollector(source = Source.XML)
dc1.setFileName(file_name = PYVISI_EXAMPLE_MESHES_PATH + FILE_3D)
dc1.setActiveVector(vector = VECTOR_FIELD_CELL_DATA)

# Create Velocity cut on a plane.
vopc1 = VelocityOnPlaneCut(scene = s, data_collector = dc1, 
        viewport = Viewport.SOUTH_WEST, color_mode = ColorMode.VECTOR, 
        arrow = Arrow.THREE_D, lut = Lut.COLOR, cell_to_point = False, 
        outline = True)
vopc1.setScaleFactor(scale_factor = 0.5)
vopc1.setPlaneToXY(offset = 0.5)
vopc1.setRatio(2)
vopc1.randomOn()

# Create a Camera.
c1 = Camera(scene = s, data_collector = dc1, viewport = Viewport.SOUTH_WEST)
c1.isometricView()
c1.elevation(angle = -20)

# Render the object.
s.render(PYVISI_EXAMPLE_IMAGES_PATH + IMAGE_NAME)

