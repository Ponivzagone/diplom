import QtQuick 2.12
import QtQuick.Controls 2.12

import QtQuick.Layouts 1.12


ScrollablePage {
    id: page

    Column {
        spacing: 20
        width: parent.width
        Frame {
            id: frameFirst
            anchors.horizontalCenter: parent.horizontalCenter
            Column {
               anchors.horizontalCenter: parent.horizontalCenter
               spacing: 5
               Row {
                   ComboBox {
                       id: comboSample
                       model: ["8000", "16000", "20500","41000"]
                   }
                   Text {
                       padding: comboSample.height / 4
                       font.pixelSize: 18
                       wrapMode: Label.Wrap
                       horizontalAlignment: Qt.AlignHCenter
                       text: "sample rate"
                   }
               }

               Row {
                   ComboBox {
                       id: comboBit
                       model: ["16", "24"]
                   }
                   Text {
                       padding: comboBit.height / 4
                       font.pixelSize: 18
                       wrapMode: Label.Wrap
                       horizontalAlignment: Qt.AlignHCenter
                       text: "bit rate"
                   }
               }

               Row {

                   ComboBox {
                       id: comboFrame
                       model: ["1024", "2048", "4196","8152"]
                   }
                   Text {
                       padding: comboFrame.height / 4
                       font.pixelSize: 18
                       wrapMode: Label.Wrap
                       horizontalAlignment: Qt.AlignHCenter
                       text: "frame size"
                   }
               }
            }
        }
        Frame {
            id: frameSecond
            anchors.horizontalCenter: parent.horizontalCenter
            Column {
                Text {
                    padding: comboSample.height / 4
                    font.pixelSize: 18
                    wrapMode: Label.Wrap
                    horizontalAlignment: Qt.AlignHCenter
                    text: "note probability"
                }

                Row {
                    SpinBox {
                          id: spinbox
                          from: 0
                          value: 070
                          to: 100 * 100
                          stepSize: 1

                          property int decimals: 2
                          property real realValue: value / 100

                          validator: DoubleValidator {
                              bottom: Math.min(spinbox.from, spinbox.to)
                              top:  Math.max(spinbox.from, spinbox.to)
                          }

                          textFromValue: function(value, locale) {
                              return Number(value / 100).toLocaleString(locale, 'f', spinbox.decimals)
                          }

                          valueFromText: function(text, locale) {
                              return Number.fromLocaleString(locale, text) * 100
                          }
                      }
                }
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            id: button
            text: "Start"
            highlighted: true
            Layout.fillWidth: true
            onClicked: function() {
                aubioReader.setNoteProb(Number(spinbox.value / 100))
                aubioReader.startRecord()
            }
        }
    }
}
