from QtQmlViewport import Product

from PyQt5.QtCore import QObject, pyqtProperty as Property, pyqtSignal as Signal, pyqtSlot as Slot, Q_CLASSINFO
from PyQt5.QtGui import QMatrix4x4, QVector3D, QQuaternion

import math


__singletons = QObject()

def get_Transforms(*args):
    try:
        return __singletons._Transforms
    except AttributeError:
        __singletons._Transforms = Transforms()
        return __singletons._Transforms
class Transforms(QObject):
    def __init__(self, parent = None):
        super().__init__(parent)


    @Slot(QVector3D, float, result = QQuaternion)
    def qFromAA(self, axis, angle_rad):
        return QQuaternion.fromAxisAndAngle(axis, math.degrees(angle_rad))

    @Slot(float, float, float, result = QQuaternion)
    def qFromEuler(self, roll_rad, pitch_rad, yaw_rad):
        return QQuaternion.fromEulerAngles(math.degrees(roll_rad), math.degrees(pitch_rad), math.degrees(yaw_rad))

    @Slot(QQuaternion, QVector3D, result = QMatrix4x4)
    def mFromTQ(self, t = QVector3D(), q = QQuaternion()):
        m = QMatrix4x4()
        m.rotate(q)
        m.translate(t)
        return m  

class Transform( Product.Product ):

    def __init__( self, parent = None ):
        super(Transform, self).__init__( parent )

    Q_CLASSINFO('DefaultProperty', 'parentTransform')

    def before_write_local_transform(self, matrix4x4):
        if matrix4x4 is None:
            matrix4x4 = QMatrix4x4()
        return matrix4x4

    Product.InputProperty(vars(), QMatrix4x4, 'localTransform', QMatrix4x4(), None, before_write_local_transform)

    Product.InputProperty(vars(), Product.Product, 'parentTransform', None)

    @Slot(result = QMatrix4x4)
    def earliestParent(self, update = False):
        tf = self.parentTransform
        while tf is not None:
            tf = tf.parentTransform
        return tf

    @Slot(result = QMatrix4x4)
    def worldTransform(self, update = False):
        if update:
            self.update()
        assert not self.dirty
        return self.localTransform if self.parentTransform is None \
               else self.parentTransform.worldTransform() * self.localTransform



class Translation( Transform ):

    def __init__( self, parent = None ):
        super(Translation, self).__init__( parent )


    def translate_cb(self):
        m = QMatrix4x4()
        m.translate(self._translate)
        self.localTransform = m

    Product.InputProperty(vars(), QVector3D, 'translate', QVector3D(), translate_cb)

class Rotation( Transform ):

    def __init__( self, parent = None ):
        super(Rotation, self).__init__( parent )


    def quaternion_cb(self):
        m = QMatrix4x4()
        m.rotate(self._quaternion)
        self.localTransform = m

    Product.InputProperty(vars(), QQuaternion, 'quaternion', QQuaternion(), quaternion_cb)

class MatrixTransform( Transform ):

    def __init__( self, parent = None, matrix = QMatrix4x4() ):
        super(MatrixTransform, self).__init__( parent )
        self.matrix = matrix

    def matrix_cb(self):
        self.localTransform = self.matrix

    Product.InputProperty(vars(), QMatrix4x4, 'matrix', QMatrix4x4(), matrix_cb)


