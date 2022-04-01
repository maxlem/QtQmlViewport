from QtQmlViewport import Product
from QtQmlViewport.Array import ArrayBase
from QtQmlViewport.Geometry import Attribs

class AmplitudeAttribs(Attribs):
    def __init__( self, parent=None, vertices = None, normals = None, amplitude = None ):
        super(AmplitudeAttribs, self).__init__( parent, vertices, normals )
        self.amplitude = amplitude

    Product.InputProperty(vars(), ArrayBase, 'amplitude', None)

    def get_attributes(self):
        '''
        override this method to add all your attribs
        '''
        a = super(AmplitudeAttribs, self).get_attributes()
        a["amplitude"] = self._amplitude
        return a

class ColorsAttribs(Attribs):
    def __init__( self, parent=None, vertices = None, normals = None, colors = None):
        super(ColorsAttribs, self).__init__( parent, vertices, normals )
        
        self.colors = colors

    Product.InputProperty(vars(), ArrayBase, 'colors', None)

    def get_attributes(self):
        '''
        override this method to add all your attribs
        '''
        a = super(ColorsAttribs, self).get_attributes()
        a["colors"] = self._colors
        return a

class SegmentationLabelsAttribs(Attribs):
    def __init__( self, parent=None ):
        super(SegmentationLabelsAttribs, self).__init__( parent )

    Product.InputProperty(vars(), ArrayBase, 'labels', None)

    def get_attributes(self):
        '''
        override this method to add all your attribs
        '''
        a = super(SegmentationLabelsAttribs, self).get_attributes()
        a["labels"] = self._labels
        return a


class TexcoordsAttribs(Attribs):
    def __init__( self, parent=None, vertices = None, normals = None, texcoords0 = None ):
        super(TexcoordsAttribs, self).__init__( parent, vertices, normals )

        self.texcoords0 = texcoords0

    Product.InputProperty(vars(), ArrayBase, 'texcoords0', None)

    def get_attributes(self):
        '''
        override this method to add all your attribs
        '''
        a = super(TexcoordsAttribs, self).get_attributes()
        a["texcoords0"] = self._texcoords0
        return a