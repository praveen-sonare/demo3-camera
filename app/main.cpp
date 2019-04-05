/*
 * Copyright (c) 2019 TOYOTA MOTOR CORPORATION
 * Copyright (C) 2016 The Qt Company Ltd.
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

#include <QtCore/QDebug>
#include <QtCore/QCommandLineParser>
#include <QtCore/QUrlQuery>
#include <QtCore/QFile>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuickControls2/QQuickStyle>
#include <unistd.h>
#include "camera.h"

#include <QQuickWindow>

#ifdef HAVE_LIBHOMESCREEN
#include <libhomescreen.hpp>
#endif
#ifdef HAVE_QLIBWINDOWMANAGER
#include <qlibwindowmanager.h>
#endif

int main(int argc, char *argv[])
{
    QString myname = QString("camera");

    QGuiApplication app(argc, argv);
    app.setApplicationName(myname);
    app.setApplicationVersion(QStringLiteral("0.1.0"));
    app.setOrganizationDomain(QStringLiteral("automotivelinux.org"));
    app.setOrganizationName(QStringLiteral("AutomotiveGradeLinux"));

    QQuickStyle::setStyle("AGL");

    QCommandLineParser parser;
    parser.addPositionalArgument("port", app.translate("main", "port for binding"));
    parser.addPositionalArgument("secret", app.translate("main", "secret for binding"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);
    QStringList positionalArguments = parser.positionalArguments();

    qmlRegisterType<Camera>("Camera", 1, 0, "Camera");

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();
    QUrl bindingAddress;
    int port = 0;
    QString secret;
    if (positionalArguments.length() == 2) {
        port = positionalArguments.takeFirst().toInt();
        secret = positionalArguments.takeFirst();
        bindingAddress.setScheme(QStringLiteral("ws"));
        bindingAddress.setHost(QStringLiteral("localhost"));
        bindingAddress.setPort(port);
        bindingAddress.setPath(QStringLiteral("/api"));
        QUrlQuery query;
        query.addQueryItem(QStringLiteral("token"), secret);
        bindingAddress.setQuery(query);
        context->setContextProperty(QStringLiteral("bindingAddress"), bindingAddress);
    } else {
        context->setContextProperty(QStringLiteral("bindingAddress"), bindingAddress);
    }

#ifdef HAVE_QLIBWINDOWMANAGER
    // WindowManager
    QLibWindowmanager* qwm = new QLibWindowmanager();
    if(qwm->init(port,secret) != 0){
        exit(EXIT_FAILURE);
    }
    // Request a surface as described in layers.json windowmanager’s file
    if (qwm->requestSurface(myname) != 0) {
        exit(EXIT_FAILURE);
    }
    // Create an event callback against an event type. Here a lambda is called when SyncDraw event occurs
    qwm->set_event_handler(QLibWindowmanager::Event_SyncDraw, [qwm, myname](json_object *object) {
        fprintf(stderr, "Surface got syncDraw!\n");
        qwm->endDraw(myname);
    });
#endif

#ifdef HAVE_LIBHOMESCREEN
    // HomeScreen
    LibHomeScreen* hs = new LibHomeScreen();
    std::string token = secret.toStdString();
    hs->init(port, token.c_str());
    // Set the event handler for Event_TapShortcut which will activate the surface for windowmanager
    hs->set_event_handler(LibHomeScreen::Event_TapShortcut, [qwm, myname](json_object *object){
        qwm->activateSurface(myname);
    });
#endif
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QObject *root = engine.rootObjects().first();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(root);
#ifdef HAVE_QLIBWINDOWMANAGER
    QObject::connect(window, SIGNAL(frameSwapped()), qwm, SLOT(slotActivateSurface()));

    qwm->set_event_handler(QLibWindowmanager::Event_Visible, [qwm, root](json_object *object) {
        fprintf(stderr, "Surface got Visible!\n");
        QMetaObject::invokeMethod(root, "changeVisible", Q_ARG(QVariant, true));
    });

    qwm->set_event_handler(QLibWindowmanager::Event_Invisible, [qwm, root](json_object *object) {
        fprintf(stderr, "Surface got Invisible!\n");
        QMetaObject::invokeMethod(root, "changeVisible", Q_ARG(QVariant, false));
    });
#else
    window->resize(1280, 720);
    window->setVisible(true);
#endif

    return app.exec();
}
