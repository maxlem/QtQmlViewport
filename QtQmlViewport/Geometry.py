from . import BVH as PybindBVH
from . import PointsBVH as PybindPointsBVH
from QtQmlViewport import Product, utils
from QtQmlViewport.Array import Array

from OpenGL import GL as gl
from PyQt5.QtCore import QObject, Q_ENUMS, pyqtSlot as Slot
from PyQt5.QtGui import QVector3D
import numpy as np

class Attribs( Product.Product ):
    def __init__( self, parent=None, vertices = None, normals = None ):
        super(Attribs, self).__init__( parent )
        self.vertices = vertices
        self.normals = normals


    Product.InputProperty(vars(), Array, 'vertices', None)
    Product.InputProperty(vars(), Array, 'normals', None)

    def get_attributes(self):
        '''
        override this method to add all your attribs
        '''
        assert self._vertices.ndarray.dtype.type in [np.float32, np.float64], "not float32/64"
        assert len(self._vertices.ndarray.shape) == 2, "not 2d matrix"
        assert self._vertices.ndarray.shape[1] in [2,3], "not 2d or 3d"

        return {"vertices": self._vertices, "normals" : self._normals}

class PrimitiveType(QObject): #for Q_ENUMS: derive from QObject
    POINTS = gl.GL_POINTS
    LINES =  gl.GL_LINES
    LINE_STRIP = gl.GL_LINE_STRIP
    LINE_LOOP = gl.GL_LINE_LOOP
    TRIANGLES = gl.GL_TRIANGLES
    TRIANGLE_STRIP = gl.GL_TRIANGLE_STRIP
    TRIANGLE_FAN = gl.GL_TRIANGLE_FAN

class BVH( Product.Product ):

    def __init__( self, parent=None, indices=None, points=None, primitive_type = PrimitiveType.TRIANGLES):
        super(BVH, self).__init__( parent )

        self.indices = indices
        self.points = points
        self.primitiveType = primitive_type
        self.bvh = None
        self._shape_indices = None

    PrimitiveType = PrimitiveType

    Q_ENUMS(PrimitiveType)

    Product.InputProperty(vars(), int, 'primitiveType', PrimitiveType.TRIANGLES)

    Product.InputProperty(vars(), Array, 'indices', None)

    Product.InputProperty(vars(), Array, 'points', None)

    def _update(self):
        if self._indices is None or self._points is None:
            raise RuntimeError('indices or points is None')

        assert self._points.ndarray.shape[1] == 3, "points other than 3d not implemented"
        assert self._points.ndarray.dtype.type in [np.float32, np.float64], "not float32/64"
        assert self._indices.ndarray.dtype.type == np.uint32, 'BVH indices must be of type uint32'


        if self._primitiveType == PrimitiveType.TRIANGLES:
            self._shape_indices = self._indices.ndarray.reshape(self._indices.ndarray.shape[0]//3, 3, order = 'C')
            self.bvh = PybindBVH(self._shape_indices, self._points.ndarray.astype('f4'))
        elif self._primitiveType == PrimitiveType.POINTS:
            self._shape_indices = self._indices.ndarray.reshape(self._indices.ndarray.shape[0], 1, order = 'C')
            self.bvh = PybindPointsBVH(self._shape_indices, self._points.ndarray.astype('f4'))
        elif self._primitiveType == PrimitiveType.LINES:
            
            indices = Array(ndarray = np.array([0,1,2, 1,2,3, 0,4,2, 2,4,6, 1,5,3, 3,5,7, 4,5,6, 5,6,7, 2,3,6, 3,6,7], 'u4'))
            self._shape_indices = indices.ndarray.reshape(indices.ndarray.shape[0]//3, 3, order = 'C')
            self.bvh = PybindBVH(self._shape_indices, self._points.ndarray.astype('f4'))
        else:
            raise NotImplementedError()

class Geometry( Product.Product ):


    def __init__( self, parent=None, indices = None, attribs = None, primitive_type = PrimitiveType.TRIANGLES ):
        super(Geometry, self).__init__( parent )
        self.bvh = None

        self.indices = indices
        self.attribs = attribs
        self.primitiveType = primitive_type

    PrimitiveType = PrimitiveType

    Q_ENUMS(PrimitiveType)

    Product.InputProperty(vars(), int, 'primitiveType', PrimitiveType.TRIANGLES)

    Product.InputProperty(vars(), Array, 'indices', None)

    Product.InputProperty(vars(), Attribs, 'attribs', None)

    @Slot(int, QVector3D, str, result = QVector3D)
    def faceAttribtAt(self, id, tuv, attribute):
        face = self.faceIndices(id)
        va, vb, vc = getattr(self.attribs, attribute).ndarray[face]
        p = tuv[1] * va + tuv[2] * (vb + 1 - tuv[1] - tuv[2]) * vc
        return utils.from_numpy(p)

    @Slot(int, result = list)
    def faceIndices(self, id):
        assert self.primitiveType == PrimitiveType.TRIANGLES
        return self.indices.ndarray[id*3:id*3+3].tolist()

    @Slot(int, result = QVector3D)
    def faceNormal(self, id):
        face = self.faceIndices(id)
        va, vb, vc = self.attribs.vertices.ndarray[face]
        p = np.cross(vb-va, vc-va)
        return utils.from_numpy(p/np.linalg.norm(p))

    def goc_bvh(self, update = False):

        if self.bvh is None and self.primitiveType in [PrimitiveType.TRIANGLES, PrimitiveType.POINTS, PrimitiveType.LINES]:
            self.bvh = BVH(self, self.indices, self.attribs.vertices, self.primitiveType)
        if self.bvh is not None and update:
            self.bvh.update()
        return self.bvh



