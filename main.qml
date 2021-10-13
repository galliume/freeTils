import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import Qt.labs.settings 1.0

import com.gcconantc.FbDetector 1.0
import com.gcconantc.FbDeployer 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("FreeTils")

    Settings {
        id: settings
    }

    FbDetector {
        id: fbDetect
    }

    FbDeployer {
        id: fbDeploy
    }

    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2);
        setY(Screen.height / 2 - height / 2);
        fbDetect.scan();
        selectedRootProject.text = settings.value("rootDirProject", "click to select the root folder of your project");
    }

    Connections {
        target: fbDetect
        function onScanned(data) {
            lstIP.append({"ip": data })
        }
    }

    ListModel {
        id: lstIP
    }

    Rectangle {
        id: rootProjectTxt
        color: "gray"
        width: parent.width
        height: 30
        border.color: "black"
        border.width: 1

        Text {
            id:selectedRootProject
            font.family: "Helvetica"
            font.pointSize: 8
            color: "white"
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                rootDirDialog.visible = true;
            }
        }
    }

    Rectangle {
        id: status
        color: "gray"
        width: 300
        height: 50
        anchors.top: rootProjectTxt.bottom
        border.color: "black"
        border.width: 1

        Text {
            id:lblStatus
            text: "Freebox list"
            font.family: "Helvetica"
            font.pointSize: 12
            color: "white"
            anchors.centerIn: parent
        }
    }

    Rectangle {
       color: "lightgray"
       anchors.top: status.bottom

       width: 300
       height: 400

            ListView {
                id: lstView
                anchors.fill: parent
                model: lstIP

                delegate: Rectangle {
                    id: rect
                    width: parent.width
                    height: 20
                    color: "beige"
                    border.color: "black"
                    border.width: 1

                    Text {
                        id: txt
                        anchors.centerIn: parent
                        font.pointSize: 10
                        text: ip
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            fbDeploy.serve(settings.value("rootDirProject"), ip);
                        }
                    }
                }
            }
    }

    FileDialog {
        id: rootDirDialog
        title: "Please choose a folder"
        folder: shortcuts.home
        selectFolder: true
        onAccepted: {
            settings.setValue("rootDirProject", rootDirDialog.fileUrl);
        }
        onRejected: {
            console.log("Canceled")
        }
    }
}
