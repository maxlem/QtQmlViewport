from QtQmlViewport import Product, Transforms

from PyQt5.QtGui import QMatrix4x4, QVector3D, QQuaternion
from PyQt5.QtCore import pyqtSlot as Slot, pyqtProperty as Property

class Camera(Product.Product):
    '''
    classdocs
    '''
    def __init__(self, parent = None):
        super(Camera, self).__init__(parent)
        '''
        Constructor
        '''
        self.perspective_override = None


    Product.ConstProperty(vars(), Transforms.Transform, 'transform', Transforms.Transform())

    Product.InputProperty(vars(), float, 'vfov', 45.0)

    Product.InputProperty(vars(), float, 'near', 0.1)

    Product.InputProperty(vars(), float, 'far', 10000)

    Product.InputProperty(vars(), QVector3D, 'eye', QVector3D(0,-10,0))

    Product.InputProperty(vars(), QVector3D, 'up', QVector3D(0,0,1))

    Product.InputProperty(vars(), QVector3D, 'center', QVector3D(0,0,0))

    @Slot(float, float)
    def pan_tilt(self, _eye, _up, dx, dy):
        _front = self.center - _eye
        _right = QVector3D.crossProduct(_front, _up).normalized()
        q = QQuaternion.fromAxisAndAngle(_up, dx) * QQuaternion.fromAxisAndAngle(_right, dy)
        self.eye = self.center - q.rotatedVector(_front)
        self.up = q.rotatedVector(_up)

    def roll(self, _eye, _up, delta):
        self.up = QQuaternion.fromAxisAndAngle(self.center - _eye, delta).rotatedVector(_up)

    def translate(self, _eye, _center, dx, dy):
        _left = QVector3D.crossProduct((_center - _eye), self.up).normalized()
        d = _left * dx + self.up * dy
        d *= (_eye - _center).length()
        self.eye = _eye + d
        self.center = _center + d

    def view_matrix(self):
        mat_v = QMatrix4x4()
        mat_v.lookAt(self.eye, self.center, self.up)
        return mat_v

    def _update(self):
        self.transform = self.view_matrix().inverted()[0]



def convert_hz_intrinsic_to_opengl_projection(
 K # 3x3 intrinsic matrix (from e.g. opencv)
,znear, zfar # clipping
,width, height # viewport dimensions
,x0=0, y0=0 # image plane origin
,window_coords=None):
    #https://gist.github.com/astraw/1341472#file_projection_math.py
    znear = float(znear)
    zfar = float(zfar)
    depth = zfar - znear
    q = -(zfar + znear) / depth
    qn = -2 * (zfar * znear) / depth

    if window_coords=='y up':
        proj = np.array([[ 2*K[0,0]/width, -2*K[0,1]/width, (-2*K[0,2]+width+2*x0)/width, 0 ],
                        [  0,             -2*K[1,1]/height,(-2*K[1,2]+height+2*y0)/height, 0],
                        [0,0,q,qn],  # This row is standard glPerspective and sets near and far planes.
                        [0,0,-1,0]], 'f4') # This row is also standard glPerspective.
    else:
        assert window_coords=='y down'
        proj = np.array([[ 2*K[0,0]/width, -2*K[0,1]/width, (-2*K[0,2]+width+2*x0)/width, 0 ],
                        [  0,              2*K[1,1]/height,( 2*K[1,2]-height+2*y0)/height, 0],
                        [0,0,q,qn],  # This row is standard glPerspective and sets near and far planes.
                        [0,0,-1,0]], 'f4') # This row is also standard glPerspective.
    return proj
