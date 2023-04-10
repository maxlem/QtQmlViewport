/*
* Created on Feb 26, 2018
*
* \author: maxime
* \file : AmplitudesProgram.qml
*/

import QtQuick 2.5
import Viewport 1.0

GLSLProgram
{
    id: component
    property color color : "black"
    uniforms: ({color: color})
    vertexShader: "
                        #version 310 es
                        precision mediump float;
                        uniform  mat4 matrix;
                        uniform  float point_size;
                        in  vec4 vertices;
                        in  vec3 normals;
                        in  float amplitude;

                        out float a;
                        void main()
                        {
                            gl_Position = matrix*vertices;
                            gl_PointSize = point_size;
                            a = amplitude;
                        }
                   "
    fragmentShader: "
                        #version 310 es
                        precision mediump float;
                        uniform  vec4 color;
                        in float a;
                        layout(location = 0) out vec4 frag_color;
                        layout(location = 1) out vec4 frag_color_copy;
                        void main()
                        {
                            float t = (.25 + a) / 1.25; //ensure minimal visibility
                            frag_color_copy = frag_color = vec4(color.x * t, color.y * t, color.z * t, color.w);
                        }
                    "
}