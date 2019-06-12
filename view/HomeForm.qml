import QtQuick 2.12
import QtQuick.Controls 2.12

import QtQuick.Layouts 1.12


ScrollablePage {
    id: page

    GridLayout {
        id: grid
        columns: 3
        Column {
            Label {
                width: parent.width
                wrapMode: Label.Wrap
                horizontalAlignment: Qt.AlignHCenter
                text: "Выберете SampleRate"
            }

            ComboBox {
                model: ["8000", "16000", "20500","41000"]
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        Column {
            Label {
                width: parent.width
                wrapMode: Label.Wrap
                horizontalAlignment: Qt.AlignHCenter
                text: "Выберете ByteRate"
            }

            ComboBox {
                model: ["16", "24"]
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        Column {
            Label {
                width: parent.width
                wrapMode: Label.Wrap
                horizontalAlignment: Qt.AlignHCenter
                text: "Выберете frame_size"
            }

            ComboBox {
                model: ["1024", "2048", "4196","8152"]
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }


        Column {

            Button {
                id: button
                text: "Начать запись"
                highlighted: true
                Layout.fillWidth: true
                onClicked: aubioReader.startRecord()
            }
        }

        Column {

            Button {
                id: button2
                text: "Остановить запись"
                highlighted: true
                Layout.fillWidth: true
                //onClicked: aubioReader.startRecord()
            }
        }
    }
}
