import App as App
import Blender
import sys
import FreeCAD
import Part
import mesh

FREECADPATH = 'F:/FreeCAD/bin'  # path to your FreeCAD.so or FreeCAD.dll file

sys.path.append(FREECADPATH)


shape = Part.Shape()
shape.read('my_shape.step')
doc = App.newDocument('Doc')
pf = doc.addObject("Part::Feature", "MyShape")
pf.Shape = shape
Mesh.export([pf], 'my_shape.stl')
