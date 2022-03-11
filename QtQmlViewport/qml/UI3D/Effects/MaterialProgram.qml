/*
* Created on Apr 2, 2018
*
* \author: maxime
* \file : MaterialProgram.qml
*/

import QtQuick 2.5
import Viewport 1.0

GLSLProgram
{
    id: component
    property color color : "blue"
    property color backColor : "red"

    property color ambientColor: Qt.rgba(.1, .1, .1, 1.0)
    property color specularColor: Qt.rgba(.1, .1, .1, 1.0)
    property real lightPower: 2e2 //light power should be roughly ("sight distance")^2
    property real shininess: 100.0 //less is more
    property bool lightFollowsCamera: true
    property vector3d lightPosition: Qt.vector3d(0,0,-10)
    property bool reverseBackfaces: true
    property bool flatShading: false

    uniforms: ({  'color': color 
                , 'back_color' : backColor
                , 'ambient_color': ambientColor
                , 'specular_color': specularColor
                , 'light_power': lightPower
                , 'shininess': shininess
                , 'light_follows_camera': lightFollowsCamera
                , 'light_position': lightPosition
                , 'reverse_backfaces' : reverseBackfaces
                , 'flat_shading': flatShading})
    vertexShader: MaterialGLSL.vertexShader
    geometryShader: MaterialGLSL.geometryShader
    fragmentShader: MaterialGLSL.fragmentShader
    }