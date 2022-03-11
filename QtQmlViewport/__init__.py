from QtQmlViewport.PyBVH import BVH, PointsBVH
from QtQmlViewport import linalg
import numpy as np
import traceback

from PyQt5.QtQml import qmlRegisterType, qmlRegisterSingletonType

#generic non-leddar-related modules

from . import Viewport, Camera, Actors, Geometry, Effect, Array, Transforms, Product, CustomAttribs, CustomEffects






qmlRegisterType(Product.Product, "Viewport", 1, 0, "Product" )
qmlRegisterType(Product.VariantProduct, "Viewport", 1, 0, "VariantProduct" )

qmlRegisterType(Viewport.Viewport, "Viewport", 1, 0, "Viewport" )

qmlRegisterType(Camera.Camera, "Viewport", 1, 0, "Camera" )

qmlRegisterType(Actors.Renderable, "Viewport", 1, 0, "Renderable" )
qmlRegisterType(Actors.Actor, "Viewport", 1, 0, "Actor" )
qmlRegisterType(Actors.Actors, "Viewport", 1, 0, "Actors" )
#
qmlRegisterType(Transforms.Transform, "Viewport", 1, 0, "Transform" )
qmlRegisterType(Transforms.Translation, "Viewport", 1, 0, "Translation" )
qmlRegisterType(Transforms.Rotation, "Viewport", 1, 0, "Rotation" )
qmlRegisterType(Transforms.MatrixTransform, "Viewport", 1, 0, "MatrixTransform" )
#
qmlRegisterType(Geometry.Geometry, "Viewport", 1, 0, "Geometry" )
qmlRegisterType(Geometry.Attribs, "Viewport", 1, 0, "Attribs" )
qmlRegisterType(CustomAttribs.AmplitudeAttribs, "Viewport", 1, 0, "AmplitudeAttribs" )
qmlRegisterType(CustomAttribs.ColorsAttribs, "Viewport", 1, 0, "ColorsAttribs" )
#
qmlRegisterType(Array.Array, "Viewport", 1, 0, "Array")
qmlRegisterType(Array.ArrayFloat1, "Viewport", 1, 0, "ArrayFloat1" )
qmlRegisterType(Array.ArrayFloat2, "Viewport", 1, 0, "ArrayFloat2" )
qmlRegisterType(Array.ArrayFloat3, "Viewport", 1, 0, "ArrayFloat3" )
qmlRegisterType(Array.ArrayFloat4, "Viewport", 1, 0, "ArrayFloat4" )
qmlRegisterType(Array.ArrayShort512, "Viewport", 1, 0, "ArrayShort512")
qmlRegisterType(Array.ArrayUShort1, "Viewport", 1, 0, "ArrayUShort1")

qmlRegisterType(Array.ArrayUInt1, "Viewport", 1, 0, "ArrayUInt1" )
qmlRegisterType(Array.ArrayUByte3, "Viewport", 1, 0, "ArrayUByte3" )
qmlRegisterType(Array.ArrayUByte4, "Viewport", 1, 0, "ArrayUByte4" )

qmlRegisterType(Array.ArrayInt1, "Viewport", 1, 0, "ArrayInt1" )

qmlRegisterType(Effect.GLSLProgram, "Viewport", 1, 0, "GLSLProgram" )
qmlRegisterType(Effect.Effect, "Viewport", 1, 0, "Effect" )
qmlRegisterSingletonType(CustomEffects.MaterialGLSL, "Viewport", 1, 0, "MaterialGLSL", CustomEffects.get_MaterialGLSL)

def merge_bvhs(bvhs, matrices = None):

    vertices_list  = []
    triangles_list = []
    
    vertex_offsets = [0]
    for i, bvh in enumerate(bvhs):
        if bvh is None:
            triangles_list.append(np.empty((0,3), 'u4'))
            vertices_list.append(np.empty((0,3), 'f4'))
            continue

        if matrices is not None and matrices[i] is not None:
            vertices_list.append(linalg.map_points(matrices[i], bvh.vertices))
        else:
            vertices_list.append(bvh.vertices)
        offset = vertex_offsets[-1]
        triangles_list.append(bvh.triangles + offset)
        offset += bvh.vertices.shape[0]
        vertex_offsets.append(offset)

    vertices = np.vstack(vertices_list)
    triangles = np.vstack(triangles_list)
    triangles_mapping = np.empty((triangles.shape[0]), 'u4')

    triangle_offsets = [0]
    offset_from = 0
    for i, t in enumerate(triangles_list):
        t_size = 0 if t is None else t.shape[0]
        triangles_mapping[offset_from:offset_from+t_size] = i
        offset_from += t_size
        triangle_offsets.append(offset_from)


    return BVH(triangles, vertices), triangles_mapping, triangle_offsets, vertex_offsets

BVH.merge_bvhs = staticmethod(merge_bvhs)

