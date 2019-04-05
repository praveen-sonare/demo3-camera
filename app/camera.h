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

#ifndef CAMERA_H
#define CAMERA_H

#include <QSize>
#include <QTimer>
#include <QImage>
#include <QQuickPaintedItem>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

static QVariantList cam_arr_bak;

class Camera : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(bool isrunning READ isrunning NOTIFY isrunningChanged)
public:
    Camera();
    ~Camera();
    void paint(QPainter *painter);
    bool isrunning() const;
    Q_INVOKABLE QVariantList camranum() const;

    Q_INVOKABLE void start(int no, int fps, QString res);
    Q_INVOKABLE void stop();

    Q_INVOKABLE void enumerateCameras();
    Q_INVOKABLE int cameraCnt();

signals:
    void isrunningChanged(const bool& isrunning);

public slots:
    void grab();

private:
    bool running;
    QImage image;
    QTimer* timer;
    cv::VideoCapture* capture;

    QVariantList cam_arr;
    int camcnt;
};

#endif // CAMERA_H
