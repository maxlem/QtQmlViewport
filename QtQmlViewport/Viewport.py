from QtQmlViewport import InFboRenderer, utils, Product, CustomActors
from QtQmlViewport.Actors import Actors, Renderable
from QtQmlViewport.Camera import Camera
from QtQmlViewport.Geometry import Geometry, BVH

from PyQt5.QtQuick import QQuickFramebufferObject
from PyQt5.QtGui import QMatrix4x4, QVector3D, QColor, qRgba
from PyQt5.QtCore import Qt, QRectF, QTimer, pyqtSlot as Slot

import numpy as np
import math, traceback, os, warnings


class NothingToPickException(Exception):
    def __init__(self, world_origin, world_direction):
        super().__init__('Nothing to pick')
        self.world_origin, self.world_direction = world_origin, world_direction

class Viewport( QQuickFramebufferObject ):

    def __init__( self, parent=None ):
        super(Viewport, self).__init__( parent )
        self.setAcceptedMouseButtons(Qt.AllButtons)
        self.setAcceptHoverEvents(True)
        self.renderer = None
        self.camera = Camera()

        self._mouse_start = None
        self._start_eye = None
        self._start_up = None

        self._debug_actors = Actors()

        self.render_to_texture_attachment = -1

        self._hovered = None
        self._clicked = None

        
        
    Product.RWProperty(vars(), bool, 'debug', False)

    Product.RWProperty(vars(), Camera, 'camera', None)

    Product.RWProperty(vars(), QColor, 'backgroundColor', QColor(qRgba(255,255,255,255)))

    Product.RWProperty(vars(), Actors, 'actors', None)

    Product.RWProperty(vars(), Renderable, 'selected', None)



    def aspect_ratio(self):
        return self.width()/self.height()
    
    def view_matrix(self):
        return self.camera.view_matrix()

    def perspective_matrix(self):
        if self.camera.perspective_override is None:
            p = QMatrix4x4()
            p.perspective(self.camera.vfov, self.aspect_ratio(), self.camera.near, self.camera.far)
            return p
        else:
            return self.camera.perspective_override

    def orthographic_matrix(self):
        p = QMatrix4x4()
        p.ortho(0, self.width(), 0, self.height(), self.camera.near, self.camera.far)
        return p




    def pick_helper(self, clicked_x, clicked_y):
        # http://schabby.de/picking-opengl-ray-tracing/
        aspect_ratio = self.aspect_ratio()

        cam_origin = self.camera.eye
        cam_direction = (self.camera.center - cam_origin).normalized()

        # The coordinate system we chose has x pointing right, y pointing down, z pointing into the screen
        # in screen coordinates, the vertical axis points down, this coincides with our 'y' axis.
        v = -self.camera.up # our y axis points down

        # in screen coordinates, the horizontal axis points right, this coincides with our x axis
        h = QVector3D.crossProduct(cam_direction, self.camera.up).normalized() # cam_direction points into the screen

        # in InFobRenderer::render(), we use Viewport::perspective_matrix(), where self.camera.fov is used 
        # as QMatrix4x4::perspective()'s verticalAngle parameter, so near clipping plane's vertical scale is given by:
        v_scale = math.tan( math.radians(self.camera.vfov) / 2 ) * self.camera.near
        h_scale = v_scale * aspect_ratio

        # translate mouse coordinates so that the origin lies in the center
        # of the viewport (screen coordinates origin is top, left)
        x = clicked_x - self.width() / 2
        y = clicked_y - self.height() / 2

        # scale mouse coordinates so that half the view port width and height
        # becomes 1 (to be coherent with v_scale, which takes half of fov)
        x /= (self.width() / 2)
        y /= (self.height() / 2)

        # the picking ray origin: corresponds to the intersection of picking ray with
        # near plane (we don't want to pick actors behind the near plane)
        world_origin = cam_origin + cam_direction * self.camera.near + h * h_scale * x + v * v_scale * y

        # the picking ray direction
        world_direction = (world_origin - cam_origin).normalized()
        return v,h,world_origin,world_direction
    
    def pick(self, clicked_x, clicked_y, modifiers = None):

        v, h, world_origin, world_direction = self.pick_helper(clicked_x, clicked_y)


        

        min_t = float("inf")
        min_result = None
        for actor in self.renderer.sorted_actors:
            if actor._geometry and actor.pickable:
                if actor._geometry.indices is not None\
                and actor._geometry.attribs.vertices is not None:

                    bvh = actor._geometry.goc_bvh()
                    if bvh is None:
                        continue
                    bvh.update()
                    if bvh.bvh is None:
                        continue
                    
                    
                    local_origin, local_direction = self.to_local(world_origin, world_direction, actor)


                    local_origin_np, local_direction_np = utils.to_numpy(local_origin), utils.to_numpy(local_direction)
                    # try to intersect the actor's geometry!
                    if bvh.primitiveType == BVH.PrimitiveType.TRIANGLES or bvh.primitiveType == BVH.PrimitiveType.LINES:

                        ids, tuvs = bvh.bvh.intersect_ray(local_origin_np, local_direction_np, True)

                        if ids.size > 0:
                            actor_min_t = tuvs[:,0].min() 
                            if actor_min_t < min_t:
                                min_t = actor_min_t
                                min_result = (actor, ids, tuvs, world_origin, world_direction, local_origin, local_direction)

                    elif bvh.primitiveType == BVH.PrimitiveType.POINTS:

                        object_id, distance, t = bvh.bvh.ray_distance(local_origin_np, local_direction_np)
                        real_distance = math.sqrt(t**2 + distance**2)
                        if real_distance < min_t:
                            min_t = real_distance
                            min_result = (actor, bvh.indices.ndarray[object_id, None], np.array([[t, distance, real_distance]]), world_origin, world_direction, local_origin, local_direction)                       
                    
        
        if self.debug and modifiers is not None and bool(modifiers & Qt.ShiftModifier):
            np_origin = utils.to_numpy(world_origin)
            np_v = utils.to_numpy(v)
            np_h = utils.to_numpy(h)

            self._debug_actors.clearActors()
            a = self._debug_actors.addActor(CustomActors.arrow(np_origin, np_origin + np_h, 0.1, QColor('red')))
            b = self._debug_actors.addActor(CustomActors.arrow(np_origin, np_origin + np_v, 0.1, QColor('green')))
            c = self._debug_actors.addActor(CustomActors.arrow(np_origin, np_origin + utils.to_numpy(world_direction) * (min_t if min_t != float("inf") else 500.0), 0.1, QColor('magenta')))
            a.pickable = False
            b.pickable = False
            c.pickable = False

        if min_result is not None:
            return min_result


        raise NothingToPickException(world_origin, world_direction)

    def to_local(self, world_origin, world_direction, actor):
        # bring back the actor at the origin
        if not "transform" in actor.bo_actor:
            actor.update()
            print(actor.dirty)
        m = actor.bo_actor["transform"]
        m_inv = m.inverted()[0]

        # bring the ray in the actor's referential
        local_origin, local_direction = m_inv.map(world_origin), m_inv.mapVector(world_direction)
        return local_origin, local_direction



    def mouseDoubleClickEvent(self, event):
        btns = event.buttons()
        if btns & Qt.MidButton or (btns & Qt.LeftButton and event.modifiers() & Qt.ControlModifier) :
            try:
                _, _, tuvs, world_origin, world_direction, _, _ = self.pick(event.localPos().x(), event.localPos().y())
                p = world_origin + world_direction * tuvs[0,0]
                self.setCameraRotationCenter(world_origin, p)

            except NothingToPickException:
                pass
            except:
                traceback.print_exc()

    @Slot(QVector3D, QVector3D)
    def setCameraRotationCenter(self, eye, center):
        # centers camera on selected point
        self.camera.center = center
        self.camera.eye = eye
        self.update()


    def signal_helper(self, signal, event, ids, tuvs, world_origin, world_direction, local_origin, local_direction):
        if len(ids) > 0:
            signal.emit(ids[0], QVector3D(tuvs[0,0], tuvs[0,1], tuvs[0,2]), world_origin, world_direction, local_origin, local_direction, utils.QObject_to_dict(event), self)


    def mousePressEvent(self, event):
        """
        Called by the Qt libraries whenever the window receives a mouse click.
        """
        self._mouse_start = (event.localPos().x(), event.localPos().y())
        self._start_eye = self.camera.eye
        self._start_up = self.camera.up
        self._start_center = self.camera.center

        if event.buttons() & Qt.LeftButton:
            try:
                actor, ids, tuvs, world_origin, world_direction, local_origin, local_direction = self.pick(event.localPos().x(), event.localPos().y(), event.modifiers())
                if actor.clickable:
                    self.signal_helper(actor.click, event, ids, tuvs, world_origin, world_direction, local_origin, local_direction)
                    self._clicked = actor
                    self._clicked.destroyed.connect(self.clearClicked)
                    if actor.selectable:
                        self.selected = actor
                        self.selected.selected = True
            except NothingToPickException as e:
                if self.selected is not None:
                    self.selected.selected = False
                    self.selected = None

            except:
                traceback.print_exc()
        
                
        event.setAccepted(True)

    def mouseMoveEvent(self, event):
        """
        Called by the Qt libraries whenever the window receives a mouse
        move/drag event.
        """
        btns = event.buttons()

        x, y = event.localPos().x(), event.localPos().y()

        x_0, y_0 = self._mouse_start

        dx, dy = (x - x_0, y - y_0)

        h_width = self.width()/2
        h_height = self.height()/2

        if btns & Qt.LeftButton:
            if self._clicked:
                _, _, world_origin, world_direction = self.pick_helper(event.pos().x(), event.pos().y())
                self._clicked.move.emit(world_origin, world_direction, utils.QObject_to_dict(event), self)
            else:
                # we want half a screen movement rotates the camera 90deg:
                self.camera.pan_tilt(self._start_eye, self._start_up, 90.0 * dx/h_width, 90.0 * dy/h_height)

        elif btns & Qt.MidButton:
            self.camera.translate(self._start_eye, self._start_center, -dx/h_width, dy/h_height)

        elif btns & (Qt.RightButton):
            self.camera.roll(self._start_eye, self._start_up, -90.0 * dy/h_width)

        # re-draw at next timer tick
        self.update()

    def mouseReleaseEvent(self, event):

        if self._clicked is not None:
                
            _, _, world_origin, world_direction = self.pick_helper(event.pos().x(), event.pos().y())
            self._clicked.release.emit(world_origin, world_direction, utils.QObject_to_dict(event), self)
            self._clicked.destroyed.disconnect(self.clearClicked)    
            self._clicked = None


    def wheelEvent(self, event):
        """
        Called by the Qt libraries whenever the window receives a mouse wheel change.
        """
        delta = event.angleDelta().y()

        # move in look direction of camera
        # note: this will only do something for non-orthographic projection
        front = self.camera.center - self.camera.eye
        if event.modifiers() & Qt.ShiftModifier:
            factor = (5*120)
        else:
            factor = (5*12)
        d = front.normalized() * delta/factor
        self.camera.eye -= d
        self.camera.center -= d

        # re-paint at the next timer tick
        self.update()

    @Slot()
    def clearHovered(self):
        self._hovered = None
    @Slot()
    def clearClicked(self):
        self._clicked = None
    def hoverMoveEvent(self, event):

        try:
            actor, ids, tuvs, world_origin, world_direction, local_origin, local_direction = self.pick(event.pos().x(), event.pos().y(), event.modifiers())
            if actor == self._hovered:
                self.signal_helper(actor.hoverMove, event, ids, tuvs, world_origin, world_direction, local_origin, local_direction)
            else:
                
                if self._hovered is not None: #release the previous pick
                    self._hovered.hoverLeave.emit(world_origin, world_direction, utils.QObject_to_dict(event), self)
                    self._hovered.mouseOver = False
                if actor.hoverable:
                    self._hovered = actor
                    self._hovered.destroyed.connect(self.clearHovered)
                    self.signal_helper(actor.hoverEnter, event, ids, tuvs, world_origin, world_direction, local_origin, local_direction)
                    actor.mouseOver = True

        except NothingToPickException as e:
            if self._hovered is not None:
                self._hovered.hoverLeave.emit(e.world_origin, e.world_direction, utils.QObject_to_dict(event), self)
                self._hovered.mouseOver = False
                self._hovered.destroyed.disconnect(self.clearHovered)
            self._hovered = None
            pass
        except:
            traceback.print_exc()





    def createRenderer( self ):
        self.renderer = InFboRenderer.InFboRenderer()
        self.timer = QTimer()
        self.timer.timeout.connect(self.harvest_updates)
        self.timer.start(0) #will be called after each event loop
        return self.renderer

    def set_render_to_texture_attachment(self, attachment):
        if "QSG_RENDER_LOOP" in os.environ:
            if os.environ['QSG_RENDER_LOOP'] != "basic":
                warnings.warn("Error: multithreaded rendering enabled, please set os.environ['QSG_RENDER_LOOP'] = 'basic' before any Qt call")
        if self.render_to_texture_attachment != attachment:
            self.render_to_texture_attachment = attachment
            self.update()

    def get_render_to_texture_array(self):
        if self.renderer is not None:
            return self.renderer.render_to_texture_array
        return None
    
    def harvest_updates(self):
        self.update()
        # TODO: to prevent clogging 1 CPU, we should do something like this
        # if self.renderer is not None:
        #     for actor in self.renderer.sorted_actors:
        #         if actor.dirty:
        #             self.update()
        #             return