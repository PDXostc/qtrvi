/*****************************************************************
 *
 * (C) 2017 Jaguar Land Rover - All Rights Reserved
 *
 * This program is licensed under the terms and conditions of the
 * Mozilla Public License, version 2.0.  The full text of the
 * Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
 *
******************************************************************/

import QtQuick 2.6
import QtQuick.Window 2.2

import RviNode 1.0

import com.jlr.rviserviceping 1.0

Window {
    id: root
    visible: true
    width: 640
    height: 480

    property string demoFullyQualifiedServiceName: "genivi.org/client/bbfbb478-d628-480a-8528-cff40d73678f/"

    Connections {
        target: root

        onClosing: node.nodeCleanup()
    }


    Rectangle {
        id: pingerButton

        width: parent.width * 0.66
        height: parent.height * 0.66
        anchors.centerIn: parent

        border.color: "gray"
        border.width: 5

        radius: 25

        color: mouse.pressed ? "light gray" : "dark gray"

        Text {
            id: pingText
            anchors.centerIn: parent
            font.pointSize: 14
            color: "#CCCCCC"
            text: "Ping!"
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            onClicked: node.invokeService(demoFullyQualifiedServiceName + "ponger", "{}")
        }
    }

    RviNode {
        id: node

        Component.onCompleted: node.nodeInit()

        onInitSuccess: {
            var fd = node.nodeConnect();
            if (fd >= 0) {
                pinger.socket = fd;
            }
        }
        onInitError: console.log("Error initializing rvi_lib, check example credentials")

        onRemoteConnectionError: console.log("Error attempting to connect to remote node, check connection used")
        onRemoteNodeConnected: node.registerService(pinger.serviceName, pinger)

        onRegisterServiceError: console.log("Error registering service with rvi node")
        onRegisterServiceSuccess: console.log("The Pinger is alive!")
    }

    RviServicePing {
        id: pinger

        serviceName: "pinger"

        onDestroyRviService: {
            Qt.quit();
        }
    }
}
