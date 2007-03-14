from esys.pyvisi import Scene, DataCollector, MapOnPlaneCut, Camera
from esys.pyvisi.constant import *

s = Scene(renderer = Renderer.ONLINE, num_viewport = 4, x_size = 1152, 
        y_size = 864)

# Create two data collector instances for two different sources.
dc1 = DataCollector(source = Source.XML)
dc1.setFileName(file_name = 
        "/home/jongui/trunk/pyvisi/test/python/data_data/interior_3D.xml")

dc2 = DataCollector(source = Source.XML)
dc2.setFileName(file_name = 
        "/home/jongui/data/laurent/subduction/source/function.0001.vtk")

# NOTE: There is a difference between (1) performing rotation followed by 
# translation and (2) performing translation followed by rotation.

# Create a map on plane cut instance for the first viewport.
mopc1 = MapOnPlaneCut(scene = s, data_collector = dc1, 
        viewport = Viewport.SOUTH_WEST)
mopc1.setPlaneToYZ(offset = 1.5)

c1 = Camera(scene = s, data_collector = dc1, viewport = Viewport.SOUTH_WEST)
c1.isometricView()

# Create three map on plane cut instances for the second viewport.
mopc2_1 = MapOnPlaneCut(scene = s, data_collector = dc1, 
        viewport = Viewport.NORTH_WEST)
mopc2_1.setPlaneToYZ(offset = 1.5)

mopc2_2 = MapOnPlaneCut(scene = s, data_collector = dc1, 
        viewport = Viewport.NORTH_WEST)
mopc2_2.setPlaneToXZ(offset = 1.5)

mopc2_3 = MapOnPlaneCut(scene = s, data_collector = dc1, 
        viewport = Viewport.NORTH_WEST)
mopc2_3.setPlaneToXY()
mopc2_3.translate(0,0,0.5)

c2 = Camera(scene = s, data_collector = dc2, viewport = Viewport.NORTH_WEST)
c2.isometricView()

# Create a map on plane cut instance for the third viewport.
mopc3 = MapOnPlaneCut(scene = s, data_collector = dc2, 
        viewport = Viewport.NORTH_EAST)
mopc3.setPlaneToXY()
mopc3.rotateX(angle = 89.9)

c3 = Camera(scene = s, data_collector = dc2, viewport = Viewport.NORTH_EAST)
c3.bottomView()
c3.azimuth(angle = -40)

# Create two map on plance cut instances for the fourth viewport.
mopc4_1 = MapOnPlaneCut(scene = s, data_collector = dc2, 
        viewport = Viewport.SOUTH_EAST)
mopc4_1.setPlaneToXZ()
mopc4_1.rotateZ(angle = -20)
mopc4_1.setOpacity(opacity = 0.8)

mopc4_2 = MapOnPlaneCut(scene = s, data_collector = dc2, 
        viewport = Viewport.SOUTH_EAST)
mopc4_2.setPlaneToXY()
mopc4_2.rotateY(angle = 20)
mopc4_2.setOpacity(opacity = 0.8)

c4 = Camera(scene = s, data_collector = dc2, viewport = Viewport.SOUTH_EAST)
c4.elevation(angle = -30)

s.render()
