/*
* Created on March 15, 2022
*
* @author: maxime
* \file : Arrow2D.qml
*
*/

import QtQuick 2.5
import Viewport 1.0
import UI3D 1.0

Actor
{
    id: component

    property alias program: program_
    property real bodyLength : 5
    property real bodyWidth  : 1
    property real pointLength: 2
    property real pointWidth : 3
    property real z          : 0

    geometry: Geometry
    {
        //      ^y                4                                              
        //      |                 |   \                                            
        //      |                 |      \                                          
        //      6-----------------5         \                                       
        //  ____|______x_____________________3__________________                                       
        //      |                           /                                         
        //      0-----------------1      /                                                              
        //                        |     /                                             
        //                        |   /                                                 
        //                        2                                                   
        primitiveType: Geometry.TRIANGLES
        indices: ArrayUInt1
        {
            input: [0,1,6, 6,1,5, /*<---- body | arrow tip ----> */1,2,3, 1,3,5, 5,3,4]
        }
        attribs: Attribs
        {
            vertices : ArrayFloat3
            {
                input:
                [
                  [0                                         , -component.bodyWidth/2             , z] //0
                , [component.bodyLength                      , -component.bodyWidth/2             , z] //1
                , [component.bodyLength                      , -component.pointWidth/2            , z] //2
                , [component.bodyLength+component.pointLength, 0                                  , z] //3
                , [component.bodyLength                      , component.pointWidth/2             , z] //4
                , [component.bodyLength                      , component.bodyWidth/2              , z] //5
                , [0                                         , component.bodyWidth/2              , z] //6
                ]
            }
            normals: ArrayFloat3
            {
                input:
                [
                 [0,0,1] //0
                ,[0,0,1] //1
                ,[0,0,1] //2
                ,[0,0,1] //3
                ,[0,0,1] //4
                ,[0,0,1] //5
                ,[0,0,1] //6
                ]
            }
        }
    }
    transform: Rotation
    {
        parentTransform: timberActor_.transform
        quaternion: Transforms.qFromEuler(Math.PI/2,0,0)
    }

    effect: Effect
    {
        shader0: MaterialProgram{id: program_; }
    }
}