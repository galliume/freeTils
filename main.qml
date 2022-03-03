import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1
import Qt.labs.settings 1.0

import com.galliume.FreeTilsApp 1.0

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

    Settings {
        id: settings
        property string rootDirProject: "click to select the root folder of your project"
        property int selectedBox: 0
        property string miniIP: "192.168.1.9"
        property string miniWsPort: "8282"
        property string miniID: "appliID"
    }

    FreeTilsApp {
        id: freeTilsApp
    }

    Component.onCompleted: {
        freeTilsApp.detectDevices();
    }

    Component.onDestruction: {
        freeTilsApp.stop();
    }

    Connections {
        target: freeTilsApp
        function onRefreshStbList(deviceAddress, iconAdress) {
            lstIP.append({"ip": deviceAddress })
        }
    }

    Connections {
        target: freeTilsApp
        function onServerUpdated(isOk, status) {
            //@todo to rework
            deployStatus.color = (isOk) ? "green" : "red";
            deployStatus.text = status;
            deployBtn.visible = (isOk) ? false : true;
            stopBtn.visible = (isOk) ? true : false;
        }
    }

    Connections {
        target: freeTilsApp
        function onLogged(out, lvl) {

            var color = "black";
            var background = "white";
            var fontWeight = "";

            if ("err" === lvl) {
                color = "white";
                background = "red";
                fontWeight = "font-weight:bold";
            }

            var log = `<p style=\"background-color:${background};color:${color}\;${fontWeight}">`;
            log += out;
            log += "</p> "

            logs.append(log);
        }
    }

    ListModel {
        id: lstIP

        ListElement {
            ip: "Select a Freebox"
//            icon: ""
        }
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
                leftPadding: 270
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
            width: 100
            height: root.menuHeight
            color: root.labelColor

            Button {
                id: refreshBtn
                text: "Refresh"
                width: parent.width
                height: parent.height
                onClicked: {
                    freeTilsApp.detectDevices();
                }
            }
        }

        Rectangle {
           color: root.menuColor
           width: root.width - 200
           height: root.menuHeight

            ComboBox {
                id: lstFbx
                leftPadding: 270
                anchors.fill: parent
                model: lstIP
//                delegate: ItemDelegate {
//                    Row {
//                        spacing: 5
//                        Image { source: icon }
//                        Text { text: ip }
//                    }
//                }
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
            width: 220
            height: root.menuHeight
            color: root.labelColor

            Button {
                id: launchBtn
                text: "Launch"
                width: parent.width - 120
                height: parent.height
                onClicked: {
                    freeTilsApp.launchQmlScene(selectedRootProject.text);
                }
            }

            Button {
                id: deployBtn
                text: "Deploy on fbx"
                width: parent.width - 100
                height: parent.height
                anchors.left: launchBtn.right
                onClicked: {
                    if (0 !== lstFbx.currentIndex) {
                        settings.selectedBox = lstFbx.currentIndex;
                        freeTilsApp.deployApp(selectedRootProject.text, lstFbx.currentIndex);
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
                width: parent.width - 100
                height: parent.height
                anchors.left: launchBtn.right
                onClicked: {
                    freeTilsApp.stop();
                    deployBtn.visible = true;
                    stopBtn.visible = false;
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

    Row {
        id: mini
        spacing: 0
        anchors.top: deployRow.bottom

        Rectangle {
            width: 100
            height: root.menuHeight
            color: root.labelColor

            Text {
                text: "Mini 4K"
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
            color: "white"

            TextInput {
                id: ipMini4k
                text: settings.miniIP
                width: 150
                height: parent.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            TextInput {
                id: nameActivity
                text: settings.miniID
                width: 150
                height: parent.height
                anchors.left: ipMini4k.right
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            TextInput {
                id: wsPort
                text: settings.miniWsPort
                width: 150
                height: parent.height
                anchors.left: nameActivity.right
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Button {
                id: connectADB
                text: "Connect ADB"
                width: 150
                height: parent.height
                anchors.left: wsPort.right
                onClicked: {
                    settings.miniIP = ipMini4k.text;
                    freeTilsApp.connectADB(ipMini4k.text);
                }
            }

            Button {
                id: startMini
                text: "Start on mini 4k"
                width: 150
                height: parent.height
                anchors.left: connectADB.right
                onClicked: {
                    settings.miniID = nameActivity.text;
                    freeTilsApp.startMini(ipMini4k.text, nameActivity.text);
                }
            }         

            Button {
                id: deployminiBtn
                text: "Deploy on mini 4k"
                width: 150
                height: parent.height
                anchors.left: startMini.right
                onClicked: {
                    settings.miniWsPort = wsPort.text;
                    freeTilsApp.deployAppMini(ipMini4k.text, wsPort.text);
                }
            }
        }
    }

    Rectangle {
        id: content
        anchors.top: mini.bottom
        width: root.width
        height: root.height - folderRow.height - deployRow.height - selectFbx.height - mini.height
        border.width: 3
        border.color: contentColor

        ScrollView {
            anchors.fill: parent

            TextArea {
                id: logs
                textFormat: Text.RichText
                readOnly: true
                selectByMouse: true
                selectByKeyboard: true
                wrapMode: TextEdit.Wrap
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
