Item
{
    id: component

    width: 800
    height: 600
    property alias actors : actors_

    //// this is necessary for InteractiveHelper.show()'s 'wait key' code:
    focus: true
    property string key: ''
    Keys.onPressed: key = event.text
    /// End InteractiveHelper.show() API

    Viewport
    {
        anchors.fill: parent
        actors: Actors
        {
            id: actors_
            XYZ{scale:5}
        }
    }

}