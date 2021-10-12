import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.12

import com.gcconantc.FbDetector 1.0
import com.gcconantc.FbDeployer 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("FreeTils")

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

    Rectangle {
        id: status
        color: "gray"
        width: 300
        height: 50
        x:0
        y:0

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
                            fbDeploy.serve(ip);
                        }
                    }
                }
            }

    }

    ListModel {
        id: lstIP
    }
}
