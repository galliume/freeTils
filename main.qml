import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1
import Qt.labs.settings 1.0

import com.galliume.FbDetector 1.0
import com.galliume.FbDeployer 1.0

Window {
    id: root
    width: 1024
    height: 768
    visible: true
    title: qsTr("FreeTils")

    readonly property string labelColor: "#264653"
    readonly property string menuColor: "#e9c46a"
    readonly property string contentColor: "#264653"
    readonly property string fontFamily: "Helvetica"
    readonly property int fontPointSize: 8
    readonly property int menuHeight: 30
    property string hostIp

    Settings {
        id: settings
        property string rootDirProject: "click to select the root folder of your project"
        property int selectedBox: 0
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

    Connections {
        target: fbDetect
        function onHostIpFounded(ip) {
            root.hostIp = ip;
        }
    }

    Connections {
        target: fbDeploy
        function onDeployed(isDeployed, status) {
            if (isDeployed) {
                deployStatus.color = "green";
                deployBtn.visible = false;
                stopBtn.visible = true;
            } else {
                deployStatus.color = "red";
                deployBtn.visible = true;
                stopBtn.visible = false;
            }

            deployStatus.text = status;
        }
    }

    Connections {
        target: fbDeploy
        function onStoped(isStoped, status) {
            if (isStoped) {
                deployBtn.visible = true;
                stopBtn.visible = false;
                deployStatus.color = "green";
                deployStatus.text = status;
            } else {
                deployStatus.color = "red";
                deployStatus.text = status;
            }
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
                leftPadding: 150
                text: settings.rootDirProject
                font.family: root.fontFamily
                font.pointSize: root.fontPointSize
                color: "black"
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignVCenter
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
                id: lblStatus
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
                leftPadding: 150
                anchors.fill: parent
                model: lstIP
                currentIndex: settings.selectedBox
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
                        settings.selectedBox = lstFbx.currentIndex;
                        fbDeploy.serve(selectedRootProject.text, lstFbx.currentValue, root.hostIp);
                    } else {
                        deployStatus.text = "Please select a Freebox";
                        deployStatus.color = "red";
                    }
                }
            }
            Button {
                id: stopBtn
                text: "Stop"
                visible: false
                width: parent.width
                height: parent.height
                onClicked: {
                    fbDeploy.stop();
                }
            }
        }

        Rectangle {
            color: root.menuColor
            width: root.width - 200
            height: root.menuHeight

            Text {
                id: deployStatus
                leftPadding: 50
                text: "Not running"
                font.family: root.fontFamily
                font.pointSize: root.fontPointSize
                color: "red"
                width: parent.width
                height: parent.height
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    Rectangle {
        id: content
        anchors.top: deployRow.bottom
        width: root.width
        height: root.height - folderRow.height - deployRow.height - selectFbx.height
        border.width: 5
        border.color: contentColor
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
