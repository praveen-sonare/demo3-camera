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

#include "camera.h"
#include <QPainter>
#include <opencv2/imgproc.hpp>

#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

using namespace cv;

Camera::Camera() {
    running = false;
    timer = new QTimer(this);
    capture = new VideoCapture();

    connect(timer, SIGNAL(timeout()), this, SLOT(grab()));
}

Camera::~Camera() {
    if (timer->isActive())
        timer->stop();
    if (capture && capture->isOpened())
        capture->release();
    delete timer;
    delete capture;
}

void Camera::paint(QPainter *painter) {
    painter->drawImage(0, 0, image);
}

void Camera::enumerateCameras() {
    int maxID = 20;
    for (int idx = maxID; idx > 2; idx--){
        std::stringstream  no;
        no << "/dev/video" << idx;
        qDebug() << idx;
        int fd = open(no.str().c_str(), O_RDWR);
        if (fd != -1){
            struct v4l2_capability cap;

            if (ioctl(fd,VIDIOC_QUERYCAP,&cap) != -1){
                if ((cap.capabilities & (V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING)) == (V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING)){
                    qDebug()<< (char*)(cap.driver) << ">" << (char*)(cap.card) << ">" << (char*)(cap.bus_info);// << (cap.version>>16)&0xFF <<  (cap.version>>8)&0XFF << cap.version&0xFF;
                    cam_arr.push_back(idx);  // vector of all available cameras
                }
            }
            close(fd);
        }
    }

    camcnt = cam_arr.length();
}

void Camera::start(int no, int fps, QString res) {
    int retryCount = 0;
    while(!capture->open(no)){
        if(retryCount++==5){
            qDebug()<< "Try to open camera for 5 times failed, give up.";
            return;
        }else{
            qDebug()<< "open camera failed, retry " << retryCount;
            usleep(200000);
        }
    }

    capture->set(CAP_PROP_FRAME_WIDTH, res.section("*", 0, 0).toInt());
    capture->set(CAP_PROP_FRAME_HEIGHT, res.section("*", 1, 1).toInt());

    if (fps > 0){
        timer->start(1000/fps);
        running = true;
        emit isrunningChanged(running);
    }
}

void Camera::stop() {
    if (timer->isActive())
        timer->stop();
    if (capture->isOpened()){
        qDebug()<< "release camera.";
        capture->release();
    }
    image = QImage();
    update();
    running = false;
    emit isrunningChanged(running);
}

QVariantList Camera::camranum() const{
    return cam_arr;
}

int Camera::cameraCnt() {
    return camcnt;
}

bool Camera::isrunning() const{
    return running;
}

void Camera::grab() {
    if (capture && capture->isOpened()){
        Mat frame;
        capture->read(frame);

        if (!frame.empty()){
            image = QImage((const uchar*)(frame.data), frame.cols, frame.rows, frame.step, QImage::Format_RGB888).rgbSwapped();
            image = image.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            update();
        }
    }
}