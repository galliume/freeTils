import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1
import Qt.labs.settings 1.0

import com.gcconantc.FbDetector 1.0
import com.gcconantc.FbDeployer 1.0

Window {
    id: root
    width: 1024
    height: 768
    visible: true
    title: qsTr("FreeTils")

    readonly property string labelColor: "#3C3C3B"
    readonly property string menuColor: "#EBEBD3"
    readonly property string fontFamily: "Helvetica"
    readonly property int fontPointSize: 8
    readonly property int menuHeight: 30

    Settings {
        id: settings
        property string rootDirProject : "click to select the root folder of your project"
    }

    FbDetector {
        id: fbDetect
    }

    FbDeployer {
        id: fbDeploy
    }

    Component.onCompleted: {
        fbDetect.scan();
    }

    Connections {
        target: fbDetect
        function onScanned(data) {
            lstIP.append({"ip": data })
        }
    }

    ListModel {
        id: lstIP
        ListElement { ip: "Select a Freebox" }
    }

    Row {
        id: folderRow
        spacing: 0

        Rectangle {
            width: 100
            height: root.menuHeight
            color: root.labelColor

            Text {
                text: "Selected folder"
                font.family: root.fontFamily
                font.pointSize: root.fontPointSize
                color: "white"
                width: parent.width
                height: parent.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            width: root.width - 100
            height: root.menuHeight
            color: root.menuColor

            Text {
                id: selectedRootProject
                text: settings.rootDirProject
                font.family: root.fontFamily
                font.pointSize: root.fontPointSize
                color: "black"
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    rootDirDialog.visible = true;
                }
            }
        }
    }

    Row {
        id: selectFbx
        anchors.top: folderRow.bottom
        spacing: 0

        Rectangle {
            id: status
            color: root.labelColor
            width: 100
            height: root.menuHeight

            Text {
                id:lblStatus
                text: "Freebox list"
                font.family: root.fontFamily
                font.pointSize: root.fontPointSize
                color: "white"
                anchors.centerIn: parent
            }
        }

        Rectangle {
           color: root.menuColor
           width: root.width - 100
           height: root.menuHeight

            ComboBox {
                id: lstFbx
                anchors.fill: parent
                model: lstIP
                currentIndex: 0
            }
        }
    }

    Row {
        id: deployRow
        anchors.top: selectFbx.bottom
        spacing: 0

        Rectangle {
            width: 100
            height: root.menuHeight
            color: root.labelColor

            Text {
                text: "Deploy status"
                font.family: root.fontFamily
                font.pointSize: root.fontPointSize
                color: "white"
                width: parent.width
                height: parent.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            width: 100
            height: root.menuHeight
            color: root.labelColor

            Button {
                id: deployBtn
                text: "Deploy"
                width: parent.width
                height: parent.height

                onClicked: {
                    if (0 !== lstFbx.currentIndex) {
                        console.log("Coucou" + lstFbx.currentValue);
                        fbDeploy.serve(selectedRootProject.text, lstFbx.currentValue);
                    }
                }
            }
        }

        Rectangle {
            color: root.menuColor
            width: root.width - 100
            height: root.menuHeight

            Text {
                text: "Not running"
                font.family: root.fontFamily
                font.pointSize: root.fontPointSize
                color: "red"
                width: parent.width
                height: parent.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    FolderDialog  {
        id: rootDirDialog
        title: "Please choose a folder"
        onAccepted: {
            selectedRootProject.text = rootDirDialog.folder;
            settings.setValue("rootDirProject", rootDirDialog.folder);
        }
        onRejected: {
            console.log("Canceled")
        }
    }
}
