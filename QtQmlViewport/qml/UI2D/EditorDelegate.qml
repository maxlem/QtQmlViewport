import QtQuick 2.0

Rectangle {
    id: root
    property string text
    property bool mode: false
    Component{
        id: component_display
        Text{}
    }
    Component{
        id: component_edit
        TextInput{}
    }

    property var callback

    Loader{
        id: loader
        anchors.fill: parent
        sourceComponent: mode ? component_edit: component_display
        onSourceComponentChanged: {
            loader.item.text = root.text
            if(sourceComponent === component_edit){
                loader.item.editingFinished.connect(onEditingFinished)
                loader.item.forceActiveFocus()
            }
        }
        function onEditingFinished(){
            mode = false
            if (root.callback != "undefined")
                root.callback(loader.item.text)
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