import QtQuick 2.12
import QtQuick.Controls 2.12

import QtQuick.Dialogs 1.3
import QtQuick.Window 2.0

import QtQuick.Layouts 1.12



import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Universal 2.12

import Qt.labs.settings 1.0




ApplicationWindow {
    id: window
    visible: true
    width: 320
    height: 480
    title: qsTr("Stack")


    Component.onCompleted: {
        x = Screen.width / 2 - width / 2
        y = Screen.height / 2 - height / 2
    }

    Shortcut {
        sequence: StandardKey.Open
        onActivated: openDialog.open()
    }
    Shortcut {
        sequence: StandardKey.SaveAs
        onActivated: saveDialog.open()
    }
    Shortcut {
        sequence: StandardKey.Quit
        onActivated: Qt.quit()
    }

    Settings {
        id: settings
        property string style: "Default"
    }

    Shortcut {
        sequences: ["Esc", "Back"]
        enabled: stackView.depth > 1
        onActivated: {
            stackView.pop()
            listView.currentIndex = -1
        }
    }

    Shortcut {
        sequence: "Menu"
        onActivated: optionsMenu.open()
    }

    header: ToolBar {
        leftPadding: 8
        contentHeight: toolButton.implicitHeight
        Material.foreground: "white"

        Flow {
            id: flow
            width: parent.width

            Row {
                id: leftMenu
                ToolButton {
                    id: toolButton
                    text: stackView.depth > 1 ? "\u25C0" : "\u2630"
                    font.pixelSize: Qt.application.font.pixelSize * 1.6
                    onClicked: {
                        if (stackView.depth > 1) {
                            stackView.pop()
                        } else {
                            drawer.open()
                        }
                    }
                }
                ToolSeparator {
                    contentItem.visible: leftMenu.y === fileRow.y
                }
            }

            Row {
                id: fileRow
                ToolButton {
                    id: openButton
                    text: "\uF115" // icon-folder-open-empty
                    font.family: "fontello"
                    onClicked: openDialog.open()
                }

            }
        }
    }


    Drawer {
        id: drawer
        width: window.width * 0.66
        height: window.height

        Column {
            anchors.fill: parent

            ItemDelegate {
                text: qsTr("Ноты")
                width: parent.width
                onClicked: {
                    stackView.push(page1)
                    drawer.close()
                }
            }
        }
    }

    StackView {
        id: stackView
        initialItem: "HomeForm.qml"
        anchors.fill: parent

        Component {
            id: page1

            Page1Form {
                width: window.width
                height: window.height
            }
        }
    }

    FileDialog {
        id: openDialog
        //fileMode: FileDialog.OpenFile
       // selectedNameFilter.index: 1
        nameFilters: ["WAV file (*.WAV)"]
        //folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        //onAccepted: document.load(file)
        onAccepted: aubioReader.setUrl(openDialog.fileUrl)
    }

}
