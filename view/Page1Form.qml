import QtQuick 2.12
import QtQuick.Controls 2.5

Page {

    title: qsTr("Page 1")

    ScrollView {

        clip: true
        width: parent.width
        height: parent.height

        Image {
            id: liveImage

            property bool counter: false

            asynchronous: true
            source: "image://live/image"
            //        anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            cache: false

            function reload() {
                counter = !counter
                source = "image://live/image?id=" + counter
            }
        }
    }
}
