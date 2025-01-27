from QtQmlViewport import Product, BVH
from QtQmlViewport.Effect import Effect
from QtQmlViewport.Geometry import Geometry
from QtQmlViewport.Transforms import Transform
from QtQmlViewport.utils import to_numpy, tf_to_numpy

from PyQt5.QtGui import QMatrix4x4
from PyQt5.QtCore import pyqtProperty as Property, pyqtSignal as Signal, pyqtSlot as Slot, QObject, Q_CLASSINFO
from PyQt5.QtQml import QQmlListProperty

import numpy as np

class Renderable(Product.Product):
    def __init__( self, parent = None ):
        super(Renderable, self).__init__( parent )
    

    def worldTransform(self):
        tf = self.transform.worldTransform(True) if self.transform else QMatrix4x4()
        if self.parent is not None and issubclass(type(self.parent), Actors):
            return self.parent.worldTransform() * tf
        return tf

    Product.InputProperty(vars(), Transform, 'transform', None)

    Product.InputProperty(vars(), bool, 'visible', True)
class Actor( Renderable ):

    def __init__( self, parent = None, geometry = None, effect = None, transform = None, name = None, visible = True, bbox = None, type_id = -1, instance_id = -1):
        super(Actor, self).__init__( parent )
        self._renderRank = 0
        self.bbox = bbox
        self.type_id = type_id
        self.instance_id = instance_id

        self.setObjectName(name)

        self.geometry = geometry
        self.effect = effect
        self.transform = transform
        self.visible = visible




    click = Signal(int, 'QVector3D', 'QVector3D', 'QVector3D', 'QVector3D', 'QVector3D', 'QVariant', 'QVariant'
                         , arguments = ['id', 'tuv', 'worldOrigin', 'worldDirection', 'localOrigin', 'localDirection', 'event', 'viewport'] )
    move = Signal('QVector3D', 'QVector3D', 'QVariant', 'QVariant'
                         , arguments = ['worldOrigin', 'worldDirection', 'event', 'viewport'] )
    release = Signal('QVector3D', 'QVector3D', 'QVariant', 'QVariant'
                         , arguments = ['worldOrigin', 'worldDirection', 'event', 'viewport'] )
    hoverMove = Signal(int, 'QVector3D', 'QVector3D', 'QVector3D', 'QVector3D', 'QVector3D', 'QVariant', 'QVariant'
                         , arguments = ['id', 'tuv', 'worldOrigin', 'worldDirection', 'localOrigin', 'localDirection', 'event', 'viewport'] )
    hoverEnter = Signal(int, 'QVector3D', 'QVector3D', 'QVector3D', 'QVector3D', 'QVector3D', 'QVariant', 'QVariant'
                         , arguments = ['id', 'tuv', 'worldOrigin', 'worldDirection', 'event', 'viewport'] )
    hoverLeave = Signal('QVector3D', 'QVector3D', 'QVariant', 'QVariant'
    , arguments = ['worldOrigin', 'worldDirection', 'event', 'viewport'])

    

    Product.InputProperty(vars(), bool, 'pickable', True) # ignored in picking?

    Product.InputProperty(vars(), bool, 'clickable', True) # Receives click and move signal

    Product.InputProperty(vars(), bool, 'hoverable', True) # Receives hover signals

    Product.InputProperty(vars(), bool, 'mouseOver', False)

    Product.InputProperty(vars(), bool, 'selectable', False) # is registered by the viewport as "selected" on mouse release

    Product.InputProperty(vars(), bool, 'selected', False)

    Product.InputProperty(vars(), Geometry, 'geometry', None)

    Product.InputProperty(vars(), Effect, 'effect', None)

    Product.InputProperty(vars(), int, 'renderRank', 0)







