/*
 * Copyright (c) 2019 TOYOTA MOTOR CORPORATION
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Window 2.2
import Camera 1.0
import QtWebSockets 1.0

ApplicationWindow {
    id: root
    width: 1920
    height: 720

    Camera {
        id: camdev_device
        anchors.horizontalCenter: parent.horizontalCenter
        width: height * cameracontrol.ratio
        height: root.height
        anchors.top: parent.top
        onIsrunningChanged: {
            camerabg.visible = !isrunning
        }

        Image {
            id: camerabg
            anchors.centerIn: parent
            width: 200
            height: 200
            source: "images/camera/camera_bg.svg"
        }
    }

    CameraControl {
        id:cameracontrol
        anchors.horizontalCenter: parent.horizontalCenter
        ratio: 4/3
        device: camdev_device
        width: camdev_device.width
        height:80
    }

    function changeVisible(visible) {
        console.log("camera visible is " + visible)
        cameracontrol.switchChecked = visible
    }
}
