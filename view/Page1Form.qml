import QtQuick 2.12
import QtQuick.Controls 2.5

Page {

    title: qsTr("Page 1")
    property int transformX:0;
    property int transformY:0;
    property real defaultSize: 800
     property int highestZ: 0



   id: root
        clip: true
        width: parent.width
        height: parent.height





    Rectangle {
        id: photoFrame
        width: image.width * (1 + 0.10 * image.height / image.width)
        height: image.height * 1.10
        scale: defaultSize / Math.max(image.sourceSize.width, image.sourceSize.height)
        Behavior on scale { NumberAnimation { duration: 200 } }
        Behavior on x { NumberAnimation { duration: 200 } }
        Behavior on y { NumberAnimation { duration: 200 } }
        border.color: "black"
        border.width: 2
        smooth: true
        antialiasing: true
        Component.onCompleted: {
            x = 0
            y = 0
            rotation = 0
        }
        Image {
            id: image

            property bool counter: false
            anchors.centerIn: parent
            asynchronous: true
            source: "image://live/image"
            antialiasing: true;
            fillMode: Image.PreserveAspectFit
            cache: false


            function reload() {

                counter = !counter
                source = "image://live/image?id=" + counter
            }

        }
        PinchArea {
            anchors.fill: parent
            pinch.target: photoFrame
            pinch.minimumRotation: -360
            pinch.maximumRotation: 360
            pinch.minimumScale: 0.1
            pinch.maximumScale: 10
            pinch.dragAxis: Pinch.XAndYAxis
            property real zRestore: 0
            onSmartZoom: {
                if (pinch.scale > 0) {
                    photoFrame.rotation = 0;
                    photoFrame.scale = Math.min(root.width, root.height) / Math.max(image.sourceSize.width, image.sourceSize.height) * 0.85
                    photoFrame.x = flick.contentX + (flick.width - photoFrame.width) / 2
                    photoFrame.y = flick.contentY + (flick.height - photoFrame.height) / 2
                    zRestore = photoFrame.z
                    photoFrame.z = ++root.highestZ;
                } else {
                    photoFrame.rotation = pinch.previousAngle
                    photoFrame.scale = pinch.previousScale
                    photoFrame.x = pinch.previousCenter.x - photoFrame.width / 2
                    photoFrame.y = pinch.previousCenter.y - photoFrame.height / 2
                    photoFrame.z = zRestore
                    --root.highestZ
                }
            }

            MouseArea {
                id: dragArea
                hoverEnabled: true
                anchors.fill: parent
                drag.target: photoFrame
                scrollGestureEnabled: false  // 2-finger-flick gesture should pass through to the Flickable
                onPressed: {
                    photoFrame.z = ++root.highestZ;
                }
                onWheel: {
                    if (wheel.modifiers & Qt.ControlModifier) {
                        photoFrame.rotation += wheel.angleDelta.y / 120 * 5;
                        if (Math.abs(photoFrame.rotation) < 4)
                            photoFrame.rotation = 0;
                    } else {
                        photoFrame.rotation += wheel.angleDelta.x / 120;
                        if (Math.abs(photoFrame.rotation) < 0.6)
                            photoFrame.rotation = 0;
                        var scaleBefore = photoFrame.scale;
                        photoFrame.scale += photoFrame.scale * wheel.angleDelta.y / 120 / 10;
                    }
                }
            }
        }
    }


    Connections {
        target: liveImageProvider
        onUpdateImage: liveImage.reload()
    }

}