class Actors( Renderable ):

    def __init__(self, parent = None, name = None, bbox = None, shared_transform = None, scale = 1, all_vertices = [], type_id = -1, instance_id = -1):
        super(Actors, self).__init__( parent )
        self.setObjectName(name)
        self._renderables = []
        self._manually_added = []
        self._instantiated = []
        self.bbox = bbox
        self.scale = scale
        self.all_vertices = all_vertices
        self.type_id = type_id
        self.instance_id = instance_id
        

        # Warning: Not a scene graph, totally manual feature, you have share this transform yourself
        # it is meant to be used to access and modify a transformed manually shared among actors (Advanced users :-) )
          
        self.shared_transform = shared_transform



    Q_CLASSINFO('DefaultProperty', 'renderables')

    renderablesChanged = Signal()
    @Property(QQmlListProperty, notify = renderablesChanged)
    def renderables(self):
        return QQmlListProperty(Renderable, self, append = self.append_renderable, at=self.at_renderable, clear = self.clear_renderable, count = self.count_renderable)

    def append_renderable(self, _, renderable):
        self.addActor(renderable, self._renderables)

    def at_renderable(self, _, index):
        return self._renderables[index]

    def clear_renderable(self, _):
        self.clearActors(True, self._renderables)

    def count_renderable(self, _):
        return self._renderables.count()
    

    Product.InputProperty(vars(), Transform, 'transform', None)

    def objectAdded(self, index, object):
        self.addActor(object, self._instantiated)
    def objectRemoved(self, index, object):
        self.removeActor(object, True, self._instantiated)

    def cb_before_write_instantiator(self, instantiator):
        if self.instantiator is not None:
            self.instantiator.objectAdded.disconnect(self.objectAdded)
            self.instantiator.objectRemoved.disconnect(self.objectRemoved)
        if instantiator is not None:
            instantiator.objectAdded.connect(self.objectAdded)
            instantiator.objectRemoved.connect(self.objectRemoved)
        return instantiator


    Product.InputProperty(vars(), QObject, 'instantiator', None, callback = None, before_write_callback = cb_before_write_instantiator)

    @Property(int, notify=renderablesChanged)
    def count(self):
        return len(self.children_actors())

    actorsChanged = Signal()

    @Property(list, notify=actorsChanged)
    def actors(self):
        return self.children_actors()

    @Slot(Renderable, result = Renderable)
    def addActor(self, actor, container = None):
        if container is None:
            container = self._manually_added

        container.append(actor)
        actor.parent = self
        self.makeDirty()
        self.actorsChanged.emit()
        return actor

    @Slot()
    def removeActor(self, actor, delete = False, container = None):
        if container is None:
            container = self._manually_added
        
        if actor in container:
            container.remove(actor)
            actor.setParent(None)
            if delete:
                actor.deleteLater()
            self.makeDirty()
            self.actorsChanged.emit()

    @Slot()
    def clearActors(self, delete = False, container = None):
        if container is None:
            container = self._manually_added

        for a in container:
            a.setParent(None)
            if delete:
                a.deleteLater()
        container.clear()
        self.makeDirty()
        self.actorsChanged.emit()

    def children_actors(self):
        return self._manually_added + self._renderables + self._instantiated

    def actor_to_id(self, id_to_actors = None):

        if id_to_actors is None:
            id_to_actors = self.get_visible_actors()

        actor_to_id_ = {}
        for i, (a, _) in enumerate(id_to_actors):
            n = a.objectName()
            if not n:
                n = "anonymous"
            
            if n in actor_to_id_:
                actor_to_id_[n].append(i)
            else:
                actor_to_id_[n] = [i]
        return actor_to_id_
    



    def merged_bvhs(self):
        '''
            Warning, this will update actors
        '''
        bvhs = []
        matrices = []
        effects = []

        
        id_to_actors = self.get_visible_actors()
        for i, (a, parentTransform) in enumerate(id_to_actors):
            a.geometry.update()
            if a.transform:
                a.transform.update()
            bvh = a.geometry.goc_bvh(True)
            bvhs.append(bvh if bvh is None else bvh.bvh)
            p = tf_to_numpy(a.transform) if parentTransform else np.eye(4, dtype = 'f4')
            m = tf_to_numpy(a.transform) if a.transform else np.eye(4, dtype = 'f4')
            matrices.append(np.matmul(p, m))

            uniforms = {}
            for k,v, in a.effect.shader0.uniforms.items():
                uniforms[k] = to_numpy(v)

            attributes = {}
            for k,v in a.geometry.attribs.get_attributes().items():
                if v is not None:
                    attributes[k] = v.ndarray 

            textures = {}
            for k,v in a.effect.shader0.textures.items():
                textures[k] = v.ndarray 

            effects.append({ "type": a.effect.objectName()
            , "uniforms": uniforms
            , "textures": textures
            , "attribs": attributes})

        bvh, triangles_mapping, triangle_offsets, vertex_offsets = BVH.merge_bvhs(bvhs, matrices)

        return bvh, {'id_to_actors': id_to_actors
        , 'triangles_mapping': triangles_mapping
        , 'triangle_offsets': triangle_offsets
        , 'vertex_offsets': vertex_offsets
        , "effects": effects}

    def is_any_visible_actor_dirty(self):


        if not self.visible:
            return False

        def recursive_check(a):
            if issubclass(type(a), Actor):
                if a.visible:
                    return a.geometry.dirty | (a.transform.dirty if a.transform else False)
                return False
                
            elif issubclass(type(a), Actors):
                return a.is_any_visible_actor_dirty()
                              
            raise TypeError(type(a))

        all_actors = self.children_actors()
        for r in all_actors:
            if recursive_check(r):
                return True

        return False

    def get_visible_actors(self, parentTransform = QMatrix4x4()):
        
        if not self.visible:
            return []

        actors = []

        tf = parentTransform * (self.transform.worldTransform(True) if self.transform else QMatrix4x4())


        all_actors = self.children_actors()

        for a in all_actors:
            if issubclass(type(a), Actor):
                if a.visible:
                    actors.append((a, tf))
            elif issubclass(type(a), Actors):
                actors.extend(a.get_visible_actors(tf)) #union of sets

        return actors


