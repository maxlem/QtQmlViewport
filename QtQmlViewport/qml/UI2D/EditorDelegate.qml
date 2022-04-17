import QtQuick 2.14

Rectangle {
    id: root
    property string text
    property bool mode: false
    Component{
        id: component_display
        Text{text: root.text}
    }
    Component{
        id: component_edit
        TextInput
        {
            text: root.text
            onEditingFinished:
            {
                root.mode = false
                if (root.callback != "undefined")
                    root.callback(text)
            }
        }
    }

    property var callback

    Loader{
        id: loader
        anchors.fill: parent
        sourceComponent: mode ? component_edit: component_display
        onSourceComponentChanged: {
            if(sourceComponent === component_edit){
                loader.item.forceActiveFocus()
            }
        }
        MouseArea{
            anchors.fill: parent
            onDoubleClicked: {
                if (mouse.button & Qt.LeftButton)
                    mode = true
            }
        }
    }
}