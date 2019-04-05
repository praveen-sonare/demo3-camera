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

RowLayout {
    property var device
    property var ratio: 4/3
    property var number: []
    property var fps: ["10", "20", "30", "40", "50", "60"]
    property var resolution: ["320*240", "640*480", "1280*720", "1920*1080"]
    property var models: [number, fps, resolution]
    property real back: 1
    property bool switchstatus: true
    property bool switchChecked: false

    Repeater {
        id: info
        model: ["No:", "FPS:", "W*H:"]
        delegate: RowLayout{
            Label {
                id: label
                text: model.modelData
                color: "#59FF7F"
            }
            ComboBox {
                id: camerainfo
                implicitWidth: index != 2 ? 100 : 245
                font: label.font
                model: models[index]
                contentItem: Text {
                    text: camerainfo.displayText
                    font: camerainfo.font
                    color: camerainfo.pressed ? "#17a81a" : "white"
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                indicator: Canvas {
                    id: canvas
                    x: camerainfo.width - width - camerainfo.rightPadding
                    y: camerainfo.topPadding + (camerainfo.availableHeight - height) / 2
                    width: 20
                    height: 12
                    contextType: "2d"

                    Connections {
                        target: camerainfo
                        onPressedChanged: canvas.requestPaint()
                    }

                    onPaint: {
                        context.reset();
                        context.moveTo(0, 0);
                        context.lineTo(width, 0);
                        context.lineTo(width / 2, height);
                        context.closePath();
                        context.fillStyle = camerainfo.pressed ? "#17a81a" : "white";
                        context.fill();
                    }
                }
                popup: Popup {
                    id: popup
                    y: camerainfo.height - 1
                    implicitWidth: camerainfo.width
                    implicitHeight: listview.count > 6 ? (listview.contentHeight/3.3) : listview.contentHeight
                    padding: 0

                    contentItem: ListView {
                        id: listview
                        clip: true
                        model: camerainfo.visible ? camerainfo.delegateModel : null
                        currentIndex: camerainfo.highlightedIndex
                        ScrollIndicator.vertical: ScrollIndicator { }
                    }
                    background: Image { source: "images/camera/camerainfo_bg.svg" }
                }
                delegate: ItemDelegate {
                    id: popupdelegate
                    width: camerainfo.width
                    contentItem: Item {
                        implicitHeight: 30
                        Text {
                            text: modelData
                            color: popupdelegate.pressed ||  highlighted ? "#21be2b" : "white"
                            font: camerainfo.font
                            elide: Text.ElideRight
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                    highlighted: camerainfo.highlightedIndex == index
                }

                background: Image { source: "images/camera/camerainfo_bg.svg" }
                onCurrentIndexChanged:
                    if(cameraswitch.checked && device){
                        var dNo = info.itemAt(0).children[1].currentText
                        var dFps = info.itemAt(1).children[1].currentText
                        var dRes = info.itemAt(2).children[1].currentText
                        if (index === 0){
                            dNo = number[listview.currentIndex]
                        }
                        if (index === 1){
                            dFps = fps[listview.currentIndex]
                        }
                        if (index === 2){
                            dRes = resolution[listview.currentIndex]
                            if(listview.currentIndex === 0 || listview.currentIndex === 1){
                                ratio = 4/3
                            }
                            else{
                                ratio = 16/9
                            }
                        }
                        device.stop()
                        device.start(dNo, dFps, dRes)
                    }
            }
        }
    }
    Switch {
        id: cameraswitch
        enabled: switchstatus
        checked: switchChecked
        onCheckedChanged: {
            if (info.itemAt(2).children[1].currentText === "320*240" || info.itemAt(2).children[1].currentText === "640*480"){
                ratio = 4/3
            }
            else{
                ratio = 16/9
            }
            if (checked && device){
                device.start(info.itemAt(0).children[1].currentText, info.itemAt(1).children[1].currentText, info.itemAt(2).children[1].currentText)
            }else if (!checked && device){
                device.stop()
            }
        }
    }

    Component.onCompleted: {
        device.enumerateCameras();
        number = device.camranum();

        if (device.cameraCnt() === 0)
            switchstatus = false;
    }
}
