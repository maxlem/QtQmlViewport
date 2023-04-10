/*
* Created on Dec 21, 2018
*
* \author: maxime
*/

import QtQuick 2.5
import UI3D 1.0
import Viewport 1.0

GLSLProgram
{
    id: component
    property alias defaultColorMap : defaultColorMap_
    property ColorMapArray colorMap : ColorMapArray{id: defaultColorMap_}
    textures: ({colorMap: component.colorMap})
    vertexShader: "
                        #version 310 es
                        precision mediump float;
                        uniform  mat4 matrix;
                        in  vec4 vertices;
                        in  vec3 normals;
                        in  float amplitude;

                        out float a;
                        void main()
                        {
                            gl_Position = matrix*vertices;
                            a = amplitude;
                        }
                   "
    fragmentShader: "
                        #version 310 es
                        precision mediump float;
                        uniform sampler1D colorMap;
                        in float a;
                        layout(location = 0) out vec4 frag_color;
                        layout(location = 1) out vec4 frag_color_copy;
                        void main()
                        {
                            frag_color_copy = frag_color = texture(colorMap, a);
                        }
                    "
}